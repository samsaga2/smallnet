CC = g++
CFLAGS = -g -DDEBUG
LINK = -lfl
FLEX = flex
BISON = bison

all: smallc

clean:
	rm -f lex.yy.c smallc.tab.c smallc.tab.h *.o smallc smallc.output *.stackdump *.d

smallc: lex.yy.o smallc.tab.o main.o astdump.o astsemant.o env.o irdump.o ast.o astcodegen.o astdeclare.o decl.o
	$(CC) $^ -o smallc $(CFLAGS) $(LINK)

smallc.tab.c: smallc.y
	$(BISON) -d -r all smallc.y

lex.yy.c: smallc.l smallc.tab.c
	$(FLEX) smallc.l

%.o: %.cc
	$(CC) -c $(CFLAGS) $<
	$(CC) -MM $(CFLAGS) $< > $*.d

%.o: %.c
	$(CC) -c $(CFLAGS) $<
	$(CC) -MM $(CFLAGS) $< > $*.d

-include *.d

