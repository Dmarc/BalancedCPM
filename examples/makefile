INSTALL_ROOT=/usr/local
INSTALL_ROOT=..

.c:
	cc -O -o $@ $< -lcash -lpng -lz -lX11 -lm -L$(INSTALL_ROOT)/lib -I$(INSTALL_ROOT)/include -L/usr/X11R6/lib

clean:
	rm -f `ls -1 *.c | sed -e 's/.c$$//'`
	rm -f color.cmap color.ps array.ps
