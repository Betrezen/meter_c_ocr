# meter_c_ocr

compile pngwrapper:
gcc -c pngwrapper.c -lpng

compile test:
gcc -c test.c
gcc pngwrapper.o test.o -o test -lpng

compile all: gcc -c pngwrapper.c -lpng && gcc -c test.c && gcc pngwrapper.o test.o -o test -lpng
