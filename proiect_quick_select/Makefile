build: serial_recursive serial_for openmp

serial_recursive: serial_recursive.c utils.h
	gcc serial_recursive.c utils.h -o serial_recursive

serial_for: serial_for.c utils.h
	gcc serial_for.c utils.h -o serial_for

openmp: openmp.c utils.h
	gcc -fopenmp openmp.c utils.h -o openmp

mpi: mpi.c utils.h
	mpicc mpi.c -o mpi

mpi_run:
	mpirun -np 6 ./mpi

openmp_run:
	./openmp 6

clean:
	rm serial_recursive serial_for openmp