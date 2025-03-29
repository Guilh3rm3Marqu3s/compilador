#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<stdarg.h>
#include<string.h>
#include<ctype.h>
#include "util.h"
#include "tabela_simbolos.h"
#include "lista_entrada.h"
#ifdef _WIN32
    //estiver usando windows
    #include <windows.h>
#else
    //LINUX/macOS
    #include <unistd.h>
#endif





Token simbolo;
int contador_linha=1;
char proximo=' ';
char atomo[100]="";
int contador_variavel=1;
FILE *arquivo=NULL;
FILE *saida=NULL;
bool verificar_parametro_inicializacao(int argc); //verifica se algum parametro foi passado
bool arquivo_existe(char *nome_arquivo); //verifica se o arquivo existe
bool simbolo_especial(char c); //verifica se é simbolo especial
bool letra(char c);//verifica se é uma letra
bool digito(char c);//verifica se é um dígito
bool letra_ou_digito(char c);//verifica se é letra ou dígito
bool palavra_reservada(char s[]);//verifica se é uma palavra reservada
void PROXIMO(void);
void ERRO(char msg[]);
void escreve_saida(void);

Token CODIGO(char s[], int tipo);

void ANALISADOR_LEXICO(void);

int main(int argc, char *argv[]){
  
  if (!verificar_parametro_inicializacao(argc))return EXIT_FAILURE;
  if (!arquivo_existe(argv[1]))return EXIT_FAILURE;
  iniciar_tabela();
  inicia_lista_entrada();
  arquivo=fopen(argv[1],"r"); //abrir o arquivo
  //saida=fopen("saida.txt","w");
  PROXIMO(); //inicializar o primeiro caractere
  while (proximo !=EOF)
  {
    
    ANALISADOR_LEXICO();
    //printf("Token: '%s' --> Valor: '%s'\n",simbolo.codigo,simbolo.valor);
  }

  printf("=================================================================\n");
  Node* aux;
  for(int i=0;i<MAX;i++){
    
    if(tabela_simbolos[i]!=NULL){
        aux=tabela_simbolos[i];
        while (aux!=NULL)
        {
            printf("Token: '%s' --> Valor: '%s' \n",aux->token.codigo,aux->token.valor);
            aux=aux->prox;
        }
      
        escreve_saida();
        
    }
  }
  
  //fclose(saida);
}

bool arquivo_existe(char *nome_arquivo){
    #ifdef _WIN32
    DWORD atributos = GetFileAttributes(nome_arquivo);
    printf("O arquivo %s nao foi encontrado.\nVerifique o diretorio",nome_arquivo);
    if(atributos != INVALID_FILE_ATTRIBUTES && !(atributos & FILE_ATTRIBUTE_DIRECTORY)){
        return true;
    }else{
        printf("O arquivo %s nao foi encontrado.\nVerifique o diretorio",nome_arquivo);
        return false;
    }

    #else
   
    if (access(nome_arquivo, F_OK) != 0){
        printf("O arquivo %s nao foi encontrado.\nVerifique o diretorio",nome_arquivo);
        return false;
    }else{
        return true;
    }
    #endif
}
bool verificar_parametro_inicializacao(int argc){
    if (argc<2){
        printf("Erro: nenhum arquivo foi passado!!\n"); 
        return false;
    }
    else return true;
}

bool simbolo_especial(char c){
    return strchr(":=+-*/(),;<>",c)!=NULL;
}
bool letra(char c){
    return isalpha(c);
}
bool digito(char c){
    return isdigit(c);
}
bool letra_ou_digito(char c){
    return (letra(c) || digito(c));
}
bool palavra_reservada(char s[]){
    const char *palavras_reservadas[]={"begin","end","if","then","else","while","integer","do","var","procedure","function","program",NULL};

    for(int i=0;palavras_reservadas[i]!=NULL;++i){
        if(strcmp(s,palavras_reservadas[i])==0)return true;
    }
    return false;
}


void ANALISADOR_LEXICO(){
    strcpy(atomo,""); //iniciando a cadeia vazia
    
    while (proximo==' ' || proximo=='\t' || proximo=='\n'){
        
        if(proximo=='\n'){
            strcpy(simbolo.codigo,"\n");
            strcpy(simbolo.valor,"\n");
            simbolo.tipo=TOKEN_NOVA_LINHA;
            contador_linha++;
            
        }else if(proximo==' '){
            strcpy(simbolo.codigo," ");
            strcpy(simbolo.valor," ");
            simbolo.tipo=TOKEN_ESPACO;
        }else{
            strcpy(simbolo.codigo,"\t");
            strcpy(simbolo.valor,"\t");
            simbolo.tipo=TOKEN_TAB;
        }
        adicionar_na_lista(simbolo);
        //fprintf(saida,"%c",proximo);
        PROXIMO();
    }
   
    if(simbolo_especial(proximo)){
        char s[3]={proximo,'\0','\0'}; //concatenar
        PROXIMO();

        if(strcmp(s,":")==0 && proximo=='='){
            strcpy(s,":=");
            PROXIMO();
        }
        
        simbolo=CODIGO(s,0);
        adicionar_na_lista(simbolo);
        //fprintf(saida,"%s",simbolo.codigo);
        

    }else if(letra(proximo)){
        do{
            strncat(atomo,&proximo,1);
            PROXIMO();
        }while(letra_ou_digito(proximo));

        if(palavra_reservada(atomo)){
            simbolo=CODIGO(atomo,0);
            adicionar_na_lista(simbolo);
            //fprintf(saida,"%s",simbolo.codigo);
        }else{
            Token* aux=pega_na_tabela(atomo);
            if(aux==NULL){
                //gravar na tabela
                simbolo=CODIGO(atomo,1);
                adicionar_na_tabela(simbolo);
                adicionar_na_lista(simbolo);
                //fprintf(saida,"%s",simbolo.codigo);

            }else{
                adicionar_na_lista(*aux);
                //fprintf(saida,"%s",aux->codigo);
            }

        }
    }else if(digito(proximo)){
        do{
            strncat(atomo,&proximo,1);
            PROXIMO();
        }while(digito(proximo));

        if(letra(proximo)){
            ERRO("variavel invalida.");
        }
        simbolo=CODIGO(atomo,2);
        adicionar_na_lista(simbolo);
        //fprintf(saida,"%s",simbolo.codigo);

    }else if(proximo==EOF){
        simbolo.tipo=TOKEN_EOF;
        stpcpy(simbolo.valor,"EOF");
        stpcpy(simbolo.codigo,"EOF");
        //adicionar_na_lista(simbolo);
    }else{
        char msg[100]="Caractere '";
        char str_proximo[2];
        str_proximo[0]=proximo;
        str_proximo[1]='\0';
        strcat(msg,str_proximo);
        strcat(msg,"'eh invalido.");
        ERRO(msg);
    }
    
    





}
Token CODIGO(char s[],int tipo){
    Token t;
    strcpy(t.valor,s);
    if(tipo==0){//simbolos
        if(strcmp(s,":=")==0){
            t.tipo=TOKEN_ATRIBUICAO;
        }
        else if(strcmp(s,":")==0){
            t.tipo=TOKEN_DOIS_PONTOS;
        }
        else if (strcmp(s,"=")==0){
            t.tipo=TOKEN_IGUAL;
        }
        else if(strcmp(s,"begin")==0){
            t.tipo=TOKEN_BEGIN;
        }
        else if(strcmp(s,"end")==0){
            t.tipo=TOKEN_END;
        }
        else if(strcmp(s,"program")==0){
            t.tipo=TOKEN_PROGRAM;
        }else if(strcmp(s,"var")==0){
            t.tipo=TOKEN_VAR;
        }else if (strcmp(s,"integer")==0){
            t.tipo=TOKEN_INTEGER;
        }else if(strcmp(s,"procedure")==0){
            t.tipo=TOKEN_PROCEDURE;
        }else if(strcmp(s,"+")==0){
            t.tipo=TOKEN_MAIS;
        }else if(strcmp(s,"-")==0){
            t.tipo=TOKEN_MENOS;
        }else if(strcmp(s,"*")==0){
            t.tipo=TOKEN_MULT;
        }else if(strcmp(s,">")==0){
            t.tipo=TOKEN_MAIOR;
        }else if(strcmp(s,"<")==0){
            t.tipo=TOKEN_MENOR;
        }else if(strcmp(s,">=")==0){
            t.tipo=TOKEN_MAIOR_IGUAL;
        }else if(strcmp(s,"<=")==0){
            t.tipo=TOKEN_MENOR_IGUAL;
        }else if(strcmp(s,"<>")==0){
            t.tipo=TOKEN_DIFERENTE;
        }else if(strcmp(s,";")==0){
            t.tipo=TOKEN_PONTO_VIRGULA;
        }
        
        
        strcpy(t.codigo,s);

    }
    else if(tipo==1){//for identificador
        t.tipo=TOKEN_IDENTIFICADOR;

        char str[20]; //armazenará o número como string

        sprintf(str,"%d",contador_variavel++);
        char codigo[30]="id";
        strcat(codigo,str);
        strcpy(t.codigo,codigo);
        
    }
    else if(tipo==2){//se for numero
        t.tipo=TOKEN_NUMERO;
        strcpy(t.codigo,s);
    }

    return t;
}
void PROXIMO(){
  
    proximo=fgetc(arquivo); //pegar o proximo caractere

    if (feof(arquivo)){//final do arquivo
        fclose(arquivo);
        arquivo=NULL;
        proximo=EOF;
    }
}

void ERRO(char msg[]){
    fprintf(stderr,"Erro na linha %d: %s\n",contador_linha,msg);
    exit(EXIT_FAILURE);
}

void escreve_saida(){
    
    if(lista_entrada!=NULL){
        FILE* saida=fopen("saida.txt","w");
        Lista_Entrada* aux=lista_entrada;
        while (aux!=NULL)
        {
            fprintf(saida,"%s",aux->token.codigo);
            aux=aux->prox;
        }
        fclose(saida);
    }

}