#ifndef pngwrapper_h__
#define pngwrapper_h__

void abort_(const char * s, ...);

void read_png_file(char* file_name,
                   png_structp* png_ptr,
                   png_infop* info_ptr,
                   int* width, int* height,
                   png_byte* color_type,
                   png_byte* bit_depth,
                   png_bytep* row_pointers,
                   char* png_binary_image);

void write_png_file(char* file_name,
                    png_structp* png_ptr,
                    png_infop* info_ptr,
                    int width, int height,
                    png_byte color_type,
                    png_byte bit_depth,
                    png_bytep* row_pointers);

void process_file(int threshold,
                  png_structp* png_ptr,
                  png_infop* info_ptr, 
                  int width, int height,
                  png_bytep* row_pointers,
                  char* png_binary_image);

#endif
