#include "lista_entrada.h"


void inicia_lista_entrada(){
    while (lista_entrada != NULL) {
        Lista_Entrada* temp = lista_entrada;
        lista_entrada = lista_entrada->prox;
        free(temp);
    }
    lista_entrada = NULL;

}

void adicionar_na_lista(Token elemento){
   
    Lista_Entrada* novoNo=(Lista_Entrada*)malloc(sizeof(Lista_Entrada));
    novoNo->token=(elemento);
   
    if(lista_entrada==NULL){
        //não há nada
        
        novoNo->prox=NULL;
        lista_entrada=novoNo;
       
    }
    else{
        //tem elementos
        if(lista_entrada->prox==NULL){
            //tem apenas 1 elemento
            novoNo->prox=NULL;
            lista_entrada->prox=novoNo;
           
        }else{
            //tem mais elementos
            Lista_Entrada* aux=lista_entrada;
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


Lista_Entrada* pega_lista(){
    return lista_entrada;
}