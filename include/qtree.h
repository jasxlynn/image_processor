#ifndef QTREE_H
#define QTREE_H

#include <stdio.h>
#include <stdlib.h>

#include "image.h"

#define INFO(...) do {fprintf(stderr, "[          ] [ INFO ] "); fprintf(stderr, __VA_ARGS__); fprintf(stderr, "\n"); fflush(stderr);} while(0)
#define ERROR(...) do {fprintf(stderr, "[          ] [ ERR  ] "); fprintf(stderr, __VA_ARGS__); fprintf(stderr, "\n"); fflush(stderr);} while(0) 

typedef struct QTNode {
    unsigned char node;
    unsigned char intensity;
    unsigned int row;
    unsigned int height;
    unsigned int col;   
    unsigned int width;
    struct QTNode *children[4];
} QTNode;

double calculate_avg_intensity(Image *image, unsigned int row, unsigned int col, unsigned int height, unsigned int width);
double calculate_rmse(Image *image, double avg_intensity, unsigned int row, unsigned int col, unsigned int width, unsigned int height);
QTNode *create_quadtree_helper(Image *image, double max_rmse, unsigned int width, unsigned int height, unsigned int row, unsigned int col);


QTNode *create_quadtree(Image *image, double max_rmse);  
QTNode *get_child1(QTNode *node);
QTNode *get_child2(QTNode *node);
QTNode *get_child3(QTNode *node);
QTNode *get_child4(QTNode *node);
unsigned char get_node_intensity(QTNode *node);
void delete_quadtree(QTNode *root);
void save_qtree_as_ppm(QTNode *root, char *filename); 

void save_qtree_as_ppm_helper(QTNode *node, unsigned char **img_arr, unsigned int row_offset, unsigned int col_offset);

QTNode *load_preorder_qt(char *filename);
QTNode *create_internal_node(unsigned int intensity, unsigned int row, unsigned int height, unsigned int col, unsigned int width);
QTNode *create_leaf_node(unsigned int intensity, unsigned int row, unsigned int height, unsigned int col, unsigned int width);
QTNode *load_preorder_qt_helper(FILE *fp);
void save_preorder_qt(QTNode *root, char *filename);  

#endif // QTREE_H