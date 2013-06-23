build:
	flex tema.l
	g++ parsing.cpp -c 
	g++ lex.yy.c -c -lfl
	g++ -o mtx lex.yy.o parsing.o -lfl -g

run:
	./mtx teste/test/simple_bonus.mt Copy "#>#abba#"

clean:
	rm a.out lex.yy.c
