#include <stdlib.h>
#include <stdio.h>

#include "dashmodel.h"

// dash = [x,y,width, ... x_n, y_n, width_n]
// **dashs because  memory allocation otherwise malloc will replace addr
int do_linerezation(char* binary_image, int width, int height, int** dashs, int* box)
{
    //we assume worst case 2 point with signal separated by 1 point without
    //that means at least width*heigh/3. but we need to keep x,y,width.
    //so. we shall need max = width * height * sizeof(int) memory
    //why int? because x, y can be more 1000. char <= 255
    //bbox  = [x,y,width, height] - crop bounding box
    printf("do_linerezation step00");

    *dashs = (int*) malloc(sizeof(int) * width * height);
    int x,y;
    int i = 0;
    int mad = 0;
    int position = 0;    

    printf("do_linerezation step0");
    int ymin = *(box+1); //175;
    int ymax = *(box+1) + *(box+3); //196;
    int xmin = *box; //66;
    int xmax = *box + *(box+2); //182;

    //seems nothing shall be crop
    if (ymin == ymax && xmin == xmax){
        ymin = 0;
        ymax = height;
        xmin = 0;
        xmax = width;
    }

    printf("do_linerezation step1");
    for (y=0; y<height; y++) {
        mad = 0;
        for (x=0; x<width; x++) {
            if (y > ymin && y < ymax && x > xmin && x < xmax){
                position = y*width + x;
                if (binary_image[position] > 0){
                    mad += 1;
                }
                else{
                    if (mad < MIN_ALLOW_DASH_LENGTH){
                        mad = 0;
                    }
                    else{
                        *dashs[i++] = x-mad;
                        *dashs[i++] = y;
                        *dashs[i++] = mad;
                        mad=0;
                    }
                }
            }
        }
    }
    printf("do_linerezation step-latest");
}

int check_cross(int* line1, int* line2)
{
    return 0;
}
// obj1=[line1, line2, line3....]
int check_cross_lists(int* obj1, int* obj2)
{
    return 0;
}

// obj1 = [dash_count1, bbox_x1, bbox_y1, width1, heigh1, dash1...dashm]
int check_cross_objs(int* obj1, int* obj2)
{
    return 0;
}

// objects = [dash_count1, bbox_x1, bbox_y1, width1, heigh1, dash1...dashm; 
//       .....dash_count_n, bbox_xn, bbox_yn, widthn, heighn, dash_n1, dash_nm;]
// **objects because  memory allocation
int get_objects(int* dash, int** objects)
{
    return 0;
}
int get_bbox(int* object, int* box_x, int* box_y, int* width, int* height)
{
    return 0;
}
//objects - list of objects whcih shall be combined
int do_combination(int* objects)
{
    return 0;
}
int do_objects_filtering(int* objects)
{
    return 0;
}

int draw_binary_image(char* binary_image, int width, int height, int* box)
{
    int position = 0;
    int ymin = box[1]; //175;
    int ymax = box[1] + box[3]; //196;
    int xmin = box[0]; //66;
    int xmax = box[0] + box[2]; //182;
    int x,y;

    //seems nothing shall be crop
    if (ymin == ymax && xmin == xmax){
        ymin = 0;
        ymax = height;
        xmin = 0;
        xmax = width;
    }

    printf("draw_binary_image: %d, %d, %d, %d; \n", xmin, ymin, xmax, ymax);
    for (y=0; y<height; y++) {
        for (x=0; x<width; x++) {
            position = y*width + x;
            //printf("%d\n", position);
            if (y > ymin && y < ymax && x > xmin && x < xmax){
                if (binary_image[position] > 0){
                    printf("-");
                }
                else{
                    printf("*");
                }
            }
        }
        if (y > ymin && y < ymax){
            printf("\n");
        }
    }
    return 0;
}

int draw_object(int* obj)
{
    return 0;
}
int draw_bbox_object(int* obj)
{
    return 0;
}
int dash_model_run(char* binary_image, int width, int height)
{
    return 0;
}