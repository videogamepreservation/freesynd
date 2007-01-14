OPTS=-g

CFLAGS = -I/usr/include/lua5.1/ -I/usr/include/SDL/ $(OPTS)
LDFLAGS= -llua5.1 -lSDL -lSDL_mixer

app: app.o chronos.o hugin.o munin.o narcissus.o siren.o fliplayer.o gaia.o
app: avatar.o nyarlathotep.o uenuku.o

clean:
	$(RM) *.o app *~
