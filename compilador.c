/*
Guilherme Negrini Marques
RA: 231020104
SO: macOS
IDE: VsCode
OBS: o código funciona para qualquer SO (seja mac,Windows ou Linux)
*/
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




Token token_atual;
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
void ERRO(char msg[],int linha);
void escreve_saida(void);
char* transforma_maiuscula(char str[]);
Token CODIGO(char s[], int tipo);

void ANALISADOR_LEXICO(void);

//Protótipos analisador sintático
void ANALISADOR_SINTATICO(void);
void programa(void);
void consume(void);
void bloco(void);
void parte_declaracoes_variaveis(void);
void parte_declaracoes_sub_rotinas(void);
void comando_composto(void);
void declaracao_variaveis(void);
void lista_identificadores(void);
void declaracao_procedimento(void);
void declaracao_funcao(void);
void parametros_formais(void);
void secao_parametros_formais(void);
void comando_sem_rotulo(void);
void comando(void);
void atribuicao(void);
void chamada_procedimento(void);
void comando_condicional(void);
void comando_repetitivo(void);
void lista_expressoes(void);
void expressao(void);
void relacao(void);
void expressao_simples(void);
void termo(void);
void fator(void);
void variavel(void);
void chamada_funcao(void);
void numero(void);
void identificador();
Token look_ahead();

int main(int argc, char *argv[]){
  
  if (!verificar_parametro_inicializacao(argc))return EXIT_FAILURE;
  if (!arquivo_existe(argv[1]))return EXIT_FAILURE;
  iniciar_tabela();
  inicia_lista_entrada();
  arquivo=fopen(argv[1],"r"); //abrir o arquivo
  PROXIMO(); //inicializar o primeiro caractere
  while (proximo !=EOF)
  {
    
    ANALISADOR_LEXICO();
    
  }
  lista_entrada=pega_lista();
  token_atual=lista_entrada->token;
  contador_linha=1;
  ANALISADOR_SINTATICO();
  printf("Análise sintática concluída com sucesso!\n");

  fclose(arquivo);
}

bool arquivo_existe(char *nome_arquivo){
    #ifdef _WIN32
    DWORD atributos = GetFileAttributes(nome_arquivo);
    
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
    return strchr(":=+-*/(),;<>.",c)!=NULL;
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
    const char *palavras_reservadas[]={"BEGIN","END","IF","THEN","ELSE","WHILE","INTEGER","DO","VAR","PROCEDURE","DIV","FUNCTION","PROGRAM","BOOLEAN",NULL};

    for(int i=0;palavras_reservadas[i]!=NULL;++i){
        if(strcmp(s,palavras_reservadas[i])==0)return true;
    }
    return false;
}


