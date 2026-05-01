#include "lexer.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>



// Keywords -- metodo de nao fritar meu cerebro

typedef struct
{
    const char *name;
    Tokenkind kind;
}Keyword;

static const Keyword keywords[] = {
    {"PRINT", TOKEN_PRINT},
    {"INC", TOKEN_INC},
    {"DEC", TOKEN_DEC},
    {"CLEAR", TOKEN_CLEAR},
    {"ADD", TOKEN_ADD},
    {"SUB", TOKEN_SUB},
    {"VAR", TOKEN_VAR},
    {"MARK", TOKEN_MARK},
    {"JUMP", TOKEN_JUMP},
    {"IF", TOKEN_IF},
    {"EQ", TOKEN_EQ},
    {"LT", TOKEN_LT},
    {"GT", TOKEN_GT},
    {"READ", TOKEN_READ},
    {"DEF", TOKEN_DEF},
    {"END", TOKEN_END},
    {"CALL", TOKEN_CALL},
    {"RETURN", TOKEN_RETURN},
    {"LOAD", TOKEN_LOAD},
    {"EXIT", TOKEN_EXIT},
    {"DEBUG", TOKEN_DEBUG},
    {"NODEBUG", TOKEN_NODEBUG},
};

// funcoes auxiliares
static Token make_token(Tokenkind kind, const char *val, int len, int line) {
    Token t;
    t.kind = kind;
    t.line = line;
    t.value = malloc(len + 1);
    if (t.value == NULL) {
        fprintf(stderr,"Erro: Falha ao alocar memória para o token na linha %d\n", line);
        exit(1); // Para o programa imediatamente
    }
    memcpy(t.value, val,  len);
    t.value[len] = '\0';
    return t;
}

// meu cerebro doi
static void skip_whitespace(Lexer *l) {
    // enquanto o caractere for nulo, ou um espaço, ou for uma quebra de linha
    while (l->src[l->i] == '\n' || l->src[l->i] == ' ' || l->src[l->i] == '\t')
    {
        if (l->src[l->i] == '\n') { // se for uma quebra de linha aumenta a linha
            l->line ++;
        }
        l->i++; // avança pra proxima letra
    }
}

static void skip_comment(Lexer *l) {
    if (l->src[l->i] == '$') {
        l->i++;
        while (l->src[l->i] != '\0' && l->src[l->i] != '\n')
        {
            l->i++;
        }
        
    }
}

static Tokenkind lookup_keyword(const char *text) {
    int num_keywords = sizeof(keywords) / sizeof(keywords[0]); // calcula quantas keywords a gente tem
    for (int i = 0; i < num_keywords; i++)
    {
        if (strcmp(text, keywords[i].name) == 0) { // compara com cada nome da lista
            return keywords[i].kind; // e retorna a kind dele
        }
    }
    return TOKEN_IDENT; // se nao for nenhuma keyword é variavel do usuario

}

static Token read_ident(Lexer *l) {
    int start = l->i; // pega o começo 
    while (isalnum(l->src[l->i])) // verfica se é alpha nbumerico e incrementa
    {
        l->i++;
    }
    int len = l->i - start; // calcula o tamanho
    Token t = make_token(TOKEN_IDENT, l->src + start, len, l->line); // gera token
    t.kind = lookup_keyword(t.value);
    return t;
}

static Token read_number(Lexer *l) {
    int start = l->i; // pega o começo 
    while (isdigit(l->src[l->i]) || l->src[l->i] == '_') // verfica se é alpha nbumerico e incrementa
    {
        l->i++;
    }
    int len = l->i - start; // calcula o tamanho
    return make_token(TOKEN_NUMBER, l->src + start, len, l->line); // gera token
}

static Token read_string(Lexer *l) {
    l->i++; // pula da aspas pro texto
    int start = l->i; // salva o começo da string
    while (l->src[l->i] != '"' && l->src[l->i] != '\0' ) // enquanto nao achar a aspas final e nao for o fim do arquivo
    {       
            if (l->src[l->i] == '\n') // ja aproveita e aumenta o contador de linha se tiver quebra
            {
                l->line++;
            }
            
            l->i++; // aumenta onde a gente está no texto
    }

    if (l->src[l->i] == '\0') {
        fprintf(stderr, "Erro: a string nao foi fechada"); // error handle pa nois
        exit(1);
    }

    int len = l->i - start;
    Token t = make_token(TOKEN_STRING, l->src + start, len, l->line);

    l->i++;
    return t;
}

Token next_token(Lexer *l) {
    skip_whitespace(l); // pula espaço em branco
    char c = l->src[l->i]; // pega o primeiro character

    if (c == '\0') { // se for o fim do arquivo
        Token t = make_token(TOKEN_EOF, "EOF", 3, l->line);
        printf("[LEXER] kind=%d val='%s'\n", t.kind, t.value);
        return t;
    }

    if (c == '$') { // se for comentario
        skip_comment(l); // pula o comentario e vai pro proximo token
        return next_token(l);
    }

    Token t;
    if(isalpha(c) || c == '_') t = read_ident(l);
    else if(isdigit(c)) t = read_number(l);
    else if(c == '"') t = read_string(l);
    else switch (c) {
        case '[': l->i++; t = make_token(TOKEN_LBRACKET, "[", 1, l->line); break;
        case ']': l->i++; t = make_token(TOKEN_RBRACKET, "]", 1, l->line); break;
        default: t = make_token(TOKEN_UNKNOW, &c, 1, l->line);
    }

    printf("[LEXER] kind=%d val='%s'\n", t.kind, t.value);
    return t;
}



