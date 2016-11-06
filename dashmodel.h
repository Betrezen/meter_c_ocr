#ifndef dashmodel_h__
#define dashmodel_h__

#define MIN_SIGNAL_VALUE 1
#define MIN_ALLOW_DASH_LENGTH 2
#define MIN_ALLOW_CROSS_LENGTH 0
#define MIN_DASH_COUNT_PER_OBJECT 5
#define MAX_DASH_COUNT_PER_OBJECT 10000

#define MIN_DIGIT_WIDTH 13
#define MAX_DIGIT_WIDTH 15
#define MIN_DIGIT_HEIGH 17
#define MAX_DIGIT_HEIGH 20

#define CROP_IMAGE_X 66
#define CROP_IMAGE_Y 175
#define CROP_IMAGE_WIDTH 122
#define CROP_IMAGE_HEIGH 20

#define MAX_DASHES 100

typedef struct {
    int x;
    int y;
    int width;
} dash;

typedef struct {
    int dash_count;
    int bbox_x;
    int bbox_y;
    int bbox_width;
    int bbox_height;
    dash* dashes[MAX_DASHES];
} object;

// dash = [x,y,width, ... x_n, y_n, width_n]
// **dashs because  memory allocation otherwise malloc will replace addr
int do_linerezation(char* binary_image, int width, int height, dash** dashes, int box[4], int* dash_count);

int check_cross(int* line1, int* line2); // line [x,y,width]; return true/false
// obj1=[line1, line2, line3....]
int check_cross_lists(int* obj1, int obj1_count, int* obj2, int obj2_count);
// obj1 = [dash_count1, bbox_x1, bbox_y1, width1, heigh1, dash1...dashm]
int check_cross_objs(int* obj1, int* obj2);

// objects = [dash_count1, bbox_x1, bbox_y1, width1, heigh1, dash1...dashm; 
//       .....dash_count_n, bbox_xn, bbox_yn, widthn, heighn, dash_n1, dash_nm;]
// **objects because  memory allocation
int get_objects(int im_width, int im_height, dash* dashes, int num_dashs, object** ret_objects, int* ret_num_objs);
int get_bbox(int* dash_model, int box[4]);
//objects - list of objects whcih shall be combined
int do_combination(int* objects);

int do_objects_filtering(int* objects);

// box = [x,y,width,height]
int draw_binary_image(char* binary_image, int width, int height, int* box);
int draw_dashes(dash* dashes, int dash_count, int y);
int draw_object(int* obj);
int draw_bbox_object(int* obj);

int dash_model_run(char* binary_image, int width, int height);

#endif