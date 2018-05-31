#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h> 
#include <stdint.h>
#include <sys/wait.h>
#include <sys/wait.h>
#include <unistd.h>
#include "cabecerasYfunciones.h"

/* Procedimiento que crea una imagen de tipo bmp, especificamente una imagen binarizada. 
   ENTRADA:
        - Estructura de una cabecera de informacion de un bitmap.
        - Estructura de una cabecera de archivo de un bitmap.
        - Un nombre que corresponde al archivo de salida.
        - Un arreglo de pixeles binarizados.
*/
void crearImagen(cabeceraInformacion *binformacion, cabeceraArchivo *bcarchivo_guardado, char *archivoSalida, unsigned char *data_imagen){
    FILE *archivo;  
    uint16_t type;

    archivo = fopen(archivoSalida, "wb" );
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



/* Función principal de creadorImagen. Realiza la lectura de la tubería de analizadorPropiedad, obteniendo los datos correspondientes 
    de las cabeceras binformacion y bcabecera, así como de binarios_color. De esta forma, es posible asignar los parámetros necesarios 
    a crearImagen.
*/
int main(int argc,char *argv[]) {
    cabeceraInformacion binformacion;
    cabeceraArchivo bcabecera;
    int tuberia = atoi(argv[0]);
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

    unsigned char *binarizar_color = (unsigned char*)malloc(binformacion.tamanoImagen * sizeof(unsigned char));
    for(int i = 0;i < binformacion.tamanoImagen;i++){
        read(tuberia,&binarizar_color[i],sizeof(unsigned char));
    }
    //Los parametros que son recibidos como char, se transforma en enteros para su posterior utilizacion
    crearImagen(&binformacion,&bcabecera,argv[1],binarizar_color); 
    return 0;
}
