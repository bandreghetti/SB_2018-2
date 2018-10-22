Aluno: Bruno Andreghetti Dantas                 Matrícula: 15/0007060

Aluna: Sara Gomes Cardoso                       Matrícula: 15/0021712

---------------------------------------------------------------------

## Comandos Gerais

* Para compilar os programas de tradução 'montador.cpp' e 'ligador.cpp':

`$ ./compileProject.sh`

## Montador

* Para gerar o programa pré-processado (.pre) e o arquivo objeto (.obj)
a partir de um arquivo de entrada em Assembly de linguagem inventada
(.asm): 

```
$ ./montador.out <arquivo>

```
Para os arquivos testes fornecidos:
```
$ ./montador.out test-files/<arquivo>

```
* Na existência de erros durante a montagem, serão emitidas mensagens para o usuário indicando
a linha e o conteúdo do erro.

## Ligador

* Para gerar o arquivo executável (.e) a partir dos arquivos objetos (.obj)
de entrada, que podem variar entre 1 e 4:

```
$ ./ligador.out <arquivo1> <arquivo2> <arquivo3> <arquivo4>

```
Para os arquivos testes fornecidos:
```
$ ./ligador.out test-files/<arquivo1> test-files/<arquivo2> test-files/<arquivo3> test-files/<arquivo4>

```
