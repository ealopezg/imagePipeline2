#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "../include/image.h"


#define STB_IMAGE_IMPLEMENTATION
#include "../lib/stb/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../lib/stb/stb_image_write.h"

/**
 * @brief Función que abre una imagen
 * 
 * @param filename Nombre del archivo
 * @return Image* Estructura de la imagen
 */
Image open_image(Image img){
    Image out;

    out.data = stbi_load(img.filename,&out.width,&out.height,&out.channels,0);
    if(out.data == NULL){
        free(out.data);
    }
    out.filename = malloc(sizeof(char)*30);
    out.stage = 0;
    strcpy(out.filename,img.filename);
    return out;
}

Image empty_image(char*filename){
    Image img;
    img.data = NULL;
    img.stage = 0;
    img.width = 0;
    img.height = 0;
    img.channels = 0;
    img.filename = malloc(sizeof(char)*30);
    strcpy(img.filename,filename);
    return img;
}

/**
 * @brief Función que libera de memoria la imagen
 * 
 * @param img Estructura Imagen
 */
void free_image(Image *img){
    if(img->data == NULL){
        stbi_image_free(img->data);
    } 
    free(img);
}

/**
 * @brief Función que guarda una estructura Imagen en un archivo
 *        con formato jpg
 * 
 * @param filename Nombre del archivo a guardar
 * @param img Estructura Imagen
 */
void write_image(Image img){
    stbi_write_jpg(img.filename, img.width, img.height, img.channels, img.data, 100);
}

char* serialize_image(Image i){
    int size = 8*sizeof(int) + 30*sizeof(char) + i.width*i.height+i.channels;
    char *buf = malloc(sizeof(char) * (size+1));
    memcpy(buf, &i.width, sizeof(int) );

    memcpy(buf + sizeof(int),&i.height,sizeof(int));

    memcpy(buf + sizeof(int)*2,&i.channels,sizeof(int));

    memcpy(buf + sizeof(int)*3,&i.stage,sizeof(int));

    memcpy(buf + sizeof(int)*4,i.filename,sizeof(char)*30);

    memcpy(buf + sizeof(int)*4 + sizeof(char)*30,i.data,i.width*i.height+i.channels);
    buf[size] = '\0';
    return buf;
}
Image deserialize_image(char* buf){
    Image i;

    i.filename = malloc(sizeof(char)*(31));
    memcpy(&i.width,buf,sizeof(int));

    memcpy(&i.height,buf + sizeof(int),sizeof(int));

    memcpy(&i.channels,buf + sizeof(int)*2,sizeof(int));
    memcpy(&i.stage,buf + sizeof(int)*3,sizeof(int));
    memcpy(i.filename,buf + sizeof(int)*4, sizeof(char)*30);
    i.filename[31] = '\0';
    int size = i.width*i.height+i.channels;
    i.data = malloc(sizeof(uint8_t) * (size+1));
    memcpy(i.data,buf + sizeof(int)*4 + sizeof(char)*30,size);
    i.data[size] = '\0';
    return i;
}
