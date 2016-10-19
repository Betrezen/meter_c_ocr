#include <stdlib.h>

#include "png.h"
#include "pngwrapper.h"
#include "dashmodel.h"


//extern void abort_(const char * s, ...);
//extern void read_png_file(char* file_name, png_structp* png_ptr, png_infop* info_ptr, int* width, int* height, png_byte* color_type, png_byte* bit_depth, png_bytep* row_pointers, char* png_binary_image);

int main(int argc, char **argv)
{
    /*if (argc != 2){
        printf("Usage: program_name <file_in>");
        return 1;
    }*/


    int x, y;
    int width;
    int height;
    png_structp png_ptr;
    png_infop info_ptr;
    png_byte color_type;
    png_byte bit_depth;

    png_bytepp row_pointers;
    char* png_binary_image;
    int* dash_image;
    int dash_count;


    read_png_file(argv[1], &png_ptr, &info_ptr, &width, &height, &color_type, &bit_depth, &row_pointers, &png_binary_image);
    printf("read_png_file competed is OK\n");

    process_file(THRESHOLD_BINARY, width, height, row_pointers, png_binary_image);
    printf("process_file competed is OK\n");

    int bbox[] = {66, 175, 116, 21};
    draw_binary_image(png_binary_image, width, height, bbox);
    printf("draw_binary_image competed is OK\n");
        
    int bbox2[4] = {66, 175, 120, 4};
    do_linerezation(png_binary_image, width, height, &dash_image, bbox2, &dash_count);
    printf("do_linerezation competed is OK. dash_count=%d\n", dash_count);

    draw_dashs(dash_image, dash_count, 176);
    draw_dashs(dash_image, dash_count, 177);

    //why 9*3 - because each item is x,y,width.... one array with periodic x,y,w....x,y,w....
    check_cross(&dash_image[0], &dash_image[9*3]);
    printf("\n\n\n");

    check_cross_lists(&dash_image[0*3], 8, &dash_image[9*3], 8);
    check_cross_lists(&dash_image[1*3], 7, &dash_image[9*3], 8);
    printf("\n\n\n");

    return 0;
}

