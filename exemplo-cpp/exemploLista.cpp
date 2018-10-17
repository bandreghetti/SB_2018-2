#include <iostream>
#include <list>
using namespace std;    //Define o contexto das bibliotecas do programa.
                        //std é a biblioteca padrão, que contém as funções de lista

typedef struct pessoa {
    char nome [30];
    int idade;
} TPessoa;

int main(){
    //Declaração
    list<int> emptyList; //constrói uma lista de inteiros vazia
    list<int> intList (4,100); //constrói uma lista de inteiros com o elemento 100 4 vezes: [100,100,100,100]
    list<TPessoa> pessoaList; //constrói uma lista de pessoas vazia

    //Operações
    intList.push_front(0); //adiciona o elemento 0 no inicio da lista
    intList.push_back(1); //adiciona o elemento 1 no fim da lista
    intList.pop_front(); //deleta o ultimo elemento da lista
    intList.pop_back(); //deleta o primeiro elemento da lista
    intList.clear(); //apaga todos os elementos da lista
    intList.push_front(15);
    intList.push_front(17);
    intList.push_front(12);
    intList.push_front(5);
    intList.remove(15); //remove todos os elementos com valor 15 da lista
    intList.sort(); //ordena a lista
    intList.front(); //acessa o primeiro elemento da lista
    intList.back(); //acessa o último elemento da lista

    //Iterador - variável usada pra varrer a lista
    list<int>::iterator iterador;

    //list.begin() retorna um iterador no inicio da lista
    //list.end() retorna um iterador no fim da lista
    //Itera incrementando ou decrementando o iterador (++ ou --)
    //Acessa o valor do elemento onde o iterador está com *, o iterador é um ponteiro para um elemento.
    printf("Minha lista contém: ");
    for(iterador = intList.begin(); iterador!= intList.end(); ++iterador){
        printf("%d ", *iterador);
    }
}