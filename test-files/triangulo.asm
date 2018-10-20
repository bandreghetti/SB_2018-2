; Programa que calcula a área de um triângulo
	TRIANGULO: EQU 1 ; teste comentário

SECTION TEXT
Main:
Func:	EXTERN
		INPUT		B + 1
		INPUT		H
		LOAD		B + 1
		MULT		H
		IF TRIANGULO
		DIV		DOIS
		STORE		R
		OUTPUT	R
		COPY B H
		COPY B, H
		COPY B + 1, H
		COPY B, H + 1
		COPY B + 1, H + 1
		JMP Func
		STOP
SECTION BSS
	B:		SPACE		2
	H:		SPACE
	R:		SPACE
	SECTION DATA
	DOIS:	CONST		2
