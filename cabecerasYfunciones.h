// Estructura que almacena la cabecera del archivo
typedef struct BITMAPCABECERAARCHIVO{
    uint32_t         tamano;          // Tamaño del archivo 
    uint16_t         reservado1;      // Reservado, indicado por definicion 
    uint16_t         reservado2;      // Reservado, indicado por definicion 
    uint32_t         offsetBit;       // Offset de la data del bitmap 
}BITMAPCABECERAARCHIVO;


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
}BITMAPINFORMACIONCABECERA;

/* Estructura que almacena la totalidad de los pixeles dentro de una imagen */
typedef struct BITMAPTOTAL{
    int         totalNegros;               // Número que indica el total de pixel negro en una imagen
    int         totalBlancos;              // Número que indica el total de pixel blanco en una imagen
    int         porcentaje;                // indicara el porcentaje en funcion a si es o no nearly black
}BITMAPTOTAL;


// Estructura que almacena estructuras de imagen
typedef struct INFOIMAGEN{
    struct      	BITMAPCABECERAARCHIVO      	cabArch;
    struct 			BITMAPINFORMACIONCABECERA  	cabInfo;
    struct      	BITMAPTOTAL  			   	bitMT;  
    int 			UMBRAL;
    int 			UMBRAL_clasificacion;
    unsigned char 	*data_imagen; 
    unsigned char 	*grisaseos; 
    unsigned char 	*binariosColor;
    char 			*archivoEntrada;
    char			*archivoBinario; 
    int 			nImagen;
    int 			bflag;
    int 			totalHebras;
    int 			posicionHebra;
    pthread_mutex_t candado;
    int             hebras;
}parametrosHebra;           



void leerImagenBMP(parametrosHebra *parametros);
void transformarAGrises(parametrosHebra *parametros);
void binarizarImagen(parametrosHebra *parametros);
void verificarNearlyBlack(parametrosHebra *parametros);
void escribirNearlyBlack(parametrosHebra *parametros);
void crearImagen(parametrosHebra *parametros);
