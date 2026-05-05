#include "lexer.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

typedef struct {
    const char *name; // nome da Keyword
    Tokenkind kind; // tipo dela
} Keyword;

static const Keyword keywords[] = {
    {"function", TOKEN_FUNCTION},
    {"end",      TOKEN_END},
    {"return",   TOKEN_RETURN},
    {"if",       TOKEN_IF},
    {"call",     TOKEN_CALL},
    {"store",    TOKEN_STORE},
    {"print",    TOKEN_PRINT},
    {"add",      TOKEN_ADD},
    {"sub",      TOKEN_SUB},
    {"mul",      TOKEN_MUL},
    {"div",      TOKEN_DIV},
};

static Token make_token(Tokenkind kind, const char *val, int len, int line) {
    Token t; // cria o token t
    t.kind  = kind; // define o kind
    t.line  = line; // define a line
    t.value = malloc(len + 1); // alloca memoria pra ele
    if (!t.value) { // se nao tiver é pq falhou a alocacao
        fprintf(stderr, "Erro: falha ao alocar token na linha %d\n", line);
        exit(1);
    }
    memcpy(t.value, val, len); // especificar o intervalo de caracteres q num pode ser excedido pela memoria origem
    t.value[len] = '\0'; // adiciona um null term no final
    return t; // retorna o token
}

static void skip_whitespace(Lexer *l) {
    while (l->src[l->i] == ' ' || l->src[l->i] == '\t') // enquanto o atual  char é igual a um espaço ou a uma sequencia de escape (tab)
        l->i++; // continua indo
}

static void skip_comment(Lexer *l) {
    if (l->src[l->i] == ';') { // se achar um comentario
        while (l->src[l->i] != '\0' && l->src[l->i] != '\n') // enquanto nao for o final do arquivo e nao for uma nova linha
            l->i++; // continua
    }
}

static Tokenkind lookup_keyword(const char *text) {
    int n = sizeof(keywords) / sizeof(keywords[0]); // tamanho do numero de keywords dividido pelo tamanho da primeira keyword?
    for (int i = 0; i < n; i++)
        if (strcmp(text, keywords[i].name) == 0) // se o texto for igual a algum da lista de keys retorna o tipo dele se nao retorna ident
            return keywords[i].kind;
    return TOKEN_IDENT;
}

static Token read_ident(Lexer *l) {
    int start = l->i; // começa onde o index atual esta
    while (isalnum((unsigned char)l->src[l->i]) || l->src[l->i] == '_') // enquanto for um numerico ou alfabetico ou _
        l->i++; // continua
    int len = l->i - start; //  a distancia entre o index e o começo de onde ele estava
    Token t = make_token(TOKEN_IDENT, l->src + start, len, l->line); // faz o token
    t.kind = lookup_keyword(t.value);// procura ele na keyword
    return t; // retorna token
}

static Token read_number(Lexer *l) {
    int start = l->i; // define start
    while (isdigit((unsigned char)l->src[l->i])) // se for um digito (0-9)
        l->i++; // aumenta
    int len = l->i - start; // distancia entre o index e o começo de onde ele estava
    return make_token(TOKEN_NUMBER, l->src + start, len, l->line); // gera o token
}

static Token read_string(Lexer *l) {
    l->i++; // pula aspas de abertura
    int start = l->i; // começo
    while (l->src[l->i] != '"' && l->src[l->i] != '\0') { // enquanto index nao for o  " e nao for o \0
        if (l->src[l->i] == '\n') l->line++; // se achar newline aumenta o contador de line e dps aumenta o index
        l->i++;
    }
    if (l->src[l->i] == '\0') {
        fprintf(stderr, "Erro: string nao fechada na linha %d\n", l->line); // error handling
        exit(1);
    }
    int len = l->i - start; // distancia
    Token t = make_token(TOKEN_STRING, l->src + start, len, l->line); // gera o token
    l->i++; // pula aspas de fechamento
    return t; // retorna  token
}

Token next_token(Lexer *l) {
    // consome espaços e comentários (mas não newlines)
    while (1) {
        skip_whitespace(l); // skipa espaço
        if (l->src[l->i] == ';') {
            skip_comment(l); // eskipa comentario
            continue;
        }
        break; // quebra dps
    }

    char c = l->src[l->i]; // char é igual ao index q é um character provavelmente ou um numero sei la

    if (c == '\0')  return make_token(TOKEN_EOF,     "EOF", 3, l->line); // se for o fim do arquivo enviar EOF
    if (c == '\n') { // se for uma newline
        l->i++; // aumenta o index
        l->line++; // aumenta a line
        return make_token(TOKEN_NEWLINE, "\n", 1, l->line); // gera token de newline
    }

    if (isalpha((unsigned char)c) || c == '_') return read_ident(l); // se for alphanumerico leia ident
    if (isdigit((unsigned char)c))             return read_number(l); // se for numero leia numero
    if (c == '"')                              return read_string(l); // se for string leia string

    // operadores simbólicos
    switch (c) {
        case '-':
            if (l->src[l->i + 1] == '>') { // se index o proximo index for > é uma arrow
                l->i += 2; // pula 2 caracteres?
                return make_token(TOKEN_ARROW, "->", 2, l->line); // gera token arrow
            }
            break;
        case '{':
            l->i++; // continua
            return make_token(TOKEN_LBRACE, "{", 1, l->line);
        case '}':
            l->i++; // continua
            return make_token(TOKEN_RBRACE, "}", 1, l->line);
    }

    // caractere desconhecido
    l->i++;
    return make_token(TOKEN_UNKNOWN, &c, 1, l->line);
}