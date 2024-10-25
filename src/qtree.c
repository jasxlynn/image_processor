#include "qtree.h"
#include <math.h> 

QTNode *create_quadtree(Image *image, double max_rmse) {
    unsigned int width = image->width;
    unsigned int height = image->height;
    return create_quadtree_helper(image, max_rmse, width, height, 0, 0);
}

QTNode *create_quadtree_helper(Image *image, double max_rmse, unsigned int width, unsigned int height, unsigned int row, unsigned int col){
    QTNode *node = (QTNode *)malloc(sizeof(QTNode));

    double avg_intensity = calculate_avg_intensity(image, row, col, width, height);
    node->intensity = (unsigned char)avg_intensity;
    node->row = row;
    node->col = col; 
    node->height = height;
    node->width = width;

    double rmse = calculate_rmse(image, max_rmse, row, col, width, height);
    if(rmse <= max_rmse){
        node->children[0] = node->children[1] = node->children[2] = node->children[3] = NULL;
        return node;
    }

    if(width > 1 && height > 1){
        unsigned int half_width = width / 2;
        unsigned int half_height = height / 2;
        node->children[0] = create_quadtree_helper(image, max_rmse, half_width, half_height, row, col);
        node->children[1] = create_quadtree_helper(image, max_rmse, half_width, half_height, row, col + half_width);
        node->children[2] = create_quadtree_helper(image, max_rmse, half_width, half_height, row + half_height, col);
        node->children[3] = create_quadtree_helper(image, max_rmse, half_width, half_height, row + half_height, col + half_width);
    } else if(width > 1 && height == 1){
        unsigned int half_width = width / 2;
        node->children[0] = create_quadtree_helper(image, max_rmse, half_width, height, row, col);
        node->children[1] = create_quadtree_helper(image, max_rmse, width - half_width, height, row, col + half_width);
        node->children[2] = node->children[3] = NULL;
    }else if(height > 1 && width == 1){
        unsigned int half_height = height / 2;
        node->children[0] = create_quadtree_helper(image, max_rmse, width, half_height, row, col);
        node->children[2] = create_quadtree_helper(image, max_rmse, width, height - half_height, row + half_height, col);
        node->children[1] = node->children[3] = NULL;
    }else{
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
    printf("Total intensity %f\n", total_intensity);
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
    return node->children[0];
}

QTNode *get_child2(QTNode *node) {
    return node->children[1];
}

QTNode *get_child3(QTNode *node) {
    return node->children[2];
}

QTNode *get_child4(QTNode *node) {
    return node->children[3];
}

unsigned char get_node_intensity(QTNode *node) {
    (void)node;
    return 0;
}

void delete_quadtree(QTNode *root) {
    (void)root;
}

void save_qtree_as_ppm(QTNode *root, char *filename) {
    (void)root;
    (void)filename;
}

QTNode *load_preorder_qt(char *filename) {
    (void)filename;
    return NULL;
}

void save_preorder_qt(QTNode *root, char *filename) {
    (void)root;
    (void)filename;
}

