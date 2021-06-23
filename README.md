# BalancedCPM
Cellular Potts Model (CPM) implementation that satisfies detailed balance and prevent cell fragmentation.

See M. Durand and E. Guesnet, “An efficient Cellular Potts Model algorithm that forbids cell fragmentation”, *Computer Physics Communications* **208**, 54-63 (2016).

##How to install:


Go into the library directory:      cd lib

Make the library:                   make all

Read the install entry in the makefile to set the include and lib directory where you want to install cashpotts.

Then:
To install libcash.a and cash.h:    make install


Then:

make potts

Some extra libraries, like x11-devel and png-devel, can be required.

##How to run:

./potts param dest_folder
