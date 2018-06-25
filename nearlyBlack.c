#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include <unistd.h>
#include <pthread.h>
#include "cabecerasYfunciones.h"


/*  Procedimiento que lee la información del bitmap de una imagen BMP recibida y la guarda en las estructuras respectivas. Además,
    guarda la imagen leida en bit en una variable 'data_imagen' (matriz de pixeles).
    ENTRADA: Estructura  
*/
void leerImagenBMP(parametrosHebra *parametros){
    FILE *archivo;
    uint16_t type;
    char *nombreArchivo = parametros -> archivoEntrada;
    archivo = fopen(nombreArchivo , "r");
    if(!archivo){
        printf(" No se pudo leer el archivo \n");
    }
    else{
        fread(&type, sizeof(uint16_t), 1, archivo);
        if(type != 0x4D42){
            printf("La imagen no es de tipo bmp");
            fclose(archivo);
        }

        fread(&parametros -> cabArch, sizeof(BITMAPCABECERAARCHIVO), 1, archivo);
        fread(&parametros -> cabInfo, sizeof(BITMAPINFORMACIONCABECERA), 1, archivo);
         
        parametros -> data_imagen = (unsigned char*)malloc(parametros -> cabInfo.tamanoImagen* sizeof(unsigned char));
        if(!parametros -> data_imagen){
            printf(" No se pudo guardar la imagen, intente nuevamente \n");
            fclose(archivo);
        }

        else{
            fseek(archivo, parametros -> cabArch.offsetBit, SEEK_SET);
            fread(parametros -> data_imagen, parametros -> cabInfo.tamanoImagen, 1, archivo);  
            fclose(archivo);
        }
    }
    
}



/*  Procedimiento que transforma a gris la imagen leida y almadenada en 'data_imagen', aplicando una fórmula definida a cada pixel. 
    El resultado obtenido en cada operación es almacedano en la variable 'grisaseos' (matriz de pixeles grisaseos).
    ENTRADA:
*/

void transformarAGrises(parametrosHebra *parametros){
    pthread_mutex_lock(&parametros -> candado);
    int filas, colorGrisaseo;
    int cantidadBits = parametros -> cabInfo.totalBit/8;

    if (parametros -> hebras < parametros -> totalHebras){
        for(filas = parametros -> posicionHebra; filas < (1 + parametros -> posicionHebra); filas = filas + 4){
            colorGrisaseo = (parametros -> data_imagen[filas+2]) * 0.3 + (parametros -> data_imagen[filas+1]) * 0.59 + (parametros -> data_imagen[filas]) * 0.11;
            parametros -> grisaseos[filas+3] = 255;
            parametros -> grisaseos[filas+2] = colorGrisaseo;
            parametros -> grisaseos[filas+1] = colorGrisaseo;
            parametros -> grisaseos[filas] = colorGrisaseo;
        }
        parametros -> posicionHebra = filas;
        parametros -> hebras++;

    }

    else{
        for(filas = parametros -> posicionHebra; filas < (parametros -> cabInfo.tamanoImagen - (parametros -> hebras -1)); filas = filas + 4){
            colorGrisaseo = (parametros -> data_imagen[filas+2]) * 0.3 + (parametros -> data_imagen[filas+1]) * 0.59 + (parametros -> data_imagen[filas]) * 0.11;
            parametros -> grisaseos[filas+3] = 255;
            parametros -> grisaseos[filas+2] = colorGrisaseo;
            parametros -> grisaseos[filas+1] = colorGrisaseo;
            parametros -> grisaseos[filas] = colorGrisaseo;
        }
    }

    pthread_mutex_unlock(&parametros -> candado);
}



/*  Procedimiento que binariza la imagen grisasea almadenada en 'grisaseos', verificando si los pixeles sobrepasan o no el umbral
    binario definido, y según dicha comparación, se procede a modificar el valor del pixel.  El resultado obtenido en cada comparación
    es almacedano en la variable 'binariosColor' (matriz de pixeles binarizados). 
    ENTRADA:
*/

void binarizarImagen(parametrosHebra *parametros){
    pthread_mutex_lock(&parametros -> candado);
    int filas;

    if (parametros -> hebras < parametros -> totalHebras){
        for(filas = parametros -> posicionHebra; filas < (1 + parametros -> posicionHebra); filas = filas + 4){
            if((parametros -> grisaseos[filas]) > (parametros -> UMBRAL)){
                parametros -> binariosColor[filas+3] = 255; 
                parametros -> binariosColor[filas + 2] = 255;
                parametros -> binariosColor[filas+ 1] = 255;
                parametros -> binariosColor[filas] = 255;
            }

            else{
                parametros -> binariosColor[filas+3] = 255; 
                parametros -> binariosColor[filas + 2] = 0;
                parametros -> binariosColor[filas+ 1] = 0;
                parametros -> binariosColor[filas] = 0;
            }
        }
        parametros -> posicionHebra = filas;
        parametros -> hebras++;
    }

    else{
        for(filas = parametros -> posicionHebra; filas < (parametros -> cabInfo.tamanoImagen - (parametros -> hebras -1)); filas = filas + 4){
            if((parametros -> grisaseos[filas]) > (parametros -> UMBRAL)){
                parametros -> binariosColor[filas+3] = 255; 
                parametros -> binariosColor[filas + 2] = 255;
                parametros -> binariosColor[filas+ 1] = 255;
                parametros -> binariosColor[filas] = 255;
            }

            else{
                parametros -> binariosColor[filas+3] = 255; 
                parametros -> binariosColor[filas + 2] = 0;
                parametros -> binariosColor[filas+ 1] = 0;
                parametros -> binariosColor[filas] = 0;
            }

        }
    }

    pthread_mutex_unlock(&parametros -> candado);
}




/* Procedimiento que verifica si la imagen corresponde o no a nearly black, realizando un conteo de pixeles negros y blancos.
   ENTRADA:
*/
void verificarNearlyBlack(parametrosHebra *parametros){
    pthread_mutex_lock(&parametros -> candado);
    int filas;

    if (parametros -> hebras < parametros -> totalHebras){
        for(filas = parametros -> posicionHebra; filas < (1 + parametros -> posicionHebra); filas = filas + 4){
            if((parametros -> grisaseos[filas]) > (parametros -> UMBRAL)){
                parametros -> bitMT.totalBlancos++;
            }

            else{
                parametros -> bitMT.totalNegros++;
            }
        }
        parametros -> posicionHebra = filas;
        parametros -> hebras++;
    }

    else{
        for(filas = parametros -> posicionHebra; filas < (parametros -> cabInfo.tamanoImagen - (parametros -> hebras -1)); filas = filas + 4){
            if((parametros -> grisaseos[filas]) > (parametros -> UMBRAL)){
                parametros -> bitMT.totalBlancos++;
            }

            else{
                parametros -> bitMT.totalNegros++;
            }

        }
    }

    pthread_mutex_unlock(&parametros -> candado);
}


/*  Procedimiento que muestra por pantalla si la imagen corresponde o no a nearly black, comparando si el porcentaje de pixeles negros
    sobrepasan o no el umbral de clasificación definido.
   ENTRADA:

*/
void escribirNearlyBlack(parametrosHebra *parametros){
    float negros = parametros -> bitMT.totalNegros;
    float todos = (parametros -> bitMT.totalBlancos) + (parametros -> bitMT.totalNegros);
    int porcentaje = (negros/todos)*100;
    parametros -> bitMT.porcentaje = porcentaje;

    if(porcentaje > (parametros -> UMBRAL)){
        printf("---------------------------------------------------------------\n");
        printf("  Imagen %d: Nearly Black Positivo, con %d %% de pixeles negros \n", parametros -> nImagen, porcentaje);
        printf("---------------------------------------------------------------\n");
        printf("\n"); 
    }
    else{
        printf("----------------------------------------------------------------\n");
        printf("  Imagen %d: Nearly Black Negativo, con %d %% de pixeles negros  \n", parametros -> nImagen, porcentaje);
        printf("----------------------------------------------------------------\n");
        printf("\n");
    }
}



/* Procedimiento que crea una imagen de tipo BMP a partir de la información de una imagen binarizada almacenada en 'binariosColor'. 
   ENTRADA:
*/
void crearImagen(parametrosHebra *parametros){
    FILE *archivo;  
    uint16_t type;
    char *nombreArchivo = parametros -> archivoBinario;
    archivo = fopen(nombreArchivo, "wb" );

    if(!archivo){ 
        printf( "La imagen no se pudo crear\n");
        exit(1);
    }

    parametros -> cabInfo.compresion = 0;
    type=0x4D42;

    fwrite(&type,sizeof(uint16_t),1,archivo);
    fwrite(&parametros -> cabArch.tamano, 4, 1, archivo);
    fwrite(&parametros -> cabArch.reservado1, 2, 1, archivo);
    fwrite(&parametros -> cabArch.reservado2, 2, 1, archivo);
    fwrite(&parametros -> cabArch.offsetBit, 4, 1, archivo);
    fwrite(&parametros -> cabInfo.tamano, 4, 1, archivo);
    fwrite(&parametros -> cabInfo.alto, 4, 1, archivo);
    fwrite(&parametros -> cabInfo.ancho, 4, 1, archivo);
    fwrite(&parametros -> cabInfo.direcciones, 2, 1, archivo);
    fwrite(&parametros -> cabInfo.totalBit, 2, 1, archivo);
    fwrite(&parametros -> cabInfo.compresion, 4, 1, archivo);
    fwrite(&parametros -> cabInfo.tamanoImagen, 4, 1, archivo);
    fwrite(&parametros -> cabInfo.XResolporMetros, 4, 1, archivo);
    fwrite(&parametros -> cabInfo.YResolporMetros,4, 1, archivo);
    fwrite(&parametros -> cabInfo.colorPixel, 4, 1, archivo);
    fwrite(&parametros -> cabInfo.coloresImportantes, 4, 1, archivo);
    fseek(archivo,parametros -> cabArch.offsetBit, SEEK_SET);
    fwrite(parametros -> binariosColor, parametros -> cabInfo.tamanoImagen, 1, archivo);
    fclose(archivo);
}
