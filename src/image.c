#include "image.h"
#include <string.h>

Image *load_image(char *filename) {    
    FILE *fp = fopen(filename, "r");
    //check if file is NULL
    if (fp == NULL){
        ERROR("File is NULL");
        exit(EXIT_FAILURE);
    }

    Image *img = malloc(sizeof(Image));

    //check if valid PPM file
    char buffer[100];
    char format_variant[100];
    unsigned int width, height = 0; 

    if(fgets(buffer, sizeof(buffer), fp) != NULL){
        sscanf(buffer, "%s", format_variant);
    }
    
    while(fgets(buffer, sizeof(buffer), fp) != NULL){
        if(buffer[0] == '#'){
            continue;
        }
        if(sscanf(buffer, "%u %u", &width, &height)){
            break; 
        }
    }

    Image *img = malloc(sizeof(Image));
    img->width = width;
    img->height = height; 
    // unsigned int width, height = 0;
    // char line[MAX_SIZE]; 
    // //skip comment line and read width and height 
    // // while(fgets(line,sizeof(line), fp)){
    // //     if(strcmp(line[0], "#") == 0){
    // //         continue;
    // //     }
    // //     sscanf(line, "%u %u", &width, &height);
    // //     break;
    // // }
    // unsigned int max_value = 0;
    // fscanf(fp, "%u", &max_value);

    // //add data into struct 
    // img->width = width;
    // img->height = height;
    // img->pixels = malloc(width * height * 3);

    // for(unsigned int i = 0; i < width * height; i++){
    //     unsigned int r, g, b;
    //     fscanf(fp, "%u %u %u", &r, &g, &b);
    //     img->pixels[i * 3] = (unsigned char)r;  
    //     img->pixels[i * 3 + 1] = (unsigned char)g;
    //     img->pixels[i * 3 + 2] = (unsigned char)b;
    // }

    fclose(fp);
    return img;
}

void delete_image(Image *image) {
    (void)image;
    // if(image != NULL){
    //     free(image->pixels);
    //     free(image);
    // }
}

unsigned short get_image_width(Image *image) {
    (void)image;
    return 0;
}

unsigned short get_image_height(Image *image) {
    (void)image;
    return 0;
}

unsigned char get_image_intensity(Image *image, unsigned int row, unsigned int col) {
    (void)image;
    (void)row;
    (void)col;
    return 0;
}

unsigned int hide_message(char *message, char *input_filename, char *output_filename) {
    (void)message;
    (void)input_filename;
    (void)output_filename;
    return 0;
}

char *reveal_message(char *input_filename) {
    (void)input_filename;
    return NULL;
}

unsigned int hide_image(char *secret_image_filename, char *input_filename, char *output_filename) {
    (void)secret_image_filename;
    (void)input_filename;
    (void)output_filename;
    return 10;
}

void reveal_image(char *input_filename, char *output_filename) {
    (void)input_filename;
    (void)output_filename;
}
