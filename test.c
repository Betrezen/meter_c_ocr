#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "png.h"
#include "pngwrapper.h"

void abortme(const char * s, ...)
{
        va_list args;
        va_start(args, s);
        vfprintf(stderr, s, args);
        fprintf(stderr, "\n");
        va_end(args);
        abort();
}

void read_png_file(char* file_name,
                   png_structp* png_ptr,
                   png_infop* info_ptr,
                   int* width, int* height,
                   png_byte* color_type,
                   png_byte* bit_depth,
                   png_bytep* row_pointers,
                   char* png_binary_image)
{
    int x, y;
    int number_of_passes;
    unsigned char header[8];    // 8 is the maximum size that can be checked

    // open file and test for it being a png
    FILE *fp = fopen(file_name, "rb");
    if (!fp)
            abortme("File %s could not be opened for reading", file_name);
    fread(header, 1, 8, fp);
    if (png_sig_cmp(header, 0, 8))
            abortme("File %s is not recognized as a PNG file", file_name);
    //printf("png header is OK\n");

    // initialize stuff
    *png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    //printf("png_ptr is OK \n");

    if (!*png_ptr)
            abortme("png_create_read_struct failed");

    *info_ptr = png_create_info_struct(*png_ptr);
    //printf("info_ptr is OK \n");

    if (!*info_ptr)
            abortme("png_create_info_struct failed");

    if (setjmp(png_jmpbuf(*png_ptr)))
            abortme("Error during init_io");

    png_init_io(*png_ptr, fp);
    png_set_sig_bytes(*png_ptr, 8);
    png_read_info(*png_ptr, *info_ptr);

    *width = png_get_image_width(*png_ptr, *info_ptr);
    *height = png_get_image_height(*png_ptr, *info_ptr);
    *color_type = png_get_color_type(*png_ptr, *info_ptr);
    *bit_depth = png_get_bit_depth(*png_ptr, *info_ptr);
    number_of_passes = png_set_interlace_handling(*png_ptr);
    png_read_update_info(*png_ptr, *info_ptr);
    //printf("width=%d, height=%d, color_type=%d, bit_depth=%d, number_of_passes=%d\n", 
    //    *width, *height, *color_type, *bit_depth, number_of_passes);


    // read file
    if (setjmp(png_jmpbuf(*png_ptr)))
            abortme("Error during read_image");

    //printf("read_png_file step n \n");
    png_binary_image = (char*) malloc(sizeof(char) * (*width) * (*height));
    row_pointers = (png_bytep*) malloc(sizeof(png_bytep) * (*height));
    //printf("read_png_file step m \n");

    for (y=0; y<(*height); y++)
            row_pointers[y] = (png_byte*) malloc(png_get_rowbytes(*png_ptr, *info_ptr));

    //printf("read_png_file step s \n");
    png_read_image(*png_ptr, row_pointers);
    fclose(fp);
    //printf("read_png_file step t \n");    
}


void write_png_file(char* file_name,
                    int width, int height,
                    png_byte color_type,
                    png_byte bit_depth,
                    png_bytep* row_pointers)
{
    int x, y;
    png_structp png_ptr;
    png_infop info_ptr;

    // create file
    FILE *fp = fopen(file_name, "wb");
    if (!fp)
            abortme("File %s could not be opened for writing", file_name);
    // initialize stuff
    png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr)
            abortme("png_create_write_struct failed");
    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr)
            abortme("png_create_info_struct failed");
    if (setjmp(png_jmpbuf(png_ptr)))
            abortme("Error during init_io");
    png_init_io(png_ptr, fp);
    // write header
    if (setjmp(png_jmpbuf(png_ptr)))
            abortme("Error during writing header");

    png_set_IHDR(png_ptr, info_ptr, width, height,
                 bit_depth, color_type, PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
    png_write_info(png_ptr, info_ptr);
    // write bytes
    if (setjmp(png_jmpbuf(png_ptr)))
            abortme("Error during writing bytes");
    png_write_image(png_ptr, row_pointers);
    // end write
    if (setjmp(png_jmpbuf(png_ptr)))
            abortme("Error during end of write");
    png_write_end(png_ptr, NULL);
    // cleanup heap allocation
    for (y=0; y<height; y++)
            free(row_pointers[y]);
    free(row_pointers);
    fclose(fp);
}


void process_file(int threshold,
                  int width, int height,
                  png_bytep* row_pointers,
                  char* png_binary_image)
{
    int rgba = 0;
    int binary_value = 0;
    float grayvalue = 0.0;
    int position = 0;
    int x, y;        

    printf("process_file: start ok");

    printf("process_file: init ok");

    for (y=0; y<height; y++) {
        png_byte* row = row_pointers[y];
        for (x=0; x<width; x++) {
            png_byte* ptr = &(row[x*4]);
            grayvalue = 0.299*ptr[0] + 0.587*ptr[1] + 0.114*ptr[2];
            binary_value = 0;
            if (grayvalue > threshold){
                binary_value = 1;
            }
            if (rgba > 0){                              
                printf("Pixel at position [ %d - %d ] has RGBA values: %d - %d - %d - %d\n", x, y, ptr[0], ptr[1], ptr[2], ptr[3]);
            }
            else{
                // Y'=0.299R+0.587G+0.114B  OR  Y'=0.2126R+0.7152G+0.0722B
                //printf("Pixel at position [ %d - %d ] has RGB values: %d - %d - %d\n", x, y, ptr[0], ptr[1], ptr[2]);
                //write binary image
                position = y*width + x;
                png_binary_image[position] = binary_value;
            }
        }
    }

}
