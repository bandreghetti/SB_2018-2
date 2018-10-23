MATH: BEGIN

SECTION TEXT
    N1: EXTERN
    N2: EXTERN

    GAUSS_SUM: EXTERN
    FAT: EXTERN

    ONE: EXTERN

    PUBLIC AUX
    PUBLIC MULT_N1
    PUBLIC ADD_N2

MULT_N1:
    LOAD AUX        ; Carrega valor acumulado
    MULT N1          ; Multiplica por N1
    STORE AUX       ; e salva o resultado

    LOAD N1         ; Subtrai 1 de N1
    SUB ONE         ;
    STORE N1        ; e salva novo N1
    JMP FAT         ; realiza o fatorial de N1-1

ADD_N2:
    LOAD AUX        ; Carrega valor acumulado
    ADD N2          ; Soma N2
    STORE AUX       ; e salva o resultado

    LOAD N2         ; Subtrai 1 de N2
    SUB ONE         ;
    STORE N2        ; e salva novo N2
    JMP GAUSS_SUM   ; realiza o soma de N2-1

SECTION BSS
    AUX: SPACE

END
