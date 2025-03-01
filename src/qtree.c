#include "qtree.h"
#include <math.h> 
#include <stdio.h>

QTNode *create_quadtree(Image *image, double max_rmse) {
    unsigned int width = image->width;
    unsigned int height = image->height;
    return create_quadtree_helper(image, max_rmse, width, height, 0, 0);
}   



QTNode *create_quadtree_helper(Image *image, double max_rmse, unsigned int width, unsigned int height, unsigned int row, unsigned int col) {
    QTNode *node = (QTNode *)malloc(sizeof(QTNode));

    double avg_intensity = calculate_avg_intensity(image, row, col, width, height);
    node->intensity = (unsigned char)avg_intensity;
    node->row = row;
    node->col = col; 
    node->height = height;
    node->width = width;
    
    for (int i = 0; i < 4; i++) {
        node->children[i] = NULL;
    }

    double rmse = calculate_rmse(image, avg_intensity, row, col, width, height);
    if (rmse <= max_rmse) {
        return node;
    }

    // If region is large enough to split in both dimensions
    if (width > 1 && height > 1) {
        unsigned int half_width = width / 2;
        unsigned int half_height = height / 2;

        node->children[0] = create_quadtree_helper(image, max_rmse, half_width, half_height, row, col);
        node->children[1] = create_quadtree_helper(image, max_rmse, width - half_width, half_height, row, col + half_width);
        node->children[2] = create_quadtree_helper(image, max_rmse, half_width, height - half_height, row + half_height, col);
        node->children[3] = create_quadtree_helper(image, max_rmse, width - half_width, height - half_height, row + half_height, col + half_width);
    } 
    // only horizontal split possible
    else if (width > 1 && height == 1) {
        unsigned int half_width = width / 2;
        node->children[0] = create_quadtree_helper(image, max_rmse, half_width, height, row, col);
        node->children[1] = create_quadtree_helper(image, max_rmse, width - half_width, height, row, col + half_width);
    } 
    // only vertical split possible
    else if (height > 1 && width == 1) {
        unsigned int half_height = height / 2;
        node->children[0] = create_quadtree_helper(image, max_rmse, width, half_height, row, col);
        node->children[2] = create_quadtree_helper(image, max_rmse, width, height - half_height, row + half_height, col);
    } 
    // No further splitting 
    else {
        node->children[0] = node->children[1] = node->children[2] = node->children[3] = NULL;
    }
    return node;
}


//helper function to get avg intensity 
double calculate_avg_intensity(Image *image, unsigned int row, unsigned int col, unsigned int width, unsigned int height) {
    double total_intensity = 0.0;
    unsigned int pixel_count = width * height; 

    for (unsigned int r = row; r < row + height; r++) {
        for (unsigned int c = col; c < col + width; c++) {
            total_intensity += get_image_intensity(image, r, c); 
        }
    }
    
    if (pixel_count == 0) {
        return 0.0;
    }

    return total_intensity / (double)pixel_count;
}
//calculate rmse 
double calculate_rmse(Image *image, double avg_intensity, unsigned int row, unsigned int col, unsigned int width, unsigned int height){
    double sum_squared_diff = 0.0;
    unsigned int pixel_count = width * height; 

    for(unsigned int r = row; r < row + height; r++){
        for(unsigned int c = col; c < col + width; c++){
            unsigned int pixel_intensity = get_image_intensity(image, r, c);
            sum_squared_diff += (pixel_intensity - avg_intensity) * (pixel_intensity - avg_intensity);
        }
    }
    return sqrt(sum_squared_diff / (double)pixel_count);
}

QTNode *get_child1(QTNode *node) {
    if(node->children[0] == NULL){
        return NULL;
    }
    return node->children[0];
}

QTNode *get_child2(QTNode *node) {
    if(node->children[1] == NULL){
        return NULL;
    }
    return node->children[1];
}

QTNode *get_child3(QTNode *node) {
    if(node->children[2] == NULL){
        return NULL;
    }
    return node->children[2];
}

QTNode *get_child4(QTNode *node) {
    if(node->children[3] == NULL){
        return NULL;
    }
    return node->children[3];
}

unsigned char get_node_intensity(QTNode *node) {
    return node->intensity;
}

void delete_quadtree(QTNode *root) {
    if(root != NULL){
        for(int i = 0; i < 4; i++){
            delete_quadtree(root->children[i]);
        }
    }
    free(root);
    root = NULL;
}

