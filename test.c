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


    read_png_file(argv[1], &png_ptr, &info_ptr, &width, &height, &color_type, &bit_depth, &row_pointers, &png_binary_image);
    printf("read_png_file competed is OK\n");

    process_file(THRESHOLD_BINARY, width, height, row_pointers, png_binary_image);
    printf("process_file competed is OK\n");

    int bbox[] = {66, 175, 116, 21};
    draw_binary_image(png_binary_image, width, height, bbox);
    printf("draw_binary_image competed is OK\n");
        
    int bbox2[] = {66, 175, 66+15, 4};
    do_linerezation(png_binary_image, width, height, &dash_image, bbox2);
    printf("do_linerezation competed is OK\n");

    return 0;
}
