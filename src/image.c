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
        for (int bit = 0; bit < 8; bit++) {
            int bit_value = (curr_char >> (7 - bit)) & 1;
            pixels[i * 8 + bit] = (pixels[i * 8 + bit] & ~1) | bit_value;
        }
    }
    //encode null terminator
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
    fgets(buffer, sizeof(buffer), fp);
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
    
    int num_chars = (pixel_count / 8); 
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
    fclose(fp);
    return message;
}

unsigned int hide_image(char *secret_image_filename, char *input_filename, char *output_filename) {
    FILE *secret = fopen(secret_image_filename, "r");
    FILE *input = fopen(input_filename, "r");
    FILE *write = fopen(output_filename, "w");
    if(secret == NULL || input == NULL || write == NULL){
        fclose(secret);
        fclose(input); 
        fclose(write);
        ERROR("error opening file");
        exit(EXIT_FAILURE);
    }

    char buffer[256];
    unsigned int secret_width, secret_height;
    fgets(buffer, sizeof(buffer), secret);
    fgets(buffer, sizeof(buffer), secret);
    while(buffer[0] == '#'){
        fgets(buffer, sizeof(buffer), secret);
    }
    sscanf(buffer, "%u %u", &secret_width, &secret_height); 
    fgets(buffer, sizeof(buffer),secret);

    int num_secret_pixels = secret_width * secret_height;
    int secret_pixels[num_secret_pixels];
    for(int i = 0; i < num_secret_pixels; i++){
        int r, g, b;
        fscanf(secret, "%d %d %d", &r, &g, &b);
        secret_pixels[i] = r;
    }
    
    unsigned int input_width, input_height;
    char buffer2[256];
    fgets(buffer2, sizeof(buffer), input);
    fgets(buffer2, sizeof(buffer), input);
    while(buffer2[0] == '#'){
        fgets(buffer2, sizeof(buffer), input);
    }
    sscanf(buffer2, "%u %u", &input_width, &input_height); 
    fgets(buffer2, sizeof(buffer),input);
    

    int num_input_pixels = input_width * input_height; 
    int input_pixels[num_input_pixels];
    for(int i = 0; i < num_input_pixels; i++){
        int r, g, b;
        fscanf(input, "%d %d %d", &r, &g, &b);
        input_pixels[i] = r;
    }
    int dimensions[2] = {secret_width, secret_height}; // Array to hold both width and height

    if(num_input_pixels < (16 + 8 * num_secret_pixels)){
        fclose(secret);
        fclose(input);
        fclose(write);
        return 0;
    }else {
        
           // Loop over both dimensions (0 for width, 1 for height)
        for (int i = 0; i < 2; i++) {
            int dimension_value = dimensions[i]; 
            for (int bit = 0; bit < 8; bit++) {
                int bit_value = (dimension_value >> (7 - bit)) & 1;
                input_pixels[(i * 8) + bit] = (input_pixels[(i * 8) + bit] & ~1) | bit_value; 
            }
        }
        int index = 16; //starting after encoding of width and height
        for(int i = 0; i < num_secret_pixels; i++){
            int curr_pixel = secret_pixels[i];
            for(int bit = 0; bit < 8; bit++){
                int bit_value = (curr_pixel >> (7 - bit)) & 1;
                input_pixels[index] = (input_pixels[index] & ~1) | bit_value; 
                index++;
            }

        }
    }

    fprintf(write, "P3\n%u %u\n255\n", input_width, input_height);
    for(int i = 0; i < num_input_pixels; i++){
        fprintf(write, "%u %u %u\n", input_pixels[i], input_pixels[i], input_pixels[i]);
    }
    fclose(secret);
    fclose(input);
    fclose(write);
    return 1;
}

void reveal_image(char *input_filename, char *output_filename) {
    FILE *read = fopen(input_filename, "r");
    FILE *write = fopen(output_filename, "w");

    if(read == NULL || write == NULL){
        fclose(read);
        fclose(write);
        ERROR("Error opening files");
        exit(EXIT_FAILURE);
    }
    char buffer[256];
    unsigned int width, height;
    fgets(buffer, sizeof(buffer), read);
    fgets(buffer, sizeof(buffer), read);
    while(buffer[0] == '#'){
        fgets(buffer, sizeof(buffer), read);
    }
    sscanf(buffer, "%u %u", &width, &height); 
    fgets(buffer, sizeof(buffer),read);

    int pixel_count = width * height;
    int pixels[pixel_count];
    
    for(int i = 0; i < pixel_count; i++){
        int r, g, b;
        fscanf(read, "%d %d %d", &r, &g, &b);
        pixels[i] = r;
    }

    int output_width, output_height; 
    int bits[8];
    int bit_index = 0;
    for(int i = 0; i < 8; i++){
        bits[bit_index] = pixels[i] & 1;
        bit_index++;
    }
    output_width = (bits[0] << 7) | (bits[1] << 6) | (bits[2] << 5) | (bits[3] << 4) | (bits[4] << 3) | (bits[5] << 2) |(bits[6] << 1) | bits[7];
    bit_index = 0;
    for(int i = 8; i < 16; i++){
        bits[bit_index] = pixels[i] & 1;
        bit_index++;
    }
    output_height = (bits[0] << 7) | (bits[1] << 6) | (bits[2] << 5) | (bits[3] << 4) | (bits[4] << 3) | (bits[5] << 2) |(bits[6] << 1) | bits[7];

    int num_hidden_pixels = output_width * output_height;
    int output_pixels[num_hidden_pixels];
    printf("Width %d Height %d\n",output_width, output_height);

    int index = 16;
    for(int i = 0; i < num_hidden_pixels; i++){
        for (int bit = 0; bit < 8; bit++) {
            bits[bit] = (pixels[index] & 1);
            index++;
        }
        output_pixels[i] = (bits[0] << 7) | (bits[1] << 6) | (bits[2] << 5) | (bits[3] << 4) | (bits[4] << 3) | (bits[5] << 2) |(bits[6] << 1) | bits[7];
    }
    fprintf(write, "P3\n%u %u\n%u\n", output_width, output_height, 255);
    for(int i = 0; i < num_hidden_pixels; i++){
        fprintf(write, "%d %d %d\n", output_pixels[i], output_pixels[i], output_pixels[i]);
    }
    fclose(read);
    fclose(write);
}