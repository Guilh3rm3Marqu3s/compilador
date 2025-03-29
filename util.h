#ifndef UTIL_H
#define UTIL_H
typedef enum{
    TOKEN_IDENTIFICADOR,
    TOKEN_NUMERO,
    TOKEN_ATRIBUICAO, //:=
    TOKEN_DOIS_PONTOS, //:
    TOKEN_IGUAL, //=
    TOKEN_BEGIN,
    TOKEN_END,
    TOKEN_EOF,
    TOKEN_MAIS,//+
    TOKEN_MENOS,//-
    TOKEN_MULT,
    TOKEN_INTEGER,
    TOKEN_VAR,
    TOKEN_PROGRAM,
    TOKEN_PROCEDURE,
    TOKEN_FUNCTION,
    TOKEN_MENOR,
    TOKEN_MAIOR,
    TOKEN_MENOR_IGUAL,
    TOKEN_MAIOR_IGUAL,
    TOKEN_PONTO_VIRGULA,
    TOKEN_DIFERENTE,
    TOKEN_ESPACO,
    TOKEN_TAB,
    TOKEN_NOVA_LINHA,



} TipoToken;

typedef struct{
    TipoToken tipo;
    char valor[60]; //atomo
    char codigo[60];
}Token;

#endif