#ifndef LISTA_ENTRADA_H
#define LISTA_ENTRADA_H

#include "util.h"
#include<stdlib.h>

typedef struct ListaEntrada{
    Token token;
    struct ListaEntrada *prox;
}Lista_Entrada;


Lista_Entrada* lista_entrada;
void inicia_lista_entrada(void);
void adicionar_na_lista(Token elemento);
Lista_Entrada* pega_lista(void);
#endif