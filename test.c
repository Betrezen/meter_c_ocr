#include <stdlib.h>

#include "png.h"
#include "pngwrapper.h"
#define THRESHOLD 132

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


    int position = 0;
    int ymin = 175;
    int ymax = 196;
    int xmin = 66;
    int xmax = 182;

    read_png_file("image.png"/*argv[1]*/, &png_ptr, &info_ptr, &width, &height, &color_type, &bit_depth, &row_pointers, &png_binary_image);
    printf("read_png_file competed is OK\n");

    process_file(THRESHOLD, width, height, row_pointers, png_binary_image);
    printf("process_file competed is OK\n");    
    
    for (y=0; y<height; y++) {
        for (x=0; x<width; x++) {
            position = y*width + x;
            if (y > ymin && y < ymax && x > xmin && x < xmax){
                if (png_binary_image[position] > 0){
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

    //printf("%d, %s\n",argc, argv[1]);
    return 0;
}
