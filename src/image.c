#include "image.h"
#include <string.h>

Image *load_image(char *filename) {    
    FILE *fp = fopen(filename, "r");
    //check if file is NULL
    if (fp == NULL){
        ERROR("File is empty");
        exit(EXIT_FAILURE);
    }

    //check if valid PPM file
    char buffer[1024];
    char magic_num[3];
    unsigned int width, height; 

    fgets(buffer, sizeof(buffer), fp);
    sscanf(buffer, "%s", magic_num);
    
    if(strcmp(magic_num, "P3") != 0){
        ERROR("Invalid file type");
        fclose(fp);
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
    img->pixels = malloc(width * height * 3 * sizeof(unsigned int)); 

    for (unsigned int i = 0; i < width * height; i++) {
        unsigned int r, g, b;
        fscanf(fp, "%u %u %u", &r, &g, &b);
        img->pixels[i * 3] = (unsigned char)r;      
        img->pixels[i * 3 + 1] = (unsigned char)g;   
        img->pixels[i * 3 + 2] = (unsigned char)b;  
    }

    fclose(fp);
    return img;
}

void delete_image(Image *image) {
    if(image != NULL){
        free((image)->pixels);
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
    unsigned int index = (row * image->width + col) * 3;
    return image->pixels[index];
}

// unsigned int hide_message(char *message, char *input_filename, char *output_filename) {
//     int length = strlen(message);

//     FILE *fp1 = fopen(input_filename, "r");
//     FILE *fp2 = fopen(output_filename, "w");

//     //scan header
//     char format_var[3];
//     int width, height;
//     fscanf(fp1, "%s", format_var);
//     fscanf(fp1, "%u %u", &width, &height);
//     // fscanf(fp1, "%u", (unsigned int)&max_intensity);

//     //copy header to output file
//     fprintf(fp2, "%s", format_var);
//     fprintf(fp2, "%u %u", width, height);


//     //add null terminator 
//     char *full_message = malloc(length + 1); 
//     strcpy(full_message, message);
//     full_message[length] = '\0';
//     fclose(fp1);
//     fclose(fp2);
//     return 0;
// }

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
