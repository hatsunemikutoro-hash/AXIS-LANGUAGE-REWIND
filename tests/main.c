// main.c
#include <stdio.h>
#include <stdlib.h>
#include "lexer.h"
#include "parser.h"
#include "ast.h"
#include "interpreter.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Uso: ./axis arquivo.ext\n");
        return 1;
    } // verifica se temos o primeiro argumento

    // lê o arquivo inteiro pra uma string
    FILE *f = fopen(argv[1], "r");
    if (!f) { perror("Erro ao abrir arquivo"); return 1; } // pega o file do primeiro argumento

    fseek(f, 0, SEEK_END); // vai até o começo do arquivo
    long size = ftell(f); // retorna a posicao atual
    rewind(f);  // define a posicao do arquivo para o inicio

    char *src = malloc(size + 1); // alocamos memoria para a src q sera usada pelo lexer
    fread(src, 1, size, f); // le os dados do arquivo
    src[size] = '\0'; // no final adiciona o null term
    fclose(f); // fecha o arquivo

    // pipeline completo
    Lexer   l   = { .src = src, .i = 0, .line = 1 };
    ASTNode *ast = parse_program(&l);
    interpret(ast);

    free_ast(ast);
    free(src);
    return 0;
}