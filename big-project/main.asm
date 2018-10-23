MAIN: BEGIN

SECTION TEXT
    FAT: EXTERN
    GAUSS_SUM: EXTERN
    N1: EXTERN
    N2: EXTERN
    IS_INITIALIZED: EXTERN
    PUBLIC RET_GAUSS
    PUBLIC RET_FAT
    
    INPUT N
    
    COPY N N1       ; 
    JMP FAT         ; realiza Fatorial de N1
    RET_FAT:        ; volta do fatorial
    STORE ACC       ; salva na variavel o valor do fatorial
    OUTPUT ACC      ; e exibe
    
    COPY N N2       ; Copia N para N2
    JMP GAUSS_SUM   ; realiza a soma de gauss
    RET_GAUSS:      ; volta da soma
    STORE ACC       ; salva na variavel o valor da soma
    OUTPUT ACC      ; e exibe
    
    STOP            ; Termina o programa
    
SECTION BSS
    ACC: SPACE
    N: SPACE
    
END