void ANALISADOR_LEXICO(){
    strcpy(atomo,""); //iniciando a cadeia vazia
    
    while (proximo==' ' || proximo=='\t' || proximo=='\n'){
        
        if(proximo=='\n'){
            //strcpy(simbolo.codigo,"\n");
            //strcpy(simbolo.lexema,"\n");
            //simbolo.tipo=TOKEN_NOVA_LINHA;
            contador_linha++;
        }
        /*
        else if(proximo==' '){
            strcpy(simbolo.codigo," ");
            strcpy(simbolo.lexema," ");
            simbolo.tipo=TOKEN_ESPACO;
        }else{
            strcpy(simbolo.codigo,"\t");
            strcpy(simbolo.lexema,"\t");
            simbolo.tipo=TOKEN_TAB;
        }
        */
        //adicionar_na_lista(simbolo);
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
        simbolo.linha=contador_linha;
        adicionar_na_lista(simbolo);
        //fprintf(saida,"%s",simbolo.codigo);
        

    }else if(letra(proximo)){
        do{
            strncat(atomo,&proximo,1);
            PROXIMO();
        }while(letra_ou_digito(proximo));

        if(palavra_reservada(transforma_maiuscula(atomo))){
            simbolo=CODIGO(atomo,0);
            simbolo.linha=contador_linha;
            adicionar_na_lista(simbolo);
            //fprintf(saida,"%s",simbolo.codigo);
        }else{
            Token* aux=pega_na_tabela(transforma_maiuscula(atomo));
            if(aux==NULL){
                //gravar na tabela
                simbolo=CODIGO(transforma_maiuscula(atomo),1);
                simbolo.linha=contador_linha;
                adicionar_na_tabela(simbolo);
                adicionar_na_lista(simbolo);
                

            }else{
                adicionar_na_lista(*aux);
            }

        }
    }else if(digito(proximo)){
        do{
            strncat(atomo,&proximo,1);
            PROXIMO();
        }while(digito(proximo));

        if(letra(proximo)){
            ERRO("variavel invalida.", contador_linha);
        }
        simbolo=CODIGO(atomo,2);
        simbolo.linha=contador_linha;
        adicionar_na_lista(simbolo);
        

    }else if(proximo==EOF){
        simbolo.tipo=TOKEN_EOF;
        strcpy(simbolo.lexema,"EOF");
        strcpy(simbolo.codigo,"EOF");
       
    }else{
        char msg[100]="Caractere '";
        char str_proximo[2];
        str_proximo[0]=proximo;
        str_proximo[1]='\0';
        strcat(msg,str_proximo);
        strcat(msg,"'eh invalido.");
        ERRO(msg, contador_linha);
    }
    
    





}
Token CODIGO(char s[],int tipo){
    Token t;
    strcpy(t.lexema,s);
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
        else if(strcmp(s,"BEGIN")==0){
            t.tipo=TOKEN_BEGIN;
        }
        else if(strcmp(s,"END")==0){
            t.tipo=TOKEN_END;
        }
        else if(strcmp(s,"PROGRAM")==0){
            t.tipo=TOKEN_PROGRAM;
        }else if(strcmp(s,"VAR")==0){
            t.tipo=TOKEN_VAR;
        }else if (strcmp(s,"INTEGER")==0){
            t.tipo=TOKEN_INTEGER;
        }else if(strcmp(s,"BOOLEAN")==0){
            t.tipo=TOKEN_BOOLEAN;
        }
        else if(strcmp(s,"PROCEDURE")==0){
            t.tipo=TOKEN_PROCEDURE;
        }else if(strcmp(s,"+")==0){
            t.tipo=TOKEN_MAIS;
        }else if(strcmp(s,"-")==0){
            t.tipo=TOKEN_MENOS;
        }else if(strcmp(s,"*")==0){
            t.tipo=TOKEN_MULT;
        }else if(strcmp(s,">")==0){
            t.tipo=TOKEN_MAIOR;
        }else if(strcmp(s,".")==0){
            t.tipo=TOKEN_PONTO;
        }
        else if(strcmp(s,"<")==0){
            t.tipo=TOKEN_MENOR;
        }else if(strcmp(s,">=")==0){
            t.tipo=TOKEN_MAIOR_IGUAL;
        }else if(strcmp(s,"<=")==0){
            t.tipo=TOKEN_MENOR_IGUAL;
        }else if(strcmp(s,"<>")==0){
            t.tipo=TOKEN_DIFERENTE;
        }else if(strcmp(s,";")==0){
            t.tipo=TOKEN_PONTO_VIRGULA;
        }else if (strcmp(s,",")==0){
            t.tipo=TOKEN_VIRGULA;
        }else if(strcmp(s,"(")==0){
            t.tipo=TOKEN_ABRE_PAR;
        }else if(strcmp(s,")")==0){
            t.tipo=TOKEN_FECHA_PAR;
        }else if(strcmp(s,"IF")==0){
            t.tipo=TOKEN_IF;
        }else if(strcmp(s,"DIV")==0){
            t.tipo=TOKEN_DIV;
        }else if (strcmp(s,"THEN")==0){
            t.tipo=TOKEN_THEN;
        }else if(strcmp(s,"ELSE")==0){
            t.tipo=TOKEN_ELSE;
        }else if (strcmp(s,"DO")==0){
            t.tipo=TOKEN_DO;
        }else if(strcmp(s,"WHILE")==0){
            t.tipo=TOKEN_WHILE;
        }
        
        
        
        
        
        strcpy(t.codigo,transforma_maiuscula(s));

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

void ERRO(char msg[],int linha){
    fprintf(stderr,"Erro na linha %d: %s\n",linha, msg);
    exit(EXIT_FAILURE);
}



char* transforma_maiuscula(char str[]){
    for(int i=0;str[i];i++){
        str[i]=toupper(str[i]);
    }
    return str;
}


//-------------------------Analisador Sintático ---------------------------------
void consume(){
    if(lista_entrada->prox!=NULL){
        lista_entrada=lista_entrada->prox;
        token_atual=lista_entrada->token;
    }
}
Token look_ahead(){
    Lista_Entrada *aux=lista_entrada;
    aux=aux->prox;
    Token t=aux->token;
    aux=NULL;
    free(aux);
    return t;
}
void ANALISADOR_SINTATICO(){
    programa();
}

void programa(){
   if(token_atual.tipo!=TOKEN_PROGRAM){
    ERRO("'PROGRAM' nao encontrado",token_atual.linha);
   }
   consume();
   if(token_atual.tipo!=TOKEN_IDENTIFICADOR){
    ERRO("Identificador invalido",token_atual.linha);
   }
   consume();
   if(token_atual.tipo!=TOKEN_PONTO_VIRGULA){
    ERRO("';' nao encontrado",token_atual.linha);
   }
   consume(); // Consome ;
   bloco();
   if(token_atual.tipo!=TOKEN_PONTO)ERRO("'.' faltante",token_atual.linha);

}

void bloco(){
 if(token_atual.tipo == TOKEN_VAR)parte_declaracoes_variaveis();
 if(token_atual.tipo == TOKEN_PROCEDURE || token_atual.tipo==TOKEN_FUNCTION)parte_declaracoes_sub_rotinas();

 comando_composto();
 
}
void parte_declaracoes_variaveis(){
 Token aux_token;
 consume(); //Consome VAR
 declaracao_variaveis();
 while (token_atual.tipo == TOKEN_PONTO_VIRGULA)
 {
    aux_token=look_ahead();
    if(aux_token.tipo!=TOKEN_IDENTIFICADOR)break;
    consume(); //consome ;
    declaracao_variaveis();
 }
  if(token_atual.tipo != TOKEN_PONTO_VIRGULA)ERRO("';' nao encontrado",token_atual.linha);
  consume();
 
 


}

void declaracao_variaveis(){
 lista_identificadores();
 if(token_atual.tipo != TOKEN_DOIS_PONTOS)ERRO("':' nao encontrado",token_atual.linha);
 consume();
 if(token_atual.tipo != TOKEN_INTEGER && token_atual.tipo != TOKEN_BOOLEAN)ERRO("Tipo invalido", token_atual.linha);
 consume();
}

void lista_identificadores(){
 if(token_atual.tipo != TOKEN_IDENTIFICADOR)ERRO("Identificador invalido", token_atual.linha);
 consume();
 while(token_atual.tipo==TOKEN_VIRGULA){
    consume();
    if(token_atual.tipo!=TOKEN_IDENTIFICADOR)break;

    consume();

    
 }
}

void parte_declaracoes_sub_rotinas(){
    while(token_atual.tipo==TOKEN_PROCEDURE || token_atual.tipo==TOKEN_FUNCTION){
        if(token_atual.tipo==TOKEN_PROCEDURE)declaracao_procedimento();
        else declaracao_funcao();
        if(token_atual.tipo != TOKEN_PONTO_VIRGULA)ERRO("';' nao encontrado", token_atual.linha);
        consume(); //consome ;
       
        
    }
}
void declaracao_procedimento(){
    consume(); // consome PROCEDURE
    if(token_atual.tipo != TOKEN_IDENTIFICADOR)ERRO("Identificador invalido", token_atual.linha);
    consume(); //consome IDENTIFICADOR
    if(token_atual.tipo==TOKEN_ABRE_PAR)parametros_formais();
    if(token_atual.tipo != TOKEN_PONTO_VIRGULA)ERRO("';' nao encontrado",token_atual.linha);
    consume(); //consome ;
    bloco();

}
void declaracao_funcao(){
    if(token_atual.tipo != TOKEN_FUNCTION)ERRO("'FUNCTION' nao identificada", token_atual.linha);

    consume(); //consome FUNCTION

    if(token_atual.tipo != TOKEN_IDENTIFICADOR)ERRO("Identificador invalido", token_atual.linha);

    consume(); //consome o identificador

    if(token_atual.tipo==TOKEN_ABRE_PAR)parametros_formais();

    if(token_atual.tipo != TOKEN_DOIS_PONTOS)ERRO("':' nao encontrado",token_atual.linha);

    consume(); // consome :

    if(token_atual.tipo != TOKEN_IDENTIFICADOR)ERRO("Identificador invalido", token_atual.linha);

    consume(); //consome o identificador

    if(token_atual.tipo != TOKEN_PONTO_VIRGULA)ERRO("';' nao encontrado",token_atual.linha);

    consume(); //consome ;

    bloco();

}
void parametros_formais(){
    if(token_atual.tipo!=TOKEN_ABRE_PAR)ERRO("'(' nao encontrado", token_atual.linha);
    consume();// consome (
    secao_parametros_formais();
    while(token_atual.tipo==TOKEN_PONTO_VIRGULA){
        consume(); //consome ;
        secao_parametros_formais();
    }
    if(token_atual.tipo!=TOKEN_FECHA_PAR)ERRO("')' nao encontrado",token_atual.linha);
    consume(); // consomoe )

}
void secao_parametros_formais(){
    if(token_atual.tipo == TOKEN_VAR)consume();
    lista_identificadores();
    if(token_atual.tipo!=TOKEN_DOIS_PONTOS)ERRO("':' nao identificado",token_atual.linha);
    consume();//consome :
    if(token_atual.tipo!=TOKEN_INTEGER && token_atual.tipo!=TOKEN_BOOLEAN)ERRO("tipo de variavel invalido",token_atual.linha);
    consume(); //consome INTEGER ou BOOLEAN
}

void comando_composto(){
    //printf("%s %d \n", token_atual.codigo, token_atual.linha);
    
    if(token_atual.tipo != TOKEN_BEGIN)ERRO("'BEGIN' nao encontrado",token_atual.linha);
    consume(); //consome BEGIN

    comando();

    while(token_atual.tipo==TOKEN_PONTO_VIRGULA){
        consume(); //consome ;
        comando();
    }

    if(token_atual.tipo != TOKEN_END)ERRO("'END' nao encontrado", token_atual.linha);

    consume(); //consome o END

}


void comando(){
    if(token_atual.tipo==TOKEN_NUMERO){
        consume(); // consome o numero
        if(token_atual.tipo!=TOKEN_DOIS_PONTOS)ERRO("':' nao encontrado", token_atual.linha);

        consume(); // consome :
    }

    comando_sem_rotulo();

}

void comando_sem_rotulo(){
    switch(token_atual.tipo){
        case TOKEN_IDENTIFICADOR:{
            Token aux=look_ahead();
            if(aux.tipo==TOKEN_ATRIBUICAO)atribuicao();
            else chamada_procedimento();
            break;
        }
        case TOKEN_BEGIN:
            comando_composto();
            break;
        case TOKEN_IF:
            comando_condicional();
            break;
        case TOKEN_WHILE:
            comando_repetitivo();
            break;
        default:
            ERRO("Comando desconhecido", token_atual.linha);
    }
}

void atribuicao(){
    //printf("%s\n", token_atual.lexema);
    if(token_atual.tipo != TOKEN_IDENTIFICADOR && token_atual.tipo!= TOKEN_READ && token_atual.tipo!= TOKEN_WRITE)ERRO("'Identificador invalido'",token_atual.linha);
    consume(); //consome identificador
    if(token_atual.tipo!=TOKEN_ATRIBUICAO)ERRO("Atribuicao invalida", token_atual.linha);
    consume(); //consome :=

    expressao();
}
void chamada_procedimento(){
    if(token_atual.tipo!=TOKEN_IDENTIFICADOR)ERRO("Identificador invalido", token_atual.linha);
    consume(); //consome identificador

    if(token_atual.tipo == TOKEN_ABRE_PAR){
        consume(); //consome (
        lista_expressoes();
        if(token_atual.tipo != TOKEN_FECHA_PAR)ERRO("')' nao encontrado", token_atual.linha);

        consume(); // consome )
    }
}

void comando_condicional(){
    if(token_atual.tipo!=TOKEN_IF)ERRO("'IF' nao encontrado", token_atual.linha);
    consume(); // consome IF
    expressao();
    if(token_atual.tipo!=TOKEN_THEN)ERRO("'THEN' nao encontrado", token_atual.linha);
    consume(); //consome THEN
    comando_sem_rotulo();

    if(token_atual.tipo==TOKEN_ELSE){
        consume(); //consome 
        comando_sem_rotulo();
    }
}

void comando_repetitivo(){
    if(token_atual.tipo!=TOKEN_WHILE)ERRO("'WHILE' nao encontrado", token_atual.linha);
    consume(); //consome WHILE
    
    expressao();

    if(token_atual.tipo!=TOKEN_DO)ERRO("'DO' nao encontrado", token_atual.linha);

    consume(); //consome DO

    comando_sem_rotulo();
}

void lista_expressoes(){
    expressao();
    while(token_atual.tipo==TOKEN_VIRGULA){
        consume(); //consome ,
        expressao();
    }
}

void expressao(){
    expressao_simples();
    if(token_atual.tipo==TOKEN_IGUAL || token_atual.tipo==TOKEN_MENOR || token_atual.tipo==TOKEN_MAIOR)
    {
        relacao();
        expressao_simples();
    }
}
void expressao_simples(){
    if(token_atual.tipo==TOKEN_MAIS || token_atual.tipo==TOKEN_MENOS)
    consume(); //consome + ou -

    termo();

    while(token_atual.tipo==TOKEN_MAIS || token_atual.tipo==TOKEN_MENOS || token_atual.tipo==TOKEN_OR){
        consume();// consome + - or
        termo();
    }
}

void relacao(){
    if(token_atual.tipo==TOKEN_IGUAL)consume(); //consome =
    else if(token_atual.tipo==TOKEN_MENOR){
        consume(); //consome <
        if(token_atual.tipo==TOKEN_MAIOR || token_atual.tipo==TOKEN_IGUAL)
        consume(); //consome > ou =
    }
    else if(token_atual.tipo==TOKEN_MAIOR){
        consume(); //consome >
        if(token_atual.tipo==TOKEN_IGUAL)consume(); //consome >=
    }
}

void termo(){
    fator();
    while(token_atual.tipo==TOKEN_MULT || token_atual.tipo==TOKEN_DIV || token_atual.tipo==TOKEN_AND){
        consume(); //consome * div and
        fator();
    }
}

void fator(){
    switch(token_atual.tipo){
        case TOKEN_IDENTIFICADOR:
        {
            Token aux=look_ahead();
            if(aux.tipo==TOKEN_ABRE_PAR)chamada_funcao();
            else{
            consume();//consome identificador
            }

            break;
        }
        case TOKEN_NUMERO:
            consume(); //consome numero
            break;
        case TOKEN_ABRE_PAR:
            consume(); //consome (
            expressao();
            if(token_atual.tipo!=TOKEN_FECHA_PAR)ERRO("')' nao encontrado", token_atual.linha);
            consume(); //consome )
            break;
        case TOKEN_NOT:
            consume(); //consome not
            fator();
            break;
        default:
        ERRO("Expressao invalida", token_atual.linha);
        
    }
}
void chamada_funcao(){
    if(token_atual.tipo!=TOKEN_IDENTIFICADOR)ERRO("Identificador de funcao invalido",token_atual.linha);
    consume(); //consome o identificador

    if(token_atual.tipo==TOKEN_ABRE_PAR){
        consume(); //consome (
        lista_expressoes();
        if(token_atual.tipo!=TOKEN_FECHA_PAR)ERRO("')' nao encontrado",token_atual.linha);

        consume(); //consome )
    }
}