#include "ast.h"
#include <stdlib.h>
#include <stdio.h>

ASTNode* create_node(NodeKind kind) {
    ASTNode* node = malloc(sizeof(ASTNode)); // alloca memoria pra struct ASTnode
    if (!node) { fprintf(stderr, "Falha ao alocar node\n"); exit(1); } // error handling

    // so inicializacao
    node->kind        = kind;
    node->value       = NULL;
    node->number      = 0;
    node->left        = NULL;
    node->right       = NULL;
    node->child_count = 0;
    node->capacity    = 4;
    node->children    = malloc(sizeof(ASTNode*) * node->capacity); // allocamento para o tipo da struct * capacidade
    if (!node->children) { fprintf(stderr, "Falha ao alocar children\n"); exit(1); } // error handling

    return node; // retorna o node
}

void add_child(ASTNode *parent, ASTNode *child) {
    if (!child) { fprintf(stderr, "Erro: child NULL\n"); exit(1); } // se nao tiver child retorna erro
    if (parent->child_count >= parent->capacity) { // se o tamanho de child for maior ou igual
        parent->capacity *= 2; // aumenta a capacidade pelo dobro
        ASTNode **tmp = realloc(parent->children, sizeof(ASTNode*) * parent->capacity); // gera um onteiro d ponteiro de uma ASTNODE temp e realloca a memoria
        if (!tmp) { fprintf(stderr, "Falha no realloc\n"); exit(1); } // se der erro retorna erro
        parent->children = tmp; // children ´e igual ao temporario
    }
    parent->children[parent->child_count++] = child; // o numero de crianças ai pega o valor la dentro adicionando mais um e é igual a child
}

// transforma KIND em str === DEBUG only
static const char* kind_to_str(NodeKind kind) {
    switch (kind) {
        case NODE_NUMBER:   return "NUMBER";
        case NODE_STRING:   return "STRING";
        case NODE_IDENT:    return "IDENT";
        case NODE_PROGRAM:  return "PROGRAM";
        case NODE_BLOCK:    return "BLOCK";
        case NODE_FUNC_DEF: return "FUNC_DEF";
        case NODE_CALL:     return "CALL";
        case NODE_RETURN:   return "RETURN";
        case NODE_STORE:    return "STORE";
        case NODE_PRINT:    return "PRINT";
        case NODE_ARITH:    return "ARITH";
        case NODE_IF:       return "IF";
        default:            return "UNKNOWN";
    }
}

// DEBUG
void debug_print_ast(ASTNode* node, int level) {
    if (!node) return;
    for (int i = 0; i < level; i++) printf("  ");
    printf("%s", kind_to_str(node->kind));
    if (node->value)               printf(" ('%s')", node->value);
    if (node->kind == NODE_NUMBER) printf(" (%lld)", (long long)node->number);
    printf("\n");
    if (node->left)  debug_print_ast(node->left,  level + 1);
    for (int i = 0; i < node->child_count; i++)
        debug_print_ast(node->children[i], level + 1);
    if (node->right) debug_print_ast(node->right, level + 1);
}

void free_ast(ASTNode* node) {
    if (!node) return; // se nao  tiver node n faz nada
    free_ast(node->left); // recursa pro node left
    free_ast(node->right); // recursa pro node right
    for (int i = 0; i < node->child_count; i++) // para cada criança
        free_ast(node->children[i]); // limpar a ast
    free(node->children); // libera mem
    if (node->value) free(node->value); //  se tiver valor libera mem
    free(node);// libera mem
}