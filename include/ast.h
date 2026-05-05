#ifndef AST_H
#define AST_H
#include <stdint.h>
#include "lexer.h"

typedef enum {
    NODE_NUMBER,
    NODE_STRING,
    NODE_IDENT,

    NODE_PROGRAM,    // raiz
    NODE_BLOCK,      // bloco de statements (corpo de func, if, etc)

    NODE_FUNC_DEF,   // function nome params... { corpo }
    NODE_CALL,       // call nome args... -> dest
    NODE_RETURN,     // return valor

    NODE_STORE,      // store x 10
    NODE_PRINT,      // print x

    NODE_ARITH,      // add/sub/mul/div a b -> result

    NODE_IF,         // if cond \n bloco end

} NodeKind;

typedef struct ASTNode {
    NodeKind kind;

    char *value;       // nome, label, op ("add","sub"...)
    int64_t number;

    // filhos genéricos — usado para listas (params, args, corpo do bloco)
    struct ASTNode **children;
    int child_count;
    int capacity;

    // filhos semânticos para nodes binários/unários
    struct ASTNode *left;   // operando esquerdo / condição / valor
    struct ASTNode *right;  // operando direito / destino

} ASTNode;

ASTNode* create_node(NodeKind kind);
void     add_child(ASTNode *parent, ASTNode *child);
void     debug_print_ast(ASTNode *node, int level);
void     free_ast(ASTNode *node);

#endif