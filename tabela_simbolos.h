#ifndef TABELA_SIMBOLOS_H
#define TABELA_SIMBOLOS_H
#define MAX 37
#include "util.h"
#include<stdlib.h>

typedef struct TabelaSimbolos{
    Token token;
    struct TabelaSimbolos *prox;
}Node;

Node* tabela_simbolos[MAX];
int hashing(char token[]);//recebe o token e retorna uma posição para procurar na tabela
void iniciar_tabela(void);
void adicionar_na_tabela(Token elemento);
Token* pega_na_tabela(char s[]);
int soma_caracteres(char s[]);
#endif