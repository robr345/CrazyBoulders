.PHONY: all clean

LIBRARIES := -L ./lib
INCLUDE := -I ./include

all: CRAZYBOULDERS.PRG

CRAZYBOULDERS.PRG: main.c game.c
	cl65 -t cx16 -O -o CRAZYBOULDERS.PRG $(INCLUDE) $(LIBRARIES) main.c game.c libX16.lib cx16.lib

clean:
	rm -r *.PRG