void save_qtree_as_ppm(QTNode *root, char *filename) {
    FILE *fp = fopen(filename, "w");
    if (fp == NULL){
        ERROR("could not write to file");
        exit(EXIT_FAILURE);
    }

    fprintf(fp, "P3\n");                         
    fprintf(fp, "%u %u\n", root->width, root->height); 
    fprintf(fp, "255\n");    

    unsigned char **image_array = malloc(root->height * sizeof(unsigned char *));
    for (unsigned int i = 0; i < root->height; i++) {
        image_array[i] = malloc(root->width * sizeof(unsigned char));
    }

    save_qtree_as_ppm_helper(root, image_array, 0, 0);

    for (unsigned int i = 0; i < root->height; i++) {
        for (unsigned int j = 0; j < root->width; j++) {
            unsigned char intensity = image_array[i][j];
            fprintf(fp, "%u %u %u\n", intensity, intensity, intensity); 
        }
    }

    for (unsigned int i = 0; i < root->height; i++) {
        free(image_array[i]);
    }
    free(image_array);

    fclose(fp);
}

void save_qtree_as_ppm_helper(QTNode *node, unsigned char **img_arr, unsigned int row_offset, unsigned int col_offset){
    if(node == NULL){
        return;
    }

    if (node->children[0] == NULL && node->children[1] == NULL && node->children[2] == NULL && node->children[3] == NULL) {
    for (unsigned int i = 0; i < node->height; i++) {
        for (unsigned int j = 0; j < node->width; j++) {
            img_arr[row_offset + i][col_offset + j] = node->intensity;
        }
    }
    } else {
        unsigned int half_width = node->width / 2;
        unsigned int half_height = node->height / 2;

        save_qtree_as_ppm_helper(node->children[0], img_arr, row_offset, col_offset);                       
        save_qtree_as_ppm_helper(node->children[1], img_arr, row_offset, col_offset + half_width);         
        save_qtree_as_ppm_helper(node->children[2], img_arr, row_offset + half_height, col_offset);        
        save_qtree_as_ppm_helper(node->children[3], img_arr, row_offset + half_height, col_offset + half_width);
    }
}   

QTNode *load_preorder_qt(char *filename) {
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        perror("Error opening file");
        return NULL;
    }

    QTNode *root = load_preorder_qt_helper(fp);
    fclose(fp); 
    return root; 
}

QTNode *load_preorder_qt_helper(FILE *fp) {
    char type;
    if (fscanf(fp, " %c", &type) != 1) {
        return NULL;
    }

    QTNode *node = malloc(sizeof(QTNode));

    unsigned int intensity, row, height, col, width;

    fscanf(fp, "%u %u %u %u %u", &intensity, &row, &height, &col, &width);
    node->intensity = intensity;
    node->row = row;
    node->height = height;
    node->col = col;
    node->width = width;

    for (int i = 0; i < 4; i++) {
        node->children[i] = NULL;
    }

    if (type == 'N') {
       if (width > 1 && height > 1) {
            node->children[0] = load_preorder_qt_helper(fp); 
            node->children[1] = load_preorder_qt_helper(fp);
            node->children[2] = load_preorder_qt_helper(fp);
            node->children[3] = load_preorder_qt_helper(fp); 
        } 
        else if (width > 1 && height == 1) {
            node->children[0] = load_preorder_qt_helper(fp); 
            node->children[1] = load_preorder_qt_helper(fp); 
        } 
        else if (height > 1 && width == 1) {
            node->children[0] = load_preorder_qt_helper(fp); 
            node->children[2] = load_preorder_qt_helper(fp); 
        }
    }
    return node;
}

void save_preorder_qt(QTNode *root, char *filename) {
    (void)(root);
    FILE *fp = fopen(filename, "w");

    if ((fp = fopen(filename, "w")) == NULL){
        ERROR("could not write to file");
        exit(EXIT_FAILURE);
    }
    save_preorder_qt_helper(root, fp);
    fclose(fp);
}

void save_preorder_qt_helper(QTNode *node, FILE *fp){
    if(node == NULL){
        return;
    }
    unsigned int intensity, row, height, col, width;
    intensity = node->intensity;
    row = node->row;
    height = node->height;
    col = node->col;
    width = node->width;

    int isLeaf = 1;
    for (int i = 0; i < 4; i++) {
        if (node->children[i] != NULL) {
            isLeaf = 0; 
            break;
        }
    }
    if (isLeaf) {
        fprintf(fp, "%c %u %u %u %u %u\n", 'L', intensity, row, height, col, width);
    } else {
        fprintf(fp, "%c %u %u %u %u %u\n", 'N', intensity, row, height, col, width);
        for (int i = 0; i < 4; i++) {
            save_preorder_qt_helper(node->children[i], fp);
        }
    }
}
