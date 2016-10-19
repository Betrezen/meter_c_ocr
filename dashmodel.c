#include <stdlib.h>
#include <stdio.h>

#include "dashmodel.h"

// dash = [x,y,width, ... x_n, y_n, width_n]
// **dashs because  memory allocation otherwise malloc will replace addr
int do_linerezation(char* binary_image, int width, int height, int** dashs, int box[4], int* dash_count)
{
    //we assume worst case 2 point with signal separated by 1 point without
    //that means at least width*heigh/3. but we need to keep x,y,width.
    //so. we shall need max = width * height * sizeof(int) memory
    //why int? because x, y can be more 1000. char <= 255
    //bbox  = [x,y,width, height] - crop bounding box

    int x,y;
    int i = 0;
    int mad = 0;
    int position = 0;
    *dash_count = 0;

    int ymin = box[1]; //175;
    int ymax = box[1] + box[3]; //196;
    int xmin = box[0]; //66;
    int xmax = box[0] + box[2]; //182;

    //seems nothing shall be crop
    if (ymin == ymax && xmin == xmax){
        ymin = 0;
        ymax = height;
        xmin = 0;
        xmax = width;
    }

    *dashs = (int*) malloc(sizeof(int) * width * height);

    for (y=0; y<height; y++) {
        mad = 0;
        for (x=0; x<width; x++) {
            if (y > ymin && y < ymax && x > xmin && x < xmax){
                position = y*width + x;
                if (binary_image[position] > 0){
                    ++mad;
                }
                else{
                    if (mad < MIN_ALLOW_DASH_LENGTH){
                        mad = 0;
                    }
                    else{
                        (*dashs)[i++] = x-mad;
                        (*dashs)[i++] = y;
                        (*dashs)[i++] = mad;
                        ++(*dash_count);
                        //printf("x=%d, y=%d, w=%d\n", x,y,mad);
                        mad=0;
                    }
                }
            }
        }
    }
}

int check_cross(int* line1, int* line2)
{
    /* we should find several cases if line1 and line2 have cross by X ordinat
        line = (x,y,weight) -> *--- -> x,y= * and weight= ---
        x1 ---------- x11 -> min distance x2-x1 = weight; weight >= min allow distance. MAD
        x1----------x11        we have cross here by X ordinat.
             x2----------x22   min allow cross distance = (x22 - x11).                  MACD

        case a                 if x1<=x2 and x11<=x22 and x11>x2 and (x11-x2)>=MACD
          x1----------x11
               x2-----------x22

        case b                 if x1>=x2 and x11>=x22 and x22>x1 and (x22-x1)>=MACD
               x1----------x11
          x2-----------x22

        case c                 if x1>=x2 and x11<=x22 and (x11-x1)>=MACD
             x1---x11
          x2-----------x22

        case d                 if x1<=x2 and x22<=x11 and (x22-x2)>=MACD
          x1----------x11
             x2---x22

        case e                 if x1<=x2 and x11==x2. That shall be covered by a/b if macd=0
          x1----------x11
                      x2-----------x22
    */

    int x1 = line1[0];
    int y1 = line1[1];
    int x11 = line1[0]+line1[2];
    int x2 = line2[0];
    int y2 = line2[1];
    int x22 = line2[0]+line2[2];
    printf("\nline1[%d,%d--%d] line2[%d, %d--%d], mac=%d, macd=%d  ", y1, x1,x11, y2, x2, x22, MIN_ALLOW_DASH_LENGTH, MIN_ALLOW_CROSS_LENGTH);

    if(((x11-x1) < MIN_ALLOW_DASH_LENGTH) || ((x22-x2) < MIN_ALLOW_DASH_LENGTH)){
        printf("DONT CROSS: case_00");
        return -1;
    }
    if(x1 <= x2 && x11 <= x22 && x11 >= x2 && (x11-x2) >= MIN_ALLOW_CROSS_LENGTH) {
        printf("CROSS: case_a");
        return 1;
    }
    if(x1 >= x2 && x11 >= x22 && x22 >= x1 && (x22-x1) >= MIN_ALLOW_CROSS_LENGTH) {
        printf("CROSS: case_b");
        return 1;
    }
    if(x1 >= x2 && x11 <= x22 && (x11-x1) >= MIN_ALLOW_CROSS_LENGTH) {
        printf("CROSS: case_c");
        return 1;
    }
    if(x1 <= x2 && x11 >= x22 && (x22-x2) >= MIN_ALLOW_CROSS_LENGTH) {
        printf("CROSS: case_d");
        return 1;
    }
    printf("DONT CROS: case_0");
    return 0;
}

// obj1=[line1, line2, line3....]
int check_cross_lists(int* obj1, int obj1_count, int* obj2, int obj2_count)
{
    int i,j;
    for (i=0; i<obj1_count; i++){
        for (j=0; j<obj2_count; j++){
            if (check_cross((obj1+i*3), (obj2+j*3)))
                return 1;
        }
    }
    return 0;
}

// obj1 = [dash_count1, bbox_x1, bbox_y1, width1, heigh1, dash1...dashm]
int check_cross_objs(int* obj1, int* obj2)
{
    // if we dont have lines on the same y for both objects
    // or we dont have lines on y-1 and y for both objects
    // then dont need to do analysis because nothing can be compare
    // if we don't have the same lines in both objects then need find cross
    return 0;
}

// objects = [dash_count1, bbox_x1, bbox_y1, width1, heigh1, dash1...dashm;
//       .....dash_count_n, bbox_xn, bbox_yn, widthn, heighn, dash_n1, dash_nm;]
// **objects because  memory allocation
int get_objects(int* dash, int** objects)
{
    return 0;
}

int get_bbox(int* dash_model, int box[4])
{
    // we have dash model and we need return back bbox around of this model
    /*
    x_values = [i[0] for i in obj]
    y_values = [i[1] for i in obj]
    x_max_values = [i[0] + i[2] for i in obj]
    x_min = min(x_values)
    y_min = min(y_values)
    x_max = max(x_max_values)
    y_max = max(y_values)
    return (x_min, y_min, x_max, y_max)
    */
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
                    printf("*");
                }
                else{
                    printf("-");
                }
            }
        }
        if (y > ymin && y < ymax){
            printf("\n");
        }
    }
    return 0;
}

int draw_dashs(int* dash, int dash_count, int ylevel)
{
    int x,y,width,i,j,latest_x;
    printf("\n");
    for (i=0,latest_x=0; i<dash_count; i++){
        x = dash[3*i];
        y = dash[3*i+1];
        width = dash[3*i+2];
        if (y == ylevel){
            printf("i[%d]x[%d--%d]",i, x, x+width);
            if (latest_x == 0){
                latest_x = x;
            }
            //printf("x[%d]y[%d]w[%d]l_x[%d]", x,y,width,latest_x);
            for (j=0; j<(x-latest_x); j++){
                printf("-");
            }
            for (j=0; j<width; j++){
                printf("*");
            }
            //printf("\n");
            latest_x=x+width;
        }
    }
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