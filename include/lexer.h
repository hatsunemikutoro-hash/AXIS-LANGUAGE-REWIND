#ifndef LEXER_H
#define LEXER_H

typedef enum {
    // LITERAIS
    TOKEN_NUMBER,
    TOKEN_STRING,
    TOKEN_IDENT,

    // KEYWORDS ESTRUTURAIS
    TOKEN_FUNCTION,  // function
    TOKEN_END,       // end / }
    TOKEN_RETURN,    // return

    // FLUXO
    TOKEN_IF,        // if
    TOKEN_CALL,      // call

    // COMANDOS
    TOKEN_STORE,     // store x 10
    TOKEN_PRINT,     // print x
    TOKEN_ADD,       // add a b -> result
    TOKEN_SUB,       // sub a b -> result
    TOKEN_MUL,       // mul a b -> result
    TOKEN_DIV,       // div a b -> result

    // OPERADORES
    TOKEN_ARROW,     // ->
    TOKEN_LBRACE,    // {
    TOKEN_RBRACE,    // }

    // CONTROLE
    TOKEN_NEWLINE,
    TOKEN_EOF,
    TOKEN_UNKNOWN,
} Tokenkind;

typedef struct {
    const char *src;
    int i;
    int line;
    int indent; // nível de indentação atual
} Lexer;

typedef struct {
    Tokenkind kind;
    char *value;
    int line;
} Token;

Token next_token(Lexer *l);

#endif