#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h> 
#include <stdint.h>
#include <unistd.h>
#include <sys/wait.h>
#include "cabecerasYfunciones.h"


/* Función que permite retornar una imagen leida en bit almacenada en una variable data_imagen. Además, permite rellenar las 
    estructuras de la cabecera y la informacion del bitmap. 
  ENTRADA:
        - Puntero al nombre del archivo a leer
        - Estructura de una cabecera de informacion de un bitmap
        - Estructura de una cabecera de archivo de un bitmap
        
  SALIDA:
        - Un arreglo de char de una imagen leida
*/
unsigned char *leerImagenBMP(char *nombreArchivo, cabeceraInformacion *binformacion, cabeceraArchivo *bcabecera){
    FILE *archivo;
    uint16_t type;
    unsigned char *data_imagen;
    archivo = fopen(nombreArchivo , "r");
    if(!archivo){
        printf("No se pudo leer la imagen \n");
        return NULL;
    }
    else{
        fread(&type, sizeof(uint16_t), 1, archivo);
        if(type != 0x4D42){
            printf("La imagen no es de tipo bmp \n");
            fclose(archivo);
            return NULL;
        }

        fread(bcabecera, sizeof(cabeceraArchivo), 1, archivo);
        fread(binformacion, sizeof(cabeceraInformacion), 1, archivo);
         
        data_imagen = (unsigned char*)malloc(binformacion -> tamanoImagen* sizeof(unsigned char));
        if(!data_imagen){
            fclose(archivo);
            return 0;
        }

        else{
            fseek(archivo, bcabecera -> offsetBit, SEEK_SET);
            fread(data_imagen, binformacion -> tamanoImagen, 1, archivo); 
            fclose(archivo);
            return data_imagen;
        }
    }
}



/* Función principal de leerImagen. Define el pid del hijo a crear y la tubería. Una vez creado el hijo, éste procede a ejecutar 
    conversorGris. Por otro lado, el padre escribe en la tubería los datos correspondientes a las cabeceras binformacion ybcabecera. 
    Además, escribe en la tubería cada caracter de data_imagen.
*/
int main(int argc,char *argv[]) {
    pid_t pid;
    int status;
    int tuberia[2];
    cabeceraInformacion binformacion;
    cabeceraArchivo bcabecera;
    unsigned char *data_imagen;
    char *cambio = (char *)malloc(2 * sizeof(char));
    //Los parametros que son recibidos como char, se transforma en enteros para su posterior utilizacion
    data_imagen = leerImagenBMP(argv[0], &binformacion, &bcabecera);
    pipe(tuberia);
    pid = fork();
        if (pid < 0){
            printf("Error al crear proceso hijo \n");
            return 0;
        }
    if(pid == 0){
        sprintf(cambio,"%d",tuberia[0]);
        char *arreglos[] = {argv[1],argv[2],argv[3],cambio,argv[4],argv[5],NULL};
        execv("./conversorGris",arreglos);
    }
    else{
        close(tuberia[0]);
        write(tuberia[1],&bcabecera.tamano,sizeof(uint32_t));
        write(tuberia[1],&bcabecera.reservado1,sizeof(uint16_t));
        write(tuberia[1],&bcabecera.reservado2,sizeof(uint16_t));
        write(tuberia[1],&bcabecera.offsetBit,sizeof(uint32_t));

        write(tuberia[1],&binformacion.alto,sizeof(uint32_t));
        write(tuberia[1],&binformacion.ancho,sizeof(uint32_t));
        write(tuberia[1],&binformacion.coloresImportantes,sizeof(uint32_t));
        write(tuberia[1],&binformacion.colorPixel,sizeof(uint16_t));
        write(tuberia[1],&binformacion.compresion,sizeof(uint16_t));
        write(tuberia[1],&binformacion.direcciones,sizeof(uint32_t));
        write(tuberia[1],&binformacion.tamano,sizeof(uint32_t));
        write(tuberia[1],&binformacion.tamanoImagen,sizeof(uint32_t));
        write(tuberia[1],&binformacion.totalBit,sizeof(uint32_t));
        write(tuberia[1],&binformacion.XResolporMetros,sizeof(uint32_t));
        write(tuberia[1],&binformacion.YResolporMetros,sizeof(uint32_t));

        for(int i = 0;i < binformacion.tamanoImagen;i++){
            write(tuberia[1],&data_imagen[i],sizeof(unsigned char));
        }
        free(data_imagen);
        waitpid(pid, &status, 0);
    }
  return 0;
}