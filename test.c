#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "png.h"
#include "pngwrapper.h"

int main(int argc, char **argv)
{
    if (argc != 2)
        abort_("Usage: program_name <file_in>");

    int x, y;
    int width;
    int height;
    png_structp* png_ptr;
    png_infop* info_ptr;
    png_byte color_type;
    png_byte bit_depth;

    png_bytep* row_pointers;
    char* png_binary_image;


    int position = 0;
    int ymin = 175;
    int ymax = 196;
    int xmin = 66;
    int xmax = 182;

    read_png_file(argv[1], &width, &height, &color_type, &bit_depth, row_pointers, png_binary_image);
    
    /*process_file(132);
    
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
    }*/
    

    //printf("%d, %s\n",argc, argv[1]);
    return 0;
}
