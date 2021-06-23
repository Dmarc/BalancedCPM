SHELL = /bin/sh

tar:
	make clean
	cd lib ; make clean
	cd examples ; make clean
	cd examples ; rm -rf Frames
	d=`basename \`pwd\`` ;\
	cd ..; rm -f $$d.tar* ;\
	tar cvf $$d.tar $$d 
