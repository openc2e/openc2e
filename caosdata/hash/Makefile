CFLAGS = -O

all : perfect generatehex

.cc.o:
	gcc $(CFLAGS) -c $<

O = lookupa.o recycle.o perfhex.o perfect.o

perfect : $(O)
	gcc -o perfect $(O) -lm

generatehex : main.o
	gcc -o generatehex main.o

# DEPENDENCIES

main.o : main.c

lookupa.o : lookupa.c standard.h lookupa.h

recycle.o : recycle.c standard.h recycle.h

perfhex.o : perfhex.c standard.h lookupa.h recycle.h perfect.h

perfect.o : perfect.c standard.h lookupa.h recycle.h perfect.h
