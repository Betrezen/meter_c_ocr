#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <limits.h>

#include "dashmodel.h"

// dash = [x,y,width, ... x_n, y_n, width_n]
// **dashs because  memory allocation otherwise malloc will replace addr
int do_linerezation(char* binary_image, int width, int height, dash** dashes, int box[4], int* dash_count)
{
    //we assume worst case 2 point with signal separated by 1 point without
    //that means at least width*heigh/3. but we need to keep x,y,width.
    //so. we shall need max = width * height * sizeof(int) memory
    //why int? because x, y can be more 1000. char <= 255
    //bbox  = [x,y,width, height] - crop bounding box

    int x,y;
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

    *dashes = (dash*) malloc(sizeof(dash) * width * height);

    for (y = ymin; y < ymax; y++)
    {
        mad = 0;
        for (x = xmin; x < xmax; x++)
        {
            //printf("[%d,%d]\n", x, y);
            position = y * width + x;
            if (binary_image[position] > 0)
            {
                ++mad;
            }
            else
            {
                if (mad >= MIN_ALLOW_DASH_LENGTH)
                {
                    (*dashes)[*dash_count].x = x-mad;
                    (*dashes)[*dash_count].y = y;
                    (*dashes)[*dash_count].width = mad;
                    ++(*dash_count);
                }
                mad = 0;
            }
        }
    }
}

int check_cross(dash* line1, dash* line2)
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

    int x1 = line1->x;
    int y1 = line1->y;
    int x11 = line1->x+line1->width;
    int x2 = line2->x;
    int y2 = line2->y;
    int x22 = line2->x+line2->width;
    //printf("\nline1[%d,%d--%d] line2[%d, %d--%d], mac=%d, macd=%d  ", y1, x1,x11, y2, x2, x22, MIN_ALLOW_DASH_LENGTH, MIN_ALLOW_CROSS_LENGTH);

    if(((x11-x1) < MIN_ALLOW_DASH_LENGTH) || ((x22-x2) < MIN_ALLOW_DASH_LENGTH)){
        //printf("DONT CROSS: case_00");
        return -1;
    }
    if(x1 <= x2 && x11 <= x22 && x11 >= x2 && (x11-x2) >= MIN_ALLOW_CROSS_LENGTH) {
        //printf("CROSS: case_a");
        return 1;
    }
    if(x1 >= x2 && x11 >= x22 && x22 >= x1 && (x22-x1) >= MIN_ALLOW_CROSS_LENGTH) {
        //printf("CROSS: case_b");
        return 1;
    }
    if(x1 >= x2 && x11 <= x22 && (x11-x1) >= MIN_ALLOW_CROSS_LENGTH) {
        //printf("CROSS: case_c");
        return 1;
    }
    if(x1 <= x2 && x11 >= x22 && (x22-x2) >= MIN_ALLOW_CROSS_LENGTH) {
        //printf("CROSS: case_d");
        return 1;
    }
    //printf("DONT CROS: case_0");
    return 0;
}

// obj1=[line1, line2, line3....]
int check_cross_lists(object* obj1, object* obj2)
{
    int i, j;
    for (i = 0; i < obj1->dash_count; i++)
    {
        for (j = 0; j < obj2->dash_count; j++)
        {
            if (check_cross(obj1->dashes[i], obj2->dashes[i]))
                return 1;
        }
    }
    return 0;
}



static int min(int val1, int val2)
{
    return val1 < val2 ? val1 : val2;
}
static int max(int val1, int val2)
{
    return val1 > val2 ? val1 : val2;
}

int intersection_in_levels
(
    int *num_obj1_dashes_on_level, int *num_obj2_dashes_on_level,
    dash*** obj1_dashes_on_level, dash*** obj2_dashes_on_level,
    int level1, int level2
)
{
    int j, k;
    int int_line_x1, int_line_x2;
    for (j = 0; j < num_obj1_dashes_on_level[level1]; ++j)
    {
        for (k = 0; k < num_obj2_dashes_on_level[level2]; ++k)
        {
            int_line_x1 = max(obj1_dashes_on_level[level1][j]->x, obj2_dashes_on_level[level1][k]->x);
            int_line_x2 = min(obj1_dashes_on_level[level1][j]->x + obj1_dashes_on_level[level1][j]->width,
                              obj2_dashes_on_level[level2][k]->x + obj2_dashes_on_level[level2][k]->width);
            if (int_line_x1 <= int_line_x2)
            {
                return 1;
            }
        }
    }
    return 0;
}

// obj1 = [dash_count1, bbox_x1, bbox_y1, width1, heigh1, dash1...dashm]
int check_cross_objs(object* obj1, object* obj2, int im_height)
{
    // if we dont have lines on the same y for both objects
    // or we dont have lines on y-1 and y for both objects
    // then dont need to do analysis because nothing can be compare
    // if we don't have the same lines in both objects then need find cross

    int i = 0;
    int result;
    int obj1_min_h = INT_MAX, obj1_max_h = 0;
    int obj2_min_h = INT_MAX, obj2_max_h = 0;

    int target_min, target_max;
    int* num_obj1_dashes_on_level = (int*) malloc(sizeof(int) * im_height);
    dash*** obj1_dashes_on_level = (dash***) malloc(sizeof(dash**) * im_height);
    for (i = 0; i < im_height; ++i)
        obj1_dashes_on_level[i] = malloc(sizeof(dash*) * MAX_DASHES);

    int* num_obj2_dashes_on_level = (int*) malloc(sizeof(int) * im_height);
    dash*** obj2_dashes_on_level = (dash***) malloc(sizeof(dash**) * im_height);
    for (i = 0; i < im_height; ++i)
        obj2_dashes_on_level[i] = malloc(sizeof(dash*) * MAX_DASHES);

    memset(num_obj1_dashes_on_level, 0, im_height * sizeof(int));
    memset(num_obj2_dashes_on_level, 0, im_height * sizeof(int));

    for (i = 0; i < obj1->dash_count; ++i)
    {
        obj1_dashes_on_level[obj1->dashes[i]->y][num_obj1_dashes_on_level[obj1->dashes[i]->y]++] = obj1->dashes[i];
        obj1_min_h = min(obj1_min_h, obj1->dashes[i]->y);
        obj1_max_h = max(obj1_max_h, obj1->dashes[i]->y);
    }

    for (i = 0; i < obj2->dash_count; ++i)
    {
        obj2_dashes_on_level[i][num_obj2_dashes_on_level[i]++] = obj1->dashes[i];
        obj2_min_h = min(obj2_min_h, obj2->dashes[i]->y);
        obj2_max_h = max(obj2_max_h, obj2->dashes[i]->y);
    }

    target_max = min(obj1_max_h, obj2_max_h);
    target_min = max(obj1_min_h, obj2_min_h);

    if (target_min < target_max)
    {
        // Find intersected in one level
        for (i = target_min; i < target_max && result == 0; ++i)
        {
            result = intersection_in_levels(
                num_obj1_dashes_on_level, num_obj2_dashes_on_level,
                obj1_dashes_on_level, obj2_dashes_on_level, i, i
            ); // Check one level intersections
            if (result == 0)
            {
                result = intersection_in_levels(
                        num_obj1_dashes_on_level, num_obj2_dashes_on_level,
                        obj1_dashes_on_level, obj2_dashes_on_level, i, i + 1
                ); // Check neighb level intersections
            }
            if (result == 0)
            {
                result = intersection_in_levels(
                        num_obj1_dashes_on_level, num_obj2_dashes_on_level,
                        obj1_dashes_on_level, obj2_dashes_on_level, i, i - 1
                ); // Check neighb level intersections
            }
        }
    }

    for (i = 0; i < im_height; ++i)
    {
        free(obj1_dashes_on_level[i]);
        free(obj2_dashes_on_level[i]);
    }
    free(obj1_dashes_on_level);
    free(obj2_dashes_on_level);
    free(num_obj1_dashes_on_level);
    free(num_obj2_dashes_on_level);

    return result;
}

// objects = [dash_count1, bbox_x1, bbox_y1, width1, heigh1, dash1...dashm;
//       .....dash_count_n, bbox_xn, bbox_yn, widthn, heighn, dash_n1, dash_nm;]
// **objects because  memory allocation
int get_objects(int im_width, int im_height, dash* dashes, int num_dashs, object** ret_objects, int* ret_num_objs)
{
    int i, j, k, l;
    int num_objs = 0;
    int max_level = 0;
    int min_level = im_height;
    uint8_t cross_flag;
    object* objects = (object *) malloc(num_dashs * sizeof(object));
    dash*** dashes_one_level = (dash***) malloc(im_height * sizeof(dash**));
    int* num_dashes_one_level = (int*) malloc(im_height * sizeof(int));

    for (i = 0; i < im_height; ++i)
    {
        dashes_one_level[i] = (dash**) malloc(MAX_DASHES * sizeof(dash*));
    }
    memset(num_dashes_one_level, 0, sizeof(int) * im_height);

    for (i = 0; i < num_dashs; ++i)
    {
        dashes_one_level[dashes[i].y][num_dashes_one_level[dashes[i].y]++] = &dashes[i];
        if (max_level < dashes[i].y)
        {
            max_level = dashes[i].y;
        }
        if (min_level > dashes[i].y)
        {
            min_level = dashes[i].y;
        }
    }

    // we are looking at mountain and find peak. There are initial objects here. 
    //if (num_dashes_one_level[mix_level] > 0) {
    //    for (i = 0; i < num_dashes_one_level[mix_level]; ++i) {
    //        objects[num_objs].dash_count = 1;
    //        objects[num_objs].dashes[0] = dashes_one_level[mix_level][i];
    //        num_objs++;
    //    }
    //}

    for (i = min_level; i < max_level; ++i)
    {
        if (num_dashes_one_level[i] == 0)
            continue;

        for (j = 0; j < num_dashes_one_level[i]; ++j)
        {
            cross_flag = 0;

            for (k = 0; k < num_objs; ++k)
            {
                for (l = 0; l < objects[k].dash_count; ++l)
                {
                    if (objects[k].dashes[l]->y == (i - 1) && check_cross(dashes_one_level[i][j], objects[k].dashes[l]) == 1)
                    {
                        cross_flag = 1;
                        objects[k].dashes[objects[k].dash_count++] = dashes_one_level[i][j];
                    }
                }
            }

            if (cross_flag == 0)
            {
                objects[num_objs].dash_count = 1;
                objects[num_objs].dashes[0] = dashes_one_level[i][j];
                num_objs++;
            }
        }
    }

    *ret_objects = objects;
    *ret_num_objs = num_objs;

    for (int i = 0; i < im_height; ++i)
    {
        free(dashes_one_level[i]);
    }
    free(dashes_one_level);
    free(num_dashes_one_level);
    return 0;
}

void get_bbox(object* obj, int bbox[4])
{
    int i, xmin=INT_MAX, xmax=0, ymin=INT_MAX, ymax=0;
    bbox[0] = INT_MAX;
    bbox[1] = INT_MAX;
    bbox[2] = 0;
    bbox[3] = 0;
    for (i = 0; i < obj->dash_count; ++i)
    {
        if (xmin > obj->dashes[i]->x)
            xmin = obj->dashes[i]->x;
        if (ymin > obj->dashes[i]->y)
            ymin = obj->dashes[i]->y;
        if (xmax < (obj->dashes[i]->x + obj->dashes[i]->width))
            xmax = (obj->dashes[i]->x + obj->dashes[i]->width);
        if (ymax < obj->dashes[i]->y)
            ymax = obj->dashes[i]->y;
    }
    bbox[0] = xmin;
    bbox[1] = ymin;
    bbox[2] = xmax;
    bbox[3] = ymax;
}

//objects - list of objects whcih shall be combined
int do_combination(object* objects, int* num_objs, int im_height)
{
    int i, j, k;
    int obj_num = *num_objs;
    int merged;

    object for_swap;

    for (i = 0; i < obj_num; ++i)
    {
        merged = 0;
        for (j = 0; j < obj_num && merged == 0; ++j)
        {
            if (i == j)
                continue;

            if (check_cross_objs(&objects[i], &objects[j], im_height) == 1)
            {
                for (k = 0; k < objects[j].dash_count; ++k)
                {
                    objects[i].dashes[objects[i].dash_count + k] = objects[j].dashes[k];
                }
                for_swap = objects[j];
                objects[j] = objects[obj_num - 1];
                objects[obj_num - 1] = for_swap;
                obj_num--;
                merged = 1;
            }
        }
    }
    *num_objs = obj_num;
    return 0;
}

int do_objects_filtering (object* objects, int num_objs,
    int bbox[4], int num_dashs,
    int** object_indexes, int* count_indexes)
{
    // if bbox has 0, 0, 0, 0 that means we do not do filtering by bbox
    // if num_dashs has 0 than means we do not do filtering by num dashes
    int i, j;
    int obj_bbox[4];
    int variant=0;

    *object_indexes = (int*) malloc(sizeof(int) * num_objs);

    if (bbox[0] == 0 && bbox[1] == 0 && bbox[0] == bbox[2] && bbox[1] == bbox[3])
        variant = 1;
    else if (num_dashs == 0)
        variant = 2;
    
    printf("\n variant = %d", variant);
    
    for (i = 0, j = 0; i < num_objs; i++)
    {
        
        if (variant == 1)
        {
            get_bbox(&objects[i], obj_bbox);
            if ((obj_bbox[2]-obj_bbox[0]) >= (bbox[2]-bbox[0]) &&
                (obj_bbox[3]-obj_bbox[1]) >= (bbox[3]-bbox[1]))
                (*object_indexes)[j++] = i;
        }
        else if (variant == 2 && objects[i].dash_count >= num_dashs)
        {
            (*object_indexes)[j++] = i;
        }
        else
        {
            (*object_indexes)[j++] = i;
        }
    }
    *count_indexes = j;
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

int draw_object(object* obj, int bbox[4])
{
    int i, j, k, xmin=bbox[0], xmax=bbox[2], ymin=bbox[1], ymax=bbox[3];
    for (i=ymin; i<=ymax; i++){
        for (j=xmin; j<=xmax; j++){
            for (k=0; k<obj->dash_count; k++){
                if (obj->dashes[k]->y == i){
                    if (j > (obj->dashes[k]->x) && j <= (obj->dashes[k]->x + obj->dashes[k]->width))
                    {
                        printf("*");
                    }
                    else{
                        printf("-");   
                    }

                }

            }
        }
        printf("\n");
    }
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