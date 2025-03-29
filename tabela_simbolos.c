#include "tabela_simbolos.h"
#include<string.h>
#include<stdbool.h>
#include<stdio.h>

void iniciar_tabela(){
    //iterar e definir como null cada posicao
   
    for(int i=0;i<MAX;++i){
        tabela_simbolos[i]=NULL;
    }
}

void adicionar_na_tabela(Token elemento){
   
    Node* novoNo=(Node*)malloc(sizeof(Node));
    novoNo->token=(elemento);
    int posicao=hashing(elemento.valor);
   
    if(tabela_simbolos[posicao]==NULL){
        //não há nada
        
        novoNo->prox=NULL;
        tabela_simbolos[posicao]=novoNo;
       
    }
    else{
        //tem elementos
        if(tabela_simbolos[posicao]->prox==NULL){
            //tem apenas 1 elemento
            novoNo->prox=NULL;
            tabela_simbolos[posicao]->prox=novoNo;
           
        }else{
            //tem mais elementos
            Node* aux=tabela_simbolos[posicao];
            while (aux->prox!=NULL)//até chegar no fim da lista
            {
                
                aux=aux->prox;
            }
            //chegou ao final
            novoNo->prox=NULL;
            aux->prox=novoNo;
            
            

        }
    }
}
int soma_caracteres(char s[]){
    
    int soma=0;
    while(*s){
        soma+=*s;
        s++;
    }
    return soma;
}
int hashing(char token[]){
    int soma=soma_caracteres(token);
    int posicao=(soma%MAX);
    return posicao;
}
Token* pega_na_tabela(char s[]){
   
    int posicao=hashing(s);
    Node* aux=tabela_simbolos[posicao];
    if(aux==NULL)return NULL;
    else if(aux->prox==NULL){
        if(strcmp(aux->token.valor,s)==0){
            return &aux->token;
        }else{
            return NULL;
        }
    }else{

    
        while (aux!=NULL)
        {
            if(strcmp(aux->token.valor,s)==0){
                return &aux->token;
            }
            aux=aux->prox;
        }
        return NULL;

    }

    
    

}