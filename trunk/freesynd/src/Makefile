OPTS=-g

CFLAGS = -I/usr/include/lua5.1/ -I/usr/include/SDL/ $(OPTS)
LDFLAGS= -llua5.1 -lSDL -lSDL_mixer

tiamat: tiamat.o chronos.o hugin.o munin.o narcissus.o siren.o scheherazade.o
tiamat: gaia.o avatar.o nyarlathotep.o uenuku.o


clean:
	$(RM) *.o app *~

lt: lt.o hugin.o

*.o: common.h