#{CC}=gcc
OBJS=ipmessage.o
OBJS+=util.o
OBJS+=receiver.o
OBJS+=messager.o
OBJS+=user.o
OBJS+=console.o
OBJS+=functions.o
OBJS+=sendfile.o
OBJS+=filelist.o
OBJS+=filesender.o
OBJS+=filereceiver.o

OPTS=-g -rdynamic

MESSAGE=messager
GTK_FLAGS=`pkg-config --cflags gtk+-2.0` `pkg-config --libs gtk+-2.0`

#`pkg-config --cflags --libs glib-2.0`

all:${MESSAGE}

%.o:%.c %.h
	${CC} ${OPTS} -c ${GTK_FLAGS} $< -o $@

${MESSAGE}:${OBJS}
	${CC} -lpthread ${GTK_FLAGS} -o ${MESSAGE} ${OBJS}

clean:
	rm -f *.o
	rm -f *~
	rm -f ${MESSAGE}

