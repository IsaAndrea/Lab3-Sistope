#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h> 
#include <stdint.h>
#include <unistd.h>
#include <sys/wait.h>
#include "cabecerasYfunciones.h"

/* Función que binariza una imágen grisasea.
   ENTRADA:
        - Estructura de una cabecera de informacion de un bitmap.
        - Un arreglo de pixeles grisaseos.
        - Un entero que representa al umbral.
        - Total de pixeles de la imagen.
   SALIDA:
        - Una matriz de pixeles binarizados.
*/
unsigned char *binarizarImagen(cabeceraInformacion *binformacion, unsigned char *data_grisaseo, int UMBRAL, bitmaptotal *total_pixel){
    int contadorNegros = 0;
    int contadorBlancos = 0;
    int filas = 0;
    unsigned char  *binariosColor;
    binariosColor = (unsigned char *)malloc(binformacion->tamanoImagen * sizeof(unsigned char));
    for(filas = 0; filas < binformacion->tamanoImagen; filas = filas + 4){
        if(data_grisaseo[filas] > UMBRAL){
            binariosColor[filas+3] = 255; 
            binariosColor[filas + 2] = 255;
            binariosColor[filas+ 1] = 255;
            binariosColor[filas] = 255;
            contadorBlancos++;
        }
        else{
            binariosColor[filas+3] = 255; 
            binariosColor[filas + 2] = 0;
            binariosColor[filas+ 1] = 0;
            binariosColor[filas] = 0;
            contadorNegros++;
        }
    }

    total_pixel->totalBlancos = contadorBlancos;
    total_pixel->totalNegros = contadorNegros; 
    return binariosColor;
}



/* Función principal de binarizadorImagen. Define el pid del hijo a crear y una tubería, para luego realizar la lectura de la tubería
    de conversorGris, obteniendo los datos correspondientes de las cabeceras binformacion y bcabecera, así como de data_grisaseos.
    De esta forma, es posible asignar los parámetros necesarios a binarizarImagen.
    Una vez creado el hijo, éste procede a ejecutar analizadorPropiedad. Por otro lado, el padre escribe en la tubería los datos 
    correspondientes a las cabeceras binformacion y bcabecera. Además, escribe en la tubería cada caracter de binarios_color.
*/
int main(int argc,char *argv[]) {
    pid_t pid;
    int status;
    int tuberiaB[2];
    unsigned char *binarizar_color;
    cabeceraInformacion binformacion;
    cabeceraArchivo bcabecera;
    bitmaptotal totalPixeles;
    int tuberia = atoi(argv[3]);
    int UMBRAL;
    char *cambio = (char *)malloc(2 * sizeof(char));

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

    unsigned char *data_grisaseos = (unsigned char*)malloc(binformacion.tamanoImagen * sizeof(unsigned char));
    for(int i = 0;i < binformacion.tamanoImagen;i++){
        read(tuberia,&data_grisaseos[i],sizeof(unsigned char));
    }
    UMBRAL = atoi(argv[0]);
    //Los parametros que son recibidos como char, se transforma en enteros para su posterior utilizacion
    binarizar_color = binarizarImagen(&binformacion,data_grisaseos,UMBRAL,&totalPixeles);
    pipe(tuberiaB);
    pid = fork();
    if (pid < 0){
        printf("Error al crear proceso hijo \n");
        return 0;
    }
    if(pid == 0){
        sprintf(cambio,"%d",tuberiaB[0]);
        char *arreglos[] = {argv[1],argv[2],cambio,argv[4],argv[5],NULL};
        execv("./analisisPropiedad",arreglos);
    }
    else{
        close(tuberiaB[0]);
        write(tuberiaB[1],&bcabecera.tamano,sizeof(uint32_t));
        write(tuberiaB[1],&bcabecera.reservado1,sizeof(uint16_t));
        write(tuberiaB[1],&bcabecera.reservado2,sizeof(uint16_t));
        write(tuberiaB[1],&bcabecera.offsetBit,sizeof(uint32_t));

        write(tuberiaB[1],&binformacion.alto,sizeof(uint32_t));
        write(tuberiaB[1],&binformacion.ancho,sizeof(uint32_t));
        write(tuberiaB[1],&binformacion.coloresImportantes,sizeof(uint32_t));
        write(tuberiaB[1],&binformacion.colorPixel,sizeof(uint16_t));
        write(tuberiaB[1],&binformacion.compresion,sizeof(uint16_t));
        write(tuberiaB[1],&binformacion.direcciones,sizeof(uint32_t));
        write(tuberiaB[1],&binformacion.tamano,sizeof(uint32_t));
        write(tuberiaB[1],&binformacion.tamanoImagen,sizeof(uint32_t));
        write(tuberiaB[1],&binformacion.totalBit,sizeof(uint32_t));
        write(tuberiaB[1],&binformacion.XResolporMetros,sizeof(uint32_t));
        write(tuberiaB[1],&binformacion.YResolporMetros,sizeof(uint32_t));

        write(tuberiaB[1],&totalPixeles.totalBlancos,sizeof(int));
        write(tuberiaB[1],&totalPixeles.totalNegros,sizeof(int));

        for(int i = 0;i < binformacion.tamanoImagen;i++){
            write(tuberiaB[1],&binarizar_color[i],sizeof(unsigned char));
        }
        free(binarizar_color);
        waitpid(pid, &status, 0);
    }
  return 0;
}