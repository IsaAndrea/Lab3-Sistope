// Estructura que almacena la cabecera del archivo
typedef struct BITMAPCABECERAARCHIVO{
    uint32_t         tamano;          // Tamaño del archivo 
    uint16_t         reservado1;      // Reservado, indicado por definicion 
    uint16_t         reservado2;      // Reservado, indicado por definicion 
    uint32_t         offsetBit;       // Offset de la data del bitmap 
}cabeceraArchivo;


// Estructura que almacena la información de cabecera de la imagen
typedef struct BITMAPINFORMACIONCABECERA{
    uint32_t         tamano;                // Tamaño de la información de cabecera 
    uint32_t         ancho;                 // Ancho de la imagen 
    uint32_t         alto;                  // Alto de la imagen 
    uint16_t         direcciones;           // Total de direcciones dentro de la imagen 
    uint16_t         totalBit;              // Número de bits por pixel (r,b,g, v)
    uint32_t         compresion;            // Variable que representa si la imagen está comprimida 
    uint32_t         tamanoImagen;          // Tamaño de la data de la imagen 
    uint32_t         XResolporMetros;       // Resolucion X de bit por metros 
    uint32_t         YResolporMetros;       // Resolucion Y de bit pos metros 
    uint32_t         colorPixel;            // Número que indica si el pixel es NEGRO o BLANCO 
    uint32_t         coloresImportantes;    //  
}cabeceraInformacion;

/* Estructura que almacena la totalidad de los pixeles dentro de una imagen */
typedef struct BITMAPTOTAL{
    int         totalNegros;               // Número que indica el total de pixel negro en una imagen
    int         totalBlancos;              // Número que indica el total de pixel blanco en una imagen
    int         porcentaje;                // indicara el porcentaje en funcion a si es o no nearly black
}bitmaptotal;


unsigned char *leerImagenBMP(char *nombreArchivo, cabeceraInformacion *binformacion, cabeceraArchivo *bcabecera);
void mostrarInformacionCabecera(cabeceraInformacion *info);
void mostrarCabeceraArchivo(cabeceraArchivo *info);
unsigned char *transformarAGrises(cabeceraInformacion *binformacion, unsigned char *data_imagen);
unsigned char *binarizarImagen(cabeceraInformacion *binformacion, unsigned char *data_grisaseo, int UMBRAL, bitmaptotal *total_pixel);
void verificarNearlyBlack(bitmaptotal *totalPixeles, int UMBRAL, int numeroImagen);
void escribirNearlyBlack(int nearlyBlack, int numeroImagen,bitmaptotal *total);
void crearImagen(cabeceraInformacion *binformacion, cabeceraArchivo *bcarchivo_guardado, char *Nombre_archivo_salida, unsigned char *data_imagen);
void procesarImagenes(int cantidadImagenes, int UMBRAL, int UMBRAL_clasificacion, int bflag, char *archivoEntrada, char *archivoBinario, char *archivoGrisaseo);
