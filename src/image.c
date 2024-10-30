#include "image.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

Image *load_image(char *filename) {    
    FILE *fp = fopen(filename, "r");
    //check if file is NULL
    if (fp == NULL){
        ERROR("File is empty");
        exit(EXIT_FAILURE);
    }

    //check if valid PPM file
    char buffer[1024];
    char format_variant[3];
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

unsigned int hide_message(char *message, char *input_filename, char *output_filename) {
    FILE *read = fopen(input_filename, "r");
    FILE *write = fopen(output_filename, "w");

    if(read == NULL || write == NULL){
        ERROR("error opening files");
        return 0;
    }

    char buffer[256];
    unsigned int width, height;
    char format_variant[3];
    fgets(buffer, sizeof(buffer), read);
    sscanf(buffer, "%s", format_variant);
    if(strcmp(format_variant, "P3") != 0){
        ERROR("Invalid file type");
        fclose(read);
        fclose(write);
        exit(EXIT_FAILURE); 
    }   
    fgets(buffer, sizeof(buffer), read);
    while(buffer[0] == '#'){
        fgets(buffer, sizeof(buffer), read);
    }
    sscanf(buffer, "%u %u", &width, &height); 
    fgets(buffer, sizeof(buffer),read);

    int pixel_count = width * height;
    int max_chars = ceil(pixel_count / 8);
    int msg_len = strlen(message);
    int pixels[pixel_count];

    for(int i = 0; i < pixel_count; i++){
        int r, g, b;
        fscanf(read, "%d %d %d", &r, &g, &b);
        pixels[i] = r;
    }

    char msg_cpy[msg_len + 1];
    strcpy(msg_cpy, message);
    int chars_to_encode = (msg_len >= max_chars) ? max_chars - 1 : msg_len;

    for (int i = 0; i < chars_to_encode && i < msg_len; i++) {
    char curr_char = msg_cpy[i];
    printf("Encoding character: %c (ASCII %d)\n", curr_char, curr_char);
        for (int bit = 0; bit < 8; bit++) {
            int bit_value = (curr_char >> (7 - bit)) & 1;
            printf("Bit %d of character '%c': %d\n", bit, curr_char, bit_value);
            pixels[i * 8 + bit] = (pixels[i * 8 + bit] & ~1) | bit_value;
        }
    }
    char null_char = '\0';
    for (int bit = 0; bit < 8; bit++) {
        int pixel_index = (chars_to_encode * 8 + bit);
        int bit_value = (null_char >> (7 - bit)) & 1;
        pixels[pixel_index] = (pixels[pixel_index] & ~1) | bit_value;
    }

    //writing to output file
    fprintf(write, "P3\n%u %u\n255\n", width, height);
    for(int i = 0; i < pixel_count; i++){
        fprintf(write, "%u %u %u\n", pixels[i], pixels[i],pixels[i]);
    }

    fclose(read);
    fclose(write);
    return chars_to_encode;
}


char *reveal_message(char *input_filename) {
    FILE *fp = fopen(input_filename, "r");
    if(fp == NULL){
        fclose(fp); 
        ERROR("error opening file");
        exit(EXIT_FAILURE);
    }
    char buffer[256];
    unsigned int width, height;
    char format_variant[3];
    fgets(buffer, sizeof(buffer), fp);
    sscanf(buffer, "%s", format_variant);
    if(strcmp(format_variant, "P3") != 0){
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

    int pixel_count = width * height;
    int pixels[pixel_count];

    for(int i = 0; i < pixel_count; i++){
        int r, g, b;
        fscanf(fp, "%d %d %d", &r, &g, &b);
        pixels[i] = r;
    }
    
    int num_chars = (pixel_count / 8) - 1; //number of characters encoded without null terminator
    char *message = (char *)malloc((num_chars + 1) * sizeof(char));

    int bits[8];
    int bit_index = 0; 
    int msg_index = 0;
    for(int i = 0; i < pixel_count; i++){
        bits[bit_index] = pixels[i] & 1;
        bit_index++; 
        if(bit_index == 8){
            char ascii_char = (bits[0] << 7) | (bits[1] << 6) | (bits[2] << 5) | (bits[3] << 4) | (bits[4] << 3) | (bits[5] << 2) |(bits[6] << 1) | bits[7];
            message[msg_index] = ascii_char;
            msg_index++;
            bit_index = 0;
        }
    }
    message[msg_index] = '\0';
    return message;
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