etapa3: y.tab.o lex.yy.o main.o hash.o astree.o
	gcc -o etapa4 y.tab.o lex.yy.o main.o hash.o astree.o
main.o: main.c lex.yy.c
	gcc -c main.c
astree.o:
	gcc -c astree.c
hash.o: hash.c
	gcc -c hash.c
lex.yy.o: lex.yy.c
	gcc -c lex.yy.c
lex.yy.c: scanner.l
	lex --header-file=lex.yy.h scanner.l
y.tab.o: y.tab.c
	gcc -c y.tab.c
y.tab.c: y.tab.h
	yacc --debug parser.y
y.tab.h: parser.y
	yacc --debug -d parser.y
	
tgz: clean
	tar cvzf etapa4.tgz .	
clean:
	rm -rf *.o lex.yy.* y.tab.* y.output etapa4 etapa4.tgz
