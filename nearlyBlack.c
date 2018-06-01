#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include <unistd.h>
#include "cabecerasYfunciones.h"


/*Función que permite retornar una imagen leida en bit almacenada en una variable data_imagen
  Ademas permite rellenar las estructuras de la cabecera y la informacion del bitmap 
  ENTRADA:
        - Puntero al nombre del archivo a leer
        - Estructura de una cabecera de informacion de un bitmap
        
  SALIDA:
        - Un arreglo de char de una imagen leida
*/
unsigned char *leerImagenBMP(char *nombreArchivo, cabeceraInformacion *binformacion, cabeceraArchivo *bcabecera){
    FILE *archivo;
    uint16_t type;
    unsigned char *data_procesada;
    archivo = fopen(nombreArchivo , "r");
    if(!archivo){
        return NULL;
    }
    else{
        fread(&type, sizeof(uint16_t), 1, archivo);
        if(type != 0x4D42){
            printf("La imagen no es de tipo bmp");
            fclose(archivo);
            return NULL;
        }

        fread(bcabecera, sizeof(cabeceraArchivo), 1, archivo);
        fread(binformacion, sizeof(cabeceraInformacion), 1, archivo);
         
        data_procesada = (unsigned char*)malloc(binformacion -> tamanoImagen* sizeof(unsigned char));
        if(!data_procesada){
            fclose(archivo);
            return 0;
        }

        else{
            fseek(archivo, bcabecera -> offsetBit, SEEK_SET);
            fread(data_procesada, binformacion -> tamanoImagen, 1, archivo);  
            fclose(archivo);
            return data_procesada;
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
unsigned char *transformarAGrises(cabeceraInformacion *binformacion, unsigned char *data_imagen){
    int filas, colorGrisaseo;
    int cantidadBits = binformacion -> totalBit/8;
    unsigned char azul, verde, rojo, extra, *grisaseos;
    grisaseos = (unsigned char *)malloc(binformacion -> tamanoImagen * sizeof(unsigned char));

    if(cantidadBits == 3){
        for(filas = 0; filas < binformacion -> tamanoImagen; filas = filas + 3){
            colorGrisaseo = data_imagen[filas+2] * 0.3 + data_imagen[filas+1] * 0.59 + data_imagen[filas] * 0.11;
            grisaseos[filas+2] = colorGrisaseo;
            grisaseos[filas+1] = colorGrisaseo;
            grisaseos[filas] = colorGrisaseo;
        }
    }

    if(cantidadBits == 4){
            for(filas = 0; filas <binformacion -> tamanoImagen; filas = filas + 4){
            colorGrisaseo = data_imagen[filas+2] * 0.3 + data_imagen[filas+1] * 0.59 + data_imagen[filas] * 0.11;
            grisaseos[filas+3] = 255;
            grisaseos[filas+2] = colorGrisaseo;
            grisaseos[filas+1] = colorGrisaseo;
            grisaseos[filas] = colorGrisaseo;
        }
    }

    return grisaseos;
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
unsigned char *binarizarImagen(cabeceraInformacion *binformacion, unsigned char *data_grisaseo, int UMBRAL, bitmaptotal *totalPixel){
    int columnas;
    int contadorTotal = 0;
    int contadorNegros = 0;
    int contadorBlancos = 0;
    int filas = 0;
    unsigned char data, *binariosColor;
    binariosColor = (unsigned char *)malloc(binformacion -> tamanoImagen * sizeof(unsigned char));

    if(binformacion -> totalBit != 32){
        for(filas = 0; filas < binformacion -> tamanoImagen; filas = filas + 3){
            if(data_grisaseo[filas] > UMBRAL){
                binariosColor[filas+2] = 255; 
                binariosColor[filas +1] = 255;
                binariosColor[filas] = 255;
                contadorNegros++;
            }

            else{
                binariosColor[filas+ 2] = 0; 
                binariosColor[filas + 1] = 0;
                binariosColor[filas] = 0;
                contadorBlancos++;
            }
        }
    }

    if(binformacion -> totalBit == 32){
        for(filas = 0; filas < binformacion -> tamanoImagen; filas = filas + 4){
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
    }

    totalPixel -> totalBlancos = contadorBlancos;
    totalPixel -> totalNegros = contadorNegros; 
    return binariosColor;
}




/* Procedimiento que verifica y muestra por pantalla si la imagen corresponde o no a nearly black.
   ENTRADA:
        - Total de pixeles de la imagen.
        - Un entero que corresponde al umbral definido.
*/
void verificarNearlyBlack(bitmaptotal *totalPixeles, int UMBRAL, int numeroImagen){
    float negros = totalPixeles -> totalNegros;
    float todos = totalPixeles -> totalBlancos + totalPixeles -> totalNegros;
    int porcentaje = (negros/todos)*100;
    totalPixeles -> porcentaje = porcentaje;

    if(porcentaje > UMBRAL){
        printf("---------------------------------------------------------------\n");
        printf("  Imagen %d: Nearly Black Positivo, con %d %% de pixeles negros \n", numeroImagen, porcentaje);
        printf("---------------------------------------------------------------\n");
        printf("\n"); 
    }
    else{
        printf("----------------------------------------------------------------\n");
        printf("  Imagen %d: Nearly Black Negativo, con %d %% de pixeles negros  \n", numeroImagen, porcentaje);
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
void crearImagen(cabeceraInformacion *binformacion, cabeceraArchivo *bcarchivo_guardado, char *Nombre_archivo_salida, unsigned char *data_imagen){
    FILE *archivo;  
    uint16_t type;

    archivo = fopen(Nombre_archivo_salida, "wb" );
    if(!archivo){ 
        printf( "La imagen no se pudo crear\n");
        exit(1);
    }
    binformacion -> compresion = 0;
    type=0x4D42;
    fwrite(&type,sizeof(uint16_t),1,archivo);
    fwrite(&bcarchivo_guardado -> tamano, 4, 1, archivo);
    fwrite(&bcarchivo_guardado -> reservado1, 2, 1, archivo);
    fwrite(&bcarchivo_guardado -> reservado2, 2, 1, archivo);
    fwrite(&bcarchivo_guardado -> offsetBit, 4, 1, archivo);
    fwrite(&binformacion -> tamano, 4, 1, archivo);
    fwrite(&binformacion -> alto, 4, 1, archivo);
    fwrite(&binformacion -> ancho, 4, 1, archivo);
    fwrite(&binformacion -> direcciones, 2, 1, archivo);
    fwrite(&binformacion -> totalBit, 2, 1, archivo);
    fwrite(&binformacion -> compresion, 4, 1, archivo);
    fwrite(&binformacion -> tamanoImagen, 4, 1, archivo);
    fwrite(&binformacion -> XResolporMetros, 4, 1, archivo);
    fwrite(&binformacion -> YResolporMetros,4, 1, archivo);
    fwrite(&binformacion -> colorPixel, 4, 1, archivo);
    fwrite(&binformacion -> coloresImportantes, 4, 1, archivo);
    fseek(archivo,bcarchivo_guardado -> offsetBit, SEEK_SET);
    fwrite(data_imagen, binformacion -> tamanoImagen, 1, archivo);
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
*/
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
}


