
CC=mpicc
CFLAGS= -Wall  -pg  -lm
DEPS =
OBJ = rotate_mpi.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

rotate_image: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -f *.o rotate_mpi