CC = clang++
CFLAGS = -g -DDEBUG
LINK = -lfl
FLEX = flex
BISON = bison
LEXER = smallc.l
PARSER = smallc.y
OBJECTS = smallc.tab.o lex.yy.o ast.o astcodegen.o astdeclare.o astdump.o astsemant.o decl.o env.o irdump.o labels.o main.o ir.o

all: smallc tags

clean:
	rm -f lex.yy.cc smallc.tab.cc smallc.tab.h *.o smallc smallc.output *.stackdump *.d tags smallc.exe

smallc: $(OBJECTS)
	$(CC) $^ -o smallc $(CFLAGS) $(LINK)

smallc.tab.cc: smallc.y
	$(BISON) -d -r all smallc.y
	mv smallc.tab.c smallc.tab.cc

lex.yy.cc: smallc.l smallc.tab.cc
	$(FLEX) smallc.l
	mv lex.yy.c lex.yy.cc

%.o: %.cc
	$(CC) -c $(CFLAGS) $<
	$(CC) -MM $(CFLAGS) $< > $*.d

tags: $(wildcard *.cc *.h)
	ctags *.cc *.h

-include *.d

