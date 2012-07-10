CC = g++
CFLAGS = -g -DDEBUG
LINK = -lfl
FLEX = flex
BISON = bison

all: smallc

clean:
	rm -f lex.yy.c smallc.tab.c smallc.tab.h *.o smallc smallc.output *.stackdump

smallc: lex.yy.o smallc.tab.o main.o astdump.o astsemant.o env.o
	$(CC) $^ -o smallc $(CFLAGS) $(LINK)

smallc.tab.c: smallc.y
	$(BISON) -d -r all smallc.y

lex.yy.c: smallc.l smallc.tab.c
	$(FLEX) smallc.l

%.o: %.cc
	$(CC) -c $(CFLAGS) $<

%.o: %.c
	$(CC) -c $(CFLAGS) $<

