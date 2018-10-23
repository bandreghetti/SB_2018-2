GAUSS_SUM: BEGIN

SECTION TEXT
    ; Retorna para Main
    RET_GAUSS: EXTERN

    ; Funcoes e variaveis da math
    ADD_N2: EXTERN
    AUX: EXTERN
    
    ; variaveis de classe
    ZERO: EXTERN
    ONE: EXTERN
    IS_INITIALIZED: EXTERN
    
    ; Funcoes e variaveis publicas da gauss_sum
    PUBLIC GAUSS_SUM
    PUBLIC N2
    
    CHECK:                          ; Checa se é o inicio da função
    LOAD IS_INITIALIZED             ; Se
    JMPP MAKE_SUM                   ; ja foi inicializado realiza a soma
    COPY ONE IS_INITIALIZED         ; se nao muda para inicializado
    COPY ZERO AUX                    ; e garante que aux comece com valor 1
    
    MAKE_SUM:                       ; Realiza o fatoria:
    LOAD N2                         ; Se N2 for > 0
    JMPP ADD_N2                     ; Soma
    JMP RET                         ; Se não retorna valor acumulado
    
    RET:                            ; 
    LOAD AUX                        ; Carrega o auxiliar
    COPY ZERO IS_INITIALIZED        ; muda variavel de inicialização para não inicializado
    JMP RET_GAUSS                     ; e retorna valor acumulado no auxiliar
    
SECTION BSS
    N2: SPACE
    
END
