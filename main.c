#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h> 
#include <stdint.h>
#include <unistd.h>
#include <sys/wait.h>
#include "cabecerasYfunciones.h"


/* Función principal. Se definen los archivos de entrada y salida a trabajar, junto con las variables solicitadas al usuario, siendo
    éstas la cantida de imágenes a analizar, el umbral y umbral de clasificación. En caso de no entregar los datos correctos, se le 
    dará aviso al usuario para que modifique con brevedad. 
    Una vez obtenidos los datos se procede a analizar cada imagen, creando un hijo por cada una para que éste proceda a ejecutar el
    proceso leerImagen, mientras el padre lo espera. 
*/
int main(int argc,char **argv){
    int c, cantidadImagenes,largo, UMBRAL, UMBRAL_clasificacion;
    int bflag = 0;
    //int tuberia[2];
    int status;
    pid_t pid;
    char *archivoEntrada, *archivoBinario; 
    char *data1 = (char*)malloc(16 * sizeof(char));
    char *data2 = (char*)malloc(16 * sizeof(char));
    char *data3 = (char*)malloc(16 * sizeof(char));
    char *data4 = (char*)malloc(16 * sizeof(char));
    opterr = 0;
    while((c = getopt(argc,argv,"c:u:n:b")) != -1)
        switch(c){
            case 'c':
                sscanf(optarg,"%d",&cantidadImagenes);
                largo = strlen(optarg);
                archivoEntrada = malloc(largo + 11);
                archivoBinario = malloc(largo + 27);
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
        printf("ingrese los parametros correctamente sino son superiores a 0 o no son los los indicados no se puede ejecutar el codigo vuelva a ejecutarlo \n");
        return -1;
    }
    else if(cantidadImagenes > 0 && (UMBRAL < 0 || UMBRAL_clasificacion < 0)){
        printf("ingrese los parametros correctamente sino son superiores a 0 o no son los los indicados no se puede ejecutar el codigo vuelva a ejecutarlo \n");
        return -1;
    }
    else if(UMBRAL_clasificacion > 101){
        printf("ingrese los parametros correctamente sino son superiores a 0 o no son los los indicados no se puede ejecutar el codigo vuelva a ejecutarlo \n");
        return -1;
    }
    while(cantidadImagenes > 0 && UMBRAL > 0 && UMBRAL_clasificacion > 0 ){
        //pipe(tuberia);
        pid = fork();
        sprintf(archivoEntrada,"imagen_%d.bmp",cantidadImagenes);
        sprintf(archivoBinario,"archivo_salida_binario_%d.bmp",cantidadImagenes);
        if (pid < 0){
            printf("Error al crear proceso hijo \n");
            return 0;
        }
        if(pid == 0){
            sprintf(data1, "%d", UMBRAL);
            sprintf(data2, "%d", UMBRAL_clasificacion);
            sprintf(data3, "%d", bflag);
            sprintf(data4, "%d", cantidadImagenes);
            char *arreglos[] = {archivoEntrada,data1,data2,data3,data4,archivoBinario,NULL};
            execv("./lectorImagen",arreglos);
            }
        else{
            waitpid(pid, &status, 0);
        }
        cantidadImagenes--;
    }
    
}

