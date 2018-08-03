#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h> 
#include <stdint.h>
#include <unistd.h>
#include <pthread.h>
#include "cabecerasYfunciones.h"


int main(int argc,char **argv){
    int c, cantidadImagenes,largo, UMBRAL, UMBRAL_clasificacion, filas, i, j, cantidadHebras;
    int bflag = 0;
    int h = 0;
    char *archivoEntrada, *archivoBinario; 
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
        parametros.nImagen = cantidadImagenes;
        parametros.UMBRAL = UMBRAL;
        parametros.UMBRAL_clasificacion = UMBRAL_clasificacion;
        parametros.bflag = bflag;
        parametros.totalHebras = cantidadHebras;
        parametros.hebras = 1;
        parametros.posicionHebra = 0;
        parametros.bitMT.totalBlancos = 0;
        parametros.bitMT.totalNegros = 0;;

        if (cantidadHebras == 0){
            pthread_mutex_init(&parametros.candado,NULL);
            leerImagenBMP(&parametros);
      

            if(parametros.data_imagen != NULL){
                if(parametros.cabInfo.totalBit == 32){
                    parametros.grisaseos = (unsigned char *)malloc(parametros.cabInfo.tamanoImagen * sizeof(unsigned char));
                    transformarAGrises(&parametros);
                    parametros.binariosColor = (unsigned char *)malloc(parametros.cabInfo.tamanoImagen * sizeof(unsigned char));
                    binarizarImagen(&parametros);
                    crearImagen(&parametros);
                    if(bflag == 1){
                        verificarNearlyBlack(&parametros);
                        escribirNearlyBlack(&parametros);
                    } 
                }
            }  
        }
        

        if (cantidadHebras > 0){  

            pthread_mutex_init(&parametros.candado,NULL);   
            pthread_t  tidHebra[cantidadHebras];
            pthread_t  tidHebraa[cantidadHebras];
            pthread_create(&tidHebra[h], NULL, (void*)leerImagenBMP, (void*)&parametros);
            pthread_join(tidHebra[h],NULL); 

            if(parametros.data_imagen != NULL){
                if(parametros.cabInfo.totalBit == 32){

                    //Transformar a grisaseos
                    parametros.grisaseos = (unsigned char *)malloc(parametros.cabInfo.tamanoImagen * sizeof(unsigned char));
                    for (h = 0; h < cantidadHebras; h++){
                        pthread_create(&tidHebra[h], NULL, (void*)transformarAGrises, (void*)&parametros);
                    }

                    for (h = 0; h < cantidadHebras; h++){
                        pthread_join(tidHebra[h], NULL);
                    }
                    parametros.hebras = 1;
                    parametros.posicionHebra = 0;

                   //Transformar a binarios
                    parametros.binariosColor = (unsigned char *)malloc(parametros.cabInfo.tamanoImagen * sizeof(unsigned char));
                    for (h = 0; h < cantidadHebras; h++){
                        pthread_create(&tidHebra[h], NULL, (void*)binarizarImagen, (void*)&parametros);
                    }

                    for (h = 0; h < cantidadHebras; h++){
                        pthread_join(tidHebra[h], NULL);
                    }
                    parametros.hebras = 1;
                    parametros.posicionHebra = 0;


                   //Verificar Nearly Black
                    for (h = 0; h < cantidadHebras; h++){
                        pthread_create(&tidHebra[h], NULL, (void*)verificarNearlyBlack, (void*)&parametros);
                    }

                    for (h = 0; h < cantidadHebras; h++){
                        pthread_join(tidHebra[h], NULL);
                    }
                    parametros.hebras = 1;
                    parametros.posicionHebra = 0;

                    crearImagen(&parametros);
                    if(bflag == 1){
                        verificarNearlyBlack(&parametros);
                        escribirNearlyBlack(&parametros);
                    }
                }

            }  
            h = 0;
        } 
        free(parametros.binariosColor);
        free(parametros.data_imagen);
        free(parametros.grisaseos);
        cantidadImagenes -= 1;
    }
}

