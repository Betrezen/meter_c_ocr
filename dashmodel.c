#include <stdlib.h>
#include "dashmodel.h"

// dash = [x,y,width, ... x_n, y_n, width_n]
// **dashs because  memory allocation otherwise malloc will replace addr
int do_linerezation(char* binary_image, int width, int heigh, char** dashs)
{
	//we assume worst case 2 point with signal separated by 1 point without
	//that means at least width*heigh/3. but we need to keep x,y,width.
	//so. we shall need max = width * height * sizeof(short) memory
	//why short? because x, y can be more 1000. char <= 255
	*dashs = (char*) malloc(sizeof(short) * width * height);
	int x,y;
	int i = 0;
	int mad = 0;

	for (y=0; y<height; y++) {
		mad = 0;
        for (x=0; x<width; x++) {
            position = y*width + x;
            if (binary_image[position] > 0){
                mad += 1;
            }
            else{
                if (mad < MIN_ALLOW_DASH_LENGTH){
                	mad = 0;
                	continue;
                }
                else{
                	dashs[i++] = mad;
                }
            }
            if (mad >= MIN_ALLOW_DASH_LENGTH){
                dashs[i++] = x-mad;
                dashs[i++] = y;
            }
        }
    }
}

int check_cross(char* line1, char* line2); // line [x,y,width]; return true/false
// obj1=[line1, line2, line3....]
int check_cross_lists(char* obj1, char* obj2);
// obj1 = [dash_count1, bbox_x1, bbox_y1, width1, heigh1, dash1...dashm]
int check_cross_objs(char* obj1, char* obj2);

// objects = [dash_count1, bbox_x1, bbox_y1, width1, heigh1, dash1...dashm; 
//       .....dash_count_n, bbox_xn, bbox_yn, widthn, heighn, dash_n1, dash_nm;]
// **objects because  memory allocation
int get_objects(char* dash, char** objects);
int get_bbox(char* object, char* box_x, char* box_y, char* width, char* heigh);
//objects - list of objects whcih shall be combined
int do_combination(char* objects);

int do_objects_filtering(char* objects);
int draw_object(char* obj);
int draw_bbox_object(char* obj);

int dash_model_run(char* binary_image);