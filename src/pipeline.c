#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include "../include/image.h"
#include "../include/utils.h"
#include "../include/pipeline.h"


/**
 * @brief Función que devuelve el valor de la función a 
 *        aplicar en la Mascara de filtro laplaciano.
 *        
 * 
 * @param p Pixel escogido
 * @param img Estructura imagen
 * @param c Configuracion
 * @return uint8_t 
 */
uint8_t laplace(uint8_t * p,int i,int j,Image*img,Config*c){
    // Se inicializa el valor inicial, en esta variable se irá guardando
    // El resultado de la función
    uint8_t pixel = 0;
    uint8_t * pivot; // Pivote para recorrer la imagen

    /**
     * @brief Primero es importante saber como la librería STBI para el manejo
     * de imágenes funciona. Al abrir una imagen, esta queda guardada como un
     * arreglo de dimensión 1 de largo: largo*ancho, donde cada pixel tiene el largo en base
     * a la cantidad de canales (en este caso 1 ya que es escala de grises).
     * Para poder recorrer y simular como si fuera una matriz, basta con ir sumando y restando
     * valores al puntero hasta llegar al pixel deseado.
     * 
     * Se necesita buscar los pixeles adyacentes, es decir:
     *   |---|---|---|
     *   | 0 | 1 | 2 |
     *   |---|---|---|
     *   | 3 | 4 | 5 |
     *   |---|---|---|
     *   | 6 | 7 | 8 |
     *   |---|---|---|
     * 
     * Donde el pixel 4 es el pixel seleccionado (p).
     * Luego se recorre utilizando el pivote para obtener los pixeles e ir multiplicando en
     * base a la matriz laplaciana(c->lap_mak)
     * 
     * Como las dimensiones son conocidas, no es necesario hacer un ciclo,
     * El orden de lo que está abajo es buscar los pixeles: 0-1-2 3-4-5 6-7-8
     * 
     * Para cada pixel encontrado, se pregunta si es nulo(Caso de borde), si no lo es
     * se le multiplica por su valor correspondiente en la matriz laplaciana y luego se
     * suma a la variable pixel. Si la bandera useSamePixel está activada se utilizará (p) en
     * los casos de borde. Y si no está activada esta no se toma en cuenta(o en otro modo "se multiplica por 0")
     * 
     * 
     */



    // PARA PIXELES 0 1 2


    //PIXEL 0: largo+1 pixeles atrás
    
    if(i-1 >= 0 && j-1 >= 0){
        pivot = p-(img->channels*(img->width+1));
        pixel = pixel + (*pivot)*c->lap_mask[0]; 
    }

    //PIXEL 1: largo pixeles atrás
    
    if(j-1 >= 0){
        pivot = p-(img->channels*(img->width));
        pixel = pixel + (*pivot)*c->lap_mask[1];
    }
    
    //PIXEL 2: largo - 1 pixeles atrás
    
    if(i+1 < img->width && j-1 >= 0){
        pivot = p-(img->channels*(img->width-1));
        pixel = pixel + (*pivot)*c->lap_mask[2];
    }


    //PIXEL 3: 1 pixel atras
    
    if(i-1 >= 0 ){
        pivot = p-(img->channels);
        pixel = pixel + (*pivot)*c->lap_mask[3];
    }
   
    //PIXEL 4: El mismo pixel
    
    
    pixel = pixel + (*p)*c->lap_mask[4];
    
    

    //PIXEL 5: 1 pixel adelante
    
    if(i+1 < img->width ){
        pivot = p+(img->channels);
        pixel = pixel + (*pivot)*c->lap_mask[5];
    }
    


    //PIXEL 6: largo-1 pixeles adelante
    
    if(i-1 >= 0  && j+1 < img->height){
        pivot = p+(img->channels*(img->width-1));
        pixel = pixel + (*pivot)*c->lap_mask[6];
    }
    

    //PIXEL 7: largo pixeles adelante
    
    if(j+1 < img->height){
        pivot = p+(img->channels*(img->width));
        pixel = pixel + (*pivot)*c->lap_mask[7];
    }
    
    //PIXEL 8: largo+1 pixeles adelante
    
    if(i+1 < img->width && j+1 < img->height){
        pivot = p+(img->channels*(img->width+1));
        pixel = pixel + (*pivot)*c->lap_mask[8];
    }



    return pixel;

}

/**
 * @brief Función que transforma la imagen a escala de grises
 * 
 * 
 * @param c Configuracion
 * @param img Estructura imagen
 */
Image rgb_to_grayscale(Config * c,Image img){
    // Inicializa las variables
    int img_size = img.width*img.height*img.channels; //Tamaño de la imagen
    int gray_channels = img.channels == 4 ? 2 : 1; // Nuevos canales
    int gray_img_size = img.width*img.height*gray_channels; //Tamaño de la imagen usando los nuevos canales
    uint8_t *gray_img = malloc(sizeof(uint8_t)*gray_img_size); //Asignar la memoria para la nueva imagen
    //Recorriendo la imagen
    //Idea extraida del video: https://solarianprogrammer.com/2019/06/10/c-programming-reading-writing-images-stb_image-libraries/
    for(uint8_t *p = img.data, *pg = gray_img; p != img.data + img_size; p+= img.channels, pg+= gray_channels){
        *pg = (uint8_t)((*p)*0.3 + (*p + 1)*0.59 + (*p+2)*0.11);
    }
    strcpy(img.data,gray_img);
    return img;
}


/**
 * @brief Función que aplica la máscara de filtro laplaciano a la imagen
 * 
 * @param c Estructura configuracion
 * @param img Estructura imagen
 */
Image apply_lap_filter(Config * c,Image img){
    int img_size = img.width*img.height*img.channels; //Tamaño de la imagen
    uint8_t *lap_img = malloc(sizeof(uint8_t)*img_size); //Asignar la memoria para la nueva imagen
    //Recorriendo la imagen
    uint8_t *p = img.data;
    uint8_t *pg = lap_img;
    for (int j = 0; j < img.height; j++)
    {
        for (int i = 0; i < img.width; i++)
        {
            *pg = laplace(p,i,j,&img,c);
            p+= img.channels;
            pg+= img.channels;
        }
        
    }
    strcpy(img.data,lap_img);
    return img;
}

/**
 * @brief Función que Binariza la imagen en base a un umbral
 * 
 * @param c Estructura configuración
 * @param img Estructura imagen
 */
Image apply_binary(Config * c,Image  img){
    
    int img_size = img.width*img.height*img.channels; //Tamaño de la imagen
    uint8_t *binary_img = malloc(sizeof(uint8_t)*img_size); //Asignar la memoria para la nueva imagen
    //Recorriendo la imagen
    //Idea extraida del video: https://solarianprogrammer.com/2019/06/10/c-programming-reading-writing-images-stb_image-libraries/
    for(uint8_t *p = img.data, *pg = binary_img; p != img.data + img_size; p+= img.channels, pg+= img.channels){
        //Si el valor del pixel es mayor al umbral asignarlo a 255 sino asignarlo a 0
        if(*p > c->bin_threshold){
            *pg = (uint8_t)255; 
        }
        else{
            *pg = (uint8_t)0;
        }
    }
    strcpy(img.data,binary_img);
    return img;
}

/**
 * @brief Función que devuelve si una imagen es lo suficentemente
 * negra en base a un umbral
 * 
 * @param c Estructura de configuracion
 * @param img Estructura Imagen
 */
Image rate(Config * c,Image img){
    int img_size = img.width*img.height*img.channels; //Tamaño de la imagen
    int black_count = 0; // Contador
    for(uint8_t *p = img.data; p != img.data + img_size; p+= img.channels){
        //Si es negro suma al contador
        if(*p == 0){
            black_count+=1; 
        }
    }
    // Si el porcentaje de pixeles negros es mayor o igual al umbral devuelve 1
    if((black_count*100.0/img.width*img.height) >= c->rating_threshold){
        c->rate = 1;
    }
    return img;
}

int main(int argc, char const *argv[])
{
    char buff[1000000] = {'\0'};
    Config*c = load_config(argc,argv);
    read(STDIN_FILENO,buff,1000000);
    Image i = deserialize_image(buff); // Deserializando la imagen 
    printf("Imagen recibida de proceso padre: %s,%d\n",i.filename,i.stage);
    int fd[2];
    if(pipe(fd) == 0){
        // Se supone que aquí la función realizará lo que corresponda a la imagen según la etapa    
        Image out;
        if(i.stage == 0){
            out = open_image(i);
        }
        else if(i.stage == 1){
            out = rgb_to_grayscale(c,i);
        }
        else if(i.stage == 2){
            out = apply_lap_filter(c,i);
        }
        else if(i.stage == 3){
            out = apply_binary(c,i);
        }
        else if(i.stage == 4){
            out = rate(c,i);
        }
        else{
            write_image(i);
            return 0;   
        }




        out.stage++;
        int size = 8*sizeof(int) + 30*sizeof(char) + out.width*out.height+out.channels;
        
        
        pid_t pid = fork();
        if(pid < 0){
            printf("ERROR");
            exit(1);
        }
        else if(pid == 0){
            
            close(fd[1]);
            dup2(fd[0],STDIN_FILENO);
            argv[0] = "pipe";
            execv("pipe",argv); // Se vuelve a llamar al mismo archivo
            
        }
        else{
            close(fd[0]);
            char * img_serialized = serialize_image(out); //Serializando la imagen
            write(fd[1],img_serialized,size);  // ENVIANDO IMAGEN SERIALIZADA
            printf("PROCESO PADRE");
        }
    }
    return 0;
    
}

