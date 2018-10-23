FAT: BEGIN

SECTION TEXT
    ; Retorna para Main
    RET_FAT: EXTERN
    
    ; Funcoes e variaveis da math
    MULT_N1: EXTERN
    AUX: EXTERN
    
    ; variaveis de classe
    ZERO: EXTERN
    ONE: EXTERN
    IS_INITIALIZED: EXTERN
    
    ; Funcoes e variaveis publicas da fat
    PUBLIC FAT
    PUBLIC N1
    
    CHECK:                          ; Checa se é o inicio da função
    LOAD IS_INITIALIZED             ; Se
    JMPP MAKE_FAT                   ; ja foi inicializado realiza o fat
    COPY ONE IS_INITIALIZED         ; se nao muda para inicializado
    COPY ONE AUX                    ; e garante que aux comece com valor 1
    
    MAKE_FAT:                       ; Realiza o fatoria:
    LOAD N1                         ; Se N1 for > 0
    JMPP MULT_N1                    ; Multiplica
    JMP RET                         ; Se não retorna valor acumulado
    
    RET:                            ; 
    LOAD AUX                        ; Carrega o auxiliar
    COPY ZERO IS_INITIALIZED        ; muda variavel de inicialização para não inicializado
    JMP RET_FAT                     ; e retorna valor acumulado no auxiliar
    
SECTION BSS
    N1: SPACE
    
END
