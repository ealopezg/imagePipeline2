#ifndef IMAGE_H
#define IMAGE_H

//Estructura imagen
typedef struct Image
{
    int width; //Ancho
    int height; //Alto
    int channels; //Cantidad de canales
    int stage;
    char *filename;
    uint8_t * data; //Arreglo de pixeles
} Image;

Image open_image(Image img);
Image empty_image(char*filename);
void write_image(Image i);
void free_image(Image *img);
void print_image(Image *img);
char* serialize_image(Image i);
Image deserialize_image(char* buf);

#endif 
