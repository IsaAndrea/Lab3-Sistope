#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h> 
#include <stdint.h>
#include <unistd.h>
#include <pthread.h>
#include "cabecerasYfunciones.h"

/*
int main(int argc,char **argv){
    int c, cantidadImagenes, largo, UMBRAL, UMBRAL_clasificacion, cantidadHebras;
    int bflag = 0;
    char *archivoEntrada, *archivoBinario; 
    opterr = 0;
    while((c = getopt(argc,argv,"c:h:u:n:b")) != -1)
        switch(c){
            case 'c':
                sscanf(optarg, "%d", &cantidadImagenes);
                largo = strlen(optarg);
                archivoEntrada = malloc(largo + 11);
                archivoBinario = malloc(largo + 27);
            case 'h':
                sscanf(optarg, "%d", &cantidadHebras);
                break;
            case 'u':
                sscanf(optarg, "%d", &UMBRAL);
                break;
            case 'n':
                sscanf(optarg, "%d", &UMBRAL_clasificacion);
                break;
            case 'b':
                bflag = 1;
                break;
            case '?':
                if(optopt == 'c')
                    fprintf(stderr,"Opcion -%c necesita un argumento \n",optopt);
                else if(isprint(optopt))
                    fprintf(stderr,"Opcion desconocida -%c .\n",optopt);
                else
                    fprintf(stderr,"Opcion con caracter desconocido '\\x%x' . \n",optopt);
                return 1;
            default:
                abort();
        }
    if((cantidadImagenes <= 0) || (UMBRAL <= 0) || (UMBRAL_clasificacion <= 0)){
        printf("Ingrese los parametros correctamente, procure que sean los indicados y vuelva a intentarlo.\n");
        return -1;
    }
    else if((cantidadImagenes > 0) && ((UMBRAL < 0) || (UMBRAL_clasificacion < 0))){
        printf("Ingrese los parametros correctamente, procure que sean los indicados y vuelva a intentarlo. \n");
        return -1;
    }
    else if(UMBRAL_clasificacion > 101){
        printf("Ingrese los parametros correctamente, procure que sean los indicados y vuelva a intentarlo. \n");
        return -1;
    }
    while((cantidadImagenes > 0) && (UMBRAL > 0) && (UMBRAL_clasificacion > 0)){
        procesarImagenes(cantidadImagenes, UMBRAL, UMBRAL_clasificacion, bflag, archivoEntrada, archivoBinario);
        cantidadImagenes--;
    }
    
}*/

/* Función principal. Se definen los archivos de entrada y salida a trabajar, junto con las variables solicitadas al usuario, siendo
    éstas la cantida de imágenes a analizar, el umbral y umbral de clasificación. En caso de no entregar los datos correctos, se le 
    dará aviso al usuario para que modifique con brevedad. 
    Una vez obtenidos los datos se procede a analizar cada imagen, creando un hijo por cada una para que éste proceda a ejecutar el
    proceso leerImagen, mientras el padre lo espera. 
*/
int main(int argc,char **argv){
    int c, cantidadImagenes,largo, UMBRAL, UMBRAL_clasificacion, filas, i, j, verificador, cantidadHebras, h;
    int bflag = 0;
    char *archivoEntrada, *archivoBinario; 
    //cabeceraInformacion binformacion;
    //cabeceraArchivo bcabecera;
    //bitmaptotal totalPixel;
    //parametrosHebra parametros;
    parametrosHebra parametros;

    opterr = 0;
    while((c = getopt(argc,argv,"c:h:u:n:b")) != -1)
        switch(c){
            case 'c':
                sscanf(optarg,"%d",&cantidadImagenes);
                largo = strlen(optarg);
                archivoEntrada = malloc(largo + 11);
                archivoBinario = malloc(largo + 27);
                break;
            case 'h':
                sscanf(optarg, "%d", &cantidadHebras);
                break;
            case 'u':
                sscanf(optarg,"%d",&UMBRAL);
                break;
            case 'n':
                sscanf(optarg,"%d",&UMBRAL_clasificacion);
                break;
            case 'b':
                bflag = 1;
                break;
            case '?':
                if(optopt == 'c')
                    fprintf(stderr,"Opcion -%c necesita un argumento \n",optopt);
                else if(isprint(optopt))
                    fprintf(stderr,"Opcion desconocida -%c .\n",optopt);
                else
                    fprintf(stderr,"Opcion con caracter desconocido '\\x%x' . \n",optopt);
                return 1;
            default:
                abort();
        }
    if(cantidadImagenes <= 0 || UMBRAL <= 0 || UMBRAL_clasificacion <= 0  ){
        printf("Ingrese los parametros correctamente, procure que sean los indicados y vuelva a intentarlo.\n");
        return -1;
    }
    else if(cantidadImagenes > 0 && (UMBRAL < 0 || UMBRAL_clasificacion < 0)){
        printf("Ingrese los parametros correctamente, procure que sean los indicados y vuelva a intentarlo.\n");
        return -1;
    }
    else if(UMBRAL_clasificacion > 101){
        printf("Ingrese los parametros correctamente, procure que sean los indicados y vuelva a intentarlo.\n");
        return -1;
    }

    while(cantidadImagenes > 0 && UMBRAL > 0 && UMBRAL_clasificacion > 0 ){
        sprintf(archivoEntrada,"imagen_%d.bmp",cantidadImagenes);
        sprintf(archivoBinario,"binario_%d.bmp",cantidadImagenes);
        parametros.archivoEntrada = archivoEntrada;
        parametros.archivoBinario = archivoBinario;
        parametros.UMBRAL = UMBRAL;
        parametros.UMBRAL_clasificacion = UMBRAL_clasificacion;
        parametros.nImagen = cantidadImagenes;


        //data_imagen = leerImagenBMP(archivoEntrada, &binformacion, &bcabecera);
        leerImagenBMP(&parametros);

        /*
        printf("leerImagen %d \n\n", binformacion.ancho);
        printf("leerImagen %d \n\n", bcabecera.tamano);*/
        
        printf("parametros %d \n\n", parametros.cabInfo.ancho);
        printf("parametros %d \n\n", parametros.cabArch.tamano);



        if(parametros.data_imagen != NULL){
            if(parametros.cabInfo.totalBit == 32){
                //grisaseos = transformarAGrises(&binformacion, data_imagen);
                transformarAGrises(&parametros);
                //binariosColor = binarizarImagen(&binformacion, grisaseos, UMBRAL, &totalPixel);
                binarizarImagen(&parametros);
            //    crearImagen(&binformacion, &bcabecera, archivoBinario, binariosColor);
                crearImagen(&parametros);
                if(bflag == 1){
              //    verificarNearlyBlack(&totalPixel, UMBRAL_clasificacion, cantidadImagenes);
                    verificarNearlyBlack(&parametros);
                }
            }
            free(parametros.binariosColor);
            free(parametros.data_imagen);
            free(parametros.grisaseos);
            cantidadImagenes -= 1;
            parametros.nImagen -= 1;
        }    
    }
    
}

