#include "ast.h"
#include <stdlib.h>
#include <stdio.h>

ASTNode* create_node(NodeKind kind) {
    ASTNode* node = malloc(sizeof(ASTNode)); // ja aloca memoria pro NODE

    if (node == NULL) { // so uma trava de segurança padrao
        fprintf(stderr, "Falhou ao alocar memoria");
        exit(1);
    }

    // inicializando - deixei a data de lado pq no parser ele ja vai inicializar ela mesmo
    node->kind = kind;
    node->left = NULL;
    node->right = NULL;
    node->op_kind = 0;

    return node;
}

// funcao de debug gerada por ia so pelo teste
void debug_print_ast(ASTNode* node, int level) {
    if (!node) return;
    for (int i = 0; i < level; i++) printf("  "); // Indentação pra ver a hierarquia

    if (node->kind == NODE_PRINT) {
        printf("STMT: PRINT\n");
    } else if (node->kind == NODE_NUMBER) {
        printf("VALUE: %f\n", node->data.number_val);
    } else if (node->kind == NODE_ARITHMETIC) {
        printf("OP: %d\n", node->op_kind);
    } else if (node->kind == NODE_PROGRAM) {
        printf("PROGRAM_BLOCK\n");
    }

    debug_print_ast(node->left, level + 1);
    debug_print_ast(node->right, level + 1);
}