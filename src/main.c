#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include "../include/utils.h"
#include "../include/image.h"

#define DEBUG 1 //DEJAR EN 1 PARA QUE NO SE SOBREESCRIBA LA IMAGEN


int main(int argc, char const *argv[])
{
    Config * c = load_config(argc,argv); //Carga de configuracion
    if(c != NULL){
        if(read_lap_mask(c)){
            char filename[30];
            if(c->show) {
                printf("|       image      |     nearly black    |\n|------------------|---------------------|\n");
            }
            for (int i = 0; i < c->images; i++)
            {
                
                int fd[2];
                if(pipe(fd)==0){
                    sprintf(filename,"imagen_%d.jpg",i+1); //Genera el nombre de la imagen
                    Image img = empty_image(filename);
                    int size = 8*sizeof(int) + 30*sizeof(char) + img.width*img.height+img.channels;
                    char * img_serialized = serialize_image(img);
                    write(fd[1],img_serialized,size);
                    close(fd[1]);
                    pid_t pid = fork();
                    if(pid < 0){
                        exit(1);
                    }
                    else if(pid == 0){
                        close(fd[1]);
                        dup2(fd[0],STDIN_FILENO);
                        argv[0] = "pipe";
                        execv("pipe",argv);
                    }
                }          
                
            }
        }
        else{
            printf("El archivo %s no se puede abrir o no existe!\n",c->lap_mask_file_name);
        }
    }
    else{
        printf("Los argumentos son inválidos o no están todos los necesarios!\n");
    }
    
    return 0;
}
