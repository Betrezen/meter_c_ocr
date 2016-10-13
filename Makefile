all: meter_ocr

meter_ocr: test.o pngwrapper.o
	gcc -o meter_ocr test.o pngwrapper.o -lpng -g

test.o: test.c
	gcc -c test.c -g

pngwrapper.o: pngwrapper.c
	gcc -c pngwrapper.c -lpng -g

clean:
	rm *.o
