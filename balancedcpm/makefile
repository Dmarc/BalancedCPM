INSTALL_ROOT=~/cash

#FLAG = -Wall -Wextra -pedantic -Wpointer-arith -Wcast-qual -Wcast-align -Wdouble-promotion -Wfloat-equal -Wshadow -Wconversion -pg
#FLAG = -Wall -Wextra -pedantic -Wpointer-arith -Wcast-qual -Wcast-align -Wdouble-promotion -Wfloat-equal -Wshadow -pg
FLAG = -O3 -s
#FLAG = -g
DATA = -std=c99 -msse2 -DHAVE_SSE2 -DSFMT_MEXP=2281 -fopenmp

potts: potts.o allocate.o operation.o bubble.o SFMT.o
	gcc potts.o allocate.o operation.o SFMT.o bubble.o -o potts -lcash -lpng -lz -lX11 -lm -L$(INSTALL_ROOT)/lib -I$(INSTALL_ROOT)/include -L/usr/X11R6/lib $(FLAG) $(DATA)

potts.o: potts.c potts.h
	gcc potts.c -c $(FLAG) $(DATA)

allocate.o: allocate.c allocate.h
	gcc allocate.c -c $(FLAG) $(DATA)

operation.o: operation.c operation.h
	gcc operation.c -c $(FLAG) $(DATA)

bubble.o: bubble.c bubble.h
	gcc bubble.c -c $(FLAG) $(DATA)

SFMT.o: SFMT.c SFMT.h SFMT-params2281.h
	gcc -c SFMT.c -O3 -msse2 $(DATA) -fno-strict-aliasing -s

clean:
	rm *.o
