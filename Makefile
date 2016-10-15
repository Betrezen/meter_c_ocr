all: meter_ocr

meter_ocr: test.o pngwrapper.o dashmodel.o
	gcc -o meter_ocr test.o dashmodel.o pngwrapper.o -lpng -g

test.o: test.c
	gcc -c test.c -g

pngwrapper.o: pngwrapper.c
	gcc -c pngwrapper.c -lpng -g

dashmodel.o: dashmodel.c
	gcc -c dashmodel.c -g

clean:
	rm *.o
