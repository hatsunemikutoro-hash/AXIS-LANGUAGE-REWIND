#ifndef LEXER_H
#define LEXER_H

typedef enum {
    //LITERAIS
    TOKEN_NUMBER,
    TOKEN_STRING,

    // IDENTIFIER
    TOKEN_IDENT,
    
    // KEYWORDS DA ARITMETICA
    TOKEN_INC,
    TOKEN_DEC,
    TOKEN_CLEAR,
    TOKEN_ADD,
    TOKEN_SUB,

    // KEYWORDS - VARIAVEIS
    TOKEN_LET,
    TOKEN_VAR,

    // FLUXO
    TOKEN_MARK,
    TOKEN_JUMP,
    TOKEN_IF,
    TOKEN_EQ,
    TOKEN_LT,
    TOKEN_GT,

    // KEYWORDS - I/O
    TOKEN_READ,
    TOKEN_PRINT,

    // KEYWORD - FUNCOES
    TOKEN_DEF,
    TOKEN_END,
    TOKEN_CALL,
    TOKEN_RETURN,

    // KEYWORDS - MODULOS
    TOKEN_LOAD,

    // KEYWORDS - SYS
    TOKEN_EXIT,
    TOKEN_DEBUG,
    TOKEN_NODEBUG,

    // SIMBOLOS
    TOKEN_LBRACKET,
    TOKEN_RBRACKET,

    // CONTROLE
    TOKEN_NEWLINE,
    TOKEN_EOF,
    TOKEN_UNKNOW,
}Tokenkind;

typedef struct
{
    const char *src; // é pra ser a string inteira
    int i; // a posicao atual o caractere
    int line; // a linha atual
}Lexer;

typedef struct
{
    Tokenkind kind;
    char *value;
    int line;
}Token;


Token next_token(Lexer *l);

#endif