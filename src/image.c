#include "image.h"
#include <string.h>

Image *load_image(char *filename) {    
    FILE *fp = fopen(filename, "r");
    //check if file is NULL
    if (fp == NULL){
        ERROR("File is NULL");
        exit(EXIT_FAILURE);
    }

    //check if valid PPM file
    char buffer[100];
    char format_variant[2];
    unsigned int width, height; 

    fgets(buffer, sizeof(buffer), fp);
    sscanf(buffer, "%s", format_variant);
    
    if(strcmp(format_variant, "P3") != 0){
        ERROR("Invalid file type");
        exit(EXIT_FAILURE); 
    }   
    fgets(buffer, sizeof(buffer), fp);
    while(buffer[0] == '#'){
        fgets(buffer, sizeof(buffer), fp);
    }

    sscanf(buffer, "%u %u", &width, &height); 
    fgets(buffer, sizeof(buffer),fp);

    Image *img = malloc(sizeof(Image));

    img->width = width;
    img->height = height; 
    img->pixels = malloc(width * height * sizeof(unsigned int)); 

    for(unsigned int i = 0; i < width * height; i++){
        unsigned int r, g, b; 
        fscanf(fp, "%u %u %u", &r, &g, &b);
        img->pixels[i * 3] = r;
        img->pixels[i * 3 + 1] = g;
        img->pixels[i * 3 + 2] = b;
    }

    fclose(fp);
    return img;
}

void delete_image(Image *image) {
    if(image != NULL){
        free(image->pixels);
        image->pixels = NULL;
        free(image);
        image = NULL;
    }
}

unsigned short get_image_width(Image *image) {
    return image->width;
}

unsigned short get_image_height(Image *image) {
    return image->height;
}

unsigned char get_image_intensity(Image *image, unsigned int row, unsigned int col) {
    printf("%u\n", image->pixels[row*col]);
    return image->pixels[row*col];
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
