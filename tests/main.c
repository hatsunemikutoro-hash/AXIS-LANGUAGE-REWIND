#include "lexer.h"
#include "parser.h"
#include "ast.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
    const char *codigo_teste = "ADD X 10";

    Lexer l;
    l.src = codigo_teste;
    l.line = 1;
    l.i = 0;

    printf("--- TESTANDO ESTA BOSTA 2.0 ---\n");
    printf("Input: %s\n\n", codigo_teste);

    ASTNode* root = parse_program(&l);
    printf(">>> AST construída com sucesso!\n");
    debug_print_ast(root, 0); // debug
    printf("\n--- EXECUÇÃO FINALIZADA ---\n");
    return 0;
}