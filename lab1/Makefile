build: serial crcw crew

serial: serial.c
	gcc serial.c -o serial

crcw: crcw.c
	gcc -fopenmp crcw.c -o crcw

crew: crew.c
	gcc -fopenmp crew.c -o crew

clean:
	rm serial crcw crew
