build: serial openmp pthreads mpi mpi-openmp mpi-pthreads

serial: serial.c utils.h
	gcc serial.c -o serial

openmp: openmp.c utils.h
	gcc -fopenmp openmp.c -o openmp

pthreads: pthreads.c utils.h
	gcc pthreads.c -o pthreads -lpthread -lm

mpi: mpi.c utils.h
	mpicc mpi.c -o mpi

mpi-openmp: mpi-openmp.c utils.h
	mpicc -fopenmp mpi-openmp.c -o mpi-openmp

mpi-pthreads: mpi-pthreads.c utils.h
	mpicc mpi-pthreads.c -o mpi-pthreads -lpthread -lm

mpi_run:
	mpirun -np 6 ./mpi

mpi-openmp_run:
	mpirun -np 3 ./mpi-openmp

mpi-pthreads_run:
	mpirun -np 3 ./mpi-pthreads

clean:
	rm serial openmp pthreads mpi mpi-openmp mpi-pthreads
