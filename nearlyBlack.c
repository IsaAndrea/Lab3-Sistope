#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include <unistd.h>
#include <pthread.h>
#include "cabecerasYfunciones.h"




/*Función que permite retornar una imagen leida en bit almacenada en una variable data_imagen
  Ademas permite rellenar las estructuras de la cabecera y la informacion del bitmap 
  ENTRADA:
        - Puntero al nombre del archivo a leer
        - Estructura de una cabecera de informacion de un bitmap
        
  SALIDA:
        - Un arreglo de char de una imagen leida
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

        fread(&parametros -> cabArch, sizeof(cabeceraArchivo), 1, archivo);
        fread(&parametros -> cabInfo, sizeof(cabeceraInformacion), 1, archivo);
         
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



/* Función que aplica una fórmula a los pixeles definidos dentro de una estructura que contiene todos los 
   pixeles de una imagen procesada.
   ENTRADA:
        - Estructura de una cabecera de informacion de un bitmap.
        - Un arreglo de pixeles que representa a la imagen.
   SALIDA:
        - Una matriz de pixeles checkeados.
*/
void transformarAGrises(parametrosHebra *parametros){
    int filas, colorGrisaseo;
    int cantidadBits = parametros -> cabInfo.totalBit/8;
    parametros -> grisaseos = (unsigned char *)malloc(parametros -> cabInfo.tamanoImagen * sizeof(unsigned char));

    if(cantidadBits == 4){
        for(filas = 0; filas < (parametros -> cabInfo.tamanoImagen); filas = filas + 4){
            colorGrisaseo = (parametros -> data_imagen[filas+2]) * 0.3 + (parametros -> data_imagen[filas+1]) * 0.59 + (parametros -> data_imagen[filas]) * 0.11;
            parametros -> grisaseos[filas+3] = 255;
            parametros -> grisaseos[filas+2] = colorGrisaseo;
            parametros -> grisaseos[filas+1] = colorGrisaseo;
            parametros -> grisaseos[filas] = colorGrisaseo;
        }
    }
}



/* Función que binariza una imágen grisasea.
   ENTRADA:
        - Estructura de una cabecera de informacion de un bitmap.
        - Un arreglo de pixeles grisaseos.
        - Un entero que representa al umbral.
        - Total de pixeles de la imagen.
   SALIDA:
        - Una matriz de pixeles binarizados.
*/
void binarizarImagen(parametrosHebra *parametros){
    int columnas;
    int contadorTotal = 0;
    int contadorNegros = 0;
    int contadorBlancos = 0;
    int filas = 0;
    parametros -> binariosColor = (unsigned char *)malloc(parametros -> cabInfo.tamanoImagen * sizeof(unsigned char));

    if(parametros -> cabInfo.totalBit == 32){
        for(filas = 0; filas < (parametros -> cabInfo.tamanoImagen); filas = filas + 4){
            if((parametros -> grisaseos[filas]) > (parametros -> UMBRAL)){
                parametros -> binariosColor[filas+3] = 255; 
                parametros -> binariosColor[filas + 2] = 255;
                parametros -> binariosColor[filas+ 1] = 255;
                parametros -> binariosColor[filas] = 255;
                contadorBlancos++;
            }

            else{
                parametros -> binariosColor[filas+3] = 255; 
                parametros -> binariosColor[filas + 2] = 0;
                parametros -> binariosColor[filas+ 1] = 0;
                parametros -> binariosColor[filas] = 0;
                contadorNegros++;
            }
        }
    }

    parametros -> bitMT.totalBlancos = contadorBlancos;
    parametros -> bitMT.totalNegros = contadorNegros; 
}




/* Procedimiento que verifica y muestra por pantalla si la imagen corresponde o no a nearly black.
   ENTRADA:
        - Total de pixeles de la imagen.
        - Un entero que corresponde al umbral definido.
*/
void verificarNearlyBlack(parametrosHebra *parametros){
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



/* Procedimiento que crea una imagen de tipo bmp.
   ENTRADA:
        - Estructura de una cabecera de informacion de un bitmap.
        - Estructura de una cabecera de archivo de un bitmap.
        - Un nombre que corresponde al archivo de salida.
        - Un arreglo de pixeles.
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





/* Procedimiento que lee una imagen bmp, la convierte a grisaseo y posteriormente la binariza.  
   ENTRADA:
        - Un entero que corresponde al total de las imagenes a procesar. 
        - Un entero que corresponde a un umbral.
        - Un entero que corresponde al umbral de clasificación.
        - Una bandera que indica si se muestra o no por pantalla la información.
        - Nombre de la imagen a procesar. 
        - Nombre del archivo grisaseo.
        - Nombre del archivo binario.

void procesarImagenes(int cantidadImagenes, int UMBRAL, int UMBRAL_clasificacion, int bflag, char *archivoEntrada, char *archivoBinario){
    int filas, i, j, verificador;
    unsigned char *data_imagen, *grisaseos, *binariosColor;
    cabeceraInformacion binformacion;
    cabeceraArchivo bcabecera;
    bitmaptotal totalPixel;
    sprintf(archivoEntrada,"imagen_%d.bmp",cantidadImagenes);
    sprintf(archivoBinario,"archivo_binario_%d.bmp",cantidadImagenes);
    data_imagen = leerImagenBMP(archivoEntrada, &binformacion, &bcabecera);
    if(data_imagen != NULL){
        if(binformacion.totalBit == 32){
        grisaseos = transformarAGrises(&binformacion, data_imagen);
        binariosColor = binarizarImagen(&binformacion, grisaseos, UMBRAL, &totalPixel);
        crearImagen(&binformacion, &bcabecera, archivoBinario, binariosColor);
        if(bflag == 1){
            verificarNearlyBlack(&totalPixel, UMBRAL_clasificacion, cantidadImagenes);
            }
        }
        else{
            grisaseos = transformarAGrises(&binformacion, data_imagen);
            binariosColor = binarizarImagen(&binformacion, data_imagen, UMBRAL, &totalPixel);
            crearImagen(&binformacion, &bcabecera, archivoBinario, binariosColor);
            if(bflag == 1){
                verificarNearlyBlack(&totalPixel, UMBRAL_clasificacion, cantidadImagenes);
            }
        }
        free(binariosColor);
        free(data_imagen);
        free(grisaseos);
        cantidadImagenes -= 1;
    }    
}*/


