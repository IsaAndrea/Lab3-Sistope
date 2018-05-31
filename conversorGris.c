#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h> 
#include <stdint.h>
#include <unistd.h>
#include <sys/wait.h>
#include "cabecerasYfunciones.h"

/* Función que aplica una fórmula a los pixeles definidos dentro de una estructura que contiene todos los pixeles de una imagen 
    procesada.
   ENTRADA:
        - Estructura de una cabecera de informacion de un bitmap.
        - Un arreglo de pixeles que representa a la imagen.
   SALIDA:
        - Una matriz de pixeles checkeados.
*/
unsigned char *transformarAGrises(cabeceraInformacion *binformacion, unsigned char *data_imagen){
    int filas, colorGrisaseo;
    unsigned char *grisaseos;
    grisaseos = (unsigned char *)malloc(binformacion->tamanoImagen * sizeof(unsigned char));
    for(filas = 0; filas <binformacion->tamanoImagen; filas = filas + 4){
        colorGrisaseo = data_imagen[filas+2] * 0.3 + data_imagen[filas+1] * 0.59 + data_imagen[filas] * 0.11;
        grisaseos[filas+3] = 255;
        grisaseos[filas+2] = colorGrisaseo;
        grisaseos[filas+1] = colorGrisaseo;
        grisaseos[filas] = colorGrisaseo;
    }
    return grisaseos;
}


/* Función principal de conversorGris. Define el pid del hijo a crear y una tubería, para luego realizar la lectura de la tubería
    de lectorImagen, obteniendo los datos correspondientes de las cabeceras binformacion y bcabecera, así como de data_imagen.
    De esta forma, es posible asignar los parámetros necesarios a transformarAGrises.
    Una vez creado el hijo, éste procede a ejecutar binarizadorImagen. Por otro lado, el padre escribe en la tubería los datos 
    correspondientes a las cabeceras binformacion y bcabecera. Además, escribe en la tubería cada caracter de data_grisaseos.
*/
int main(int argc,char *argv[]) {
    pid_t pid;
    int status;
    int tuberiaC[2];
    unsigned char *data_grisaseos;
    cabeceraInformacion binformacion;
    cabeceraArchivo bcabecera;
    int tuberia = atoi(argv[3]);
    char *cambio = (char *)malloc(2 * sizeof(char));
    //lectura de la data en el pipe
    read(tuberia,&bcabecera.tamano,sizeof(uint32_t));
    read(tuberia,&bcabecera.reservado1,sizeof(uint16_t));
    read(tuberia,&bcabecera.reservado2,sizeof(uint16_t));
    read(tuberia,&bcabecera.offsetBit,sizeof(uint32_t));

    read(tuberia,&binformacion.alto,sizeof(uint32_t));
    read(tuberia,&binformacion.ancho,sizeof(uint32_t));
    read(tuberia,&binformacion.coloresImportantes,sizeof(uint32_t));
    read(tuberia,&binformacion.colorPixel,sizeof(uint16_t));
    read(tuberia,&binformacion.compresion,sizeof(uint16_t));
    read(tuberia,&binformacion.direcciones,sizeof(uint32_t));
    read(tuberia,&binformacion.tamano,sizeof(uint32_t));
    read(tuberia,&binformacion.tamanoImagen,sizeof(uint32_t));
    read(tuberia,&binformacion.totalBit,sizeof(uint32_t));
    read(tuberia,&binformacion.XResolporMetros,sizeof(uint32_t));
    read(tuberia,&binformacion.YResolporMetros,sizeof(uint32_t));

    unsigned char *data_imagen = (unsigned char*)malloc(binformacion.tamanoImagen* sizeof(unsigned char));
    for(int i = 0;i < binformacion.tamanoImagen;i++){
        read(tuberia,&data_imagen[i],sizeof(unsigned char));
    }
    data_grisaseos = transformarAGrises(&binformacion,data_imagen);
    //Los parametros que son recibidos como char, se transforma en enteros para su posterior utilizacion
    pipe(tuberiaC);
    pid = fork();
    if (pid < 0){
        printf("Error al crear proceso hijo \n");
        return 0;
    }
    if(pid == 0){
        sprintf(cambio,"%d",tuberiaC[0]);
        char *arreglos[] = {argv[0],argv[1],argv[2],cambio,argv[4],argv[5],NULL};
        execv("./binarizarImagen",arreglos);
        printf("regrese a el hijo \n");
    }
    else{
        close(tuberiaC[0]);
        write(tuberiaC[1],&bcabecera.tamano,sizeof(uint32_t));
        write(tuberiaC[1],&bcabecera.reservado1,sizeof(uint16_t));
        write(tuberiaC[1],&bcabecera.reservado2,sizeof(uint16_t));
        write(tuberiaC[1],&bcabecera.offsetBit,sizeof(uint32_t));

        write(tuberiaC[1],&binformacion.alto,sizeof(uint32_t));
        write(tuberiaC[1],&binformacion.ancho,sizeof(uint32_t));
        write(tuberiaC[1],&binformacion.coloresImportantes,sizeof(uint32_t));
        write(tuberiaC[1],&binformacion.colorPixel,sizeof(uint16_t));
        write(tuberiaC[1],&binformacion.compresion,sizeof(uint16_t));
        write(tuberiaC[1],&binformacion.direcciones,sizeof(uint32_t));
        write(tuberiaC[1],&binformacion.tamano,sizeof(uint32_t));
        write(tuberiaC[1],&binformacion.tamanoImagen,sizeof(uint32_t));
        write(tuberiaC[1],&binformacion.totalBit,sizeof(uint32_t));
        write(tuberiaC[1],&binformacion.XResolporMetros,sizeof(uint32_t));
        write(tuberiaC[1],&binformacion.YResolporMetros,sizeof(uint32_t));

        for(int i = 0;i < binformacion.tamanoImagen;i++){
            write(tuberiaC[1],&data_grisaseos[i],sizeof(unsigned char));
        }
        free(data_grisaseos);
        waitpid(pid, &status, 0);
    }
  return 0;
}
