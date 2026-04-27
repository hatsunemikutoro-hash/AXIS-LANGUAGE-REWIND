#ifndef AST_H
#define AST_H

typedef enum {
    // LITERAIS E IDENTIFICADORES
    NODE_NUMBER,
    NODE_STRING,
    NODE_IDENT,

    // OPERAÇÕES (Agrupei para facilitar o switch no interpreter)
    NODE_ARITHMETIC, // Para ADD, SUB, INC, DEC, CLEAR
    NODE_VAR_DECL,   // Para LET, VAR
    NODE_ASSIGN,     // Para quando você atribui valor

    // FLUXO
    NODE_MARK,       // Labels
    NODE_JUMP,
    NODE_IF,
    NODE_COMPARE,    // Para EQ, LT, GT

    // I/O E SYS
    NODE_READ,
    NODE_PRINT,
    NODE_EXIT,
    NODE_SYSTEM,     // DEBUG, NODEBUG, LOAD

    // FUNÇÕES
    NODE_FUNC_DEF,
    NODE_CALL,
    NODE_RETURN,

    // ESTRUTURA
    NODE_BLOCK,      // Um grupo de comandos entre colchetes ou linhas
    NODE_PROGRAM     // A raiz de tudo
} NodeKind;

typedef struct ASTNode {
    NodeKind kind;
    
    // Dados (usamos union para economizar memória, já que um nó não é tudo ao mesmo tempo)
    union {
        double number_val;   // Para TOKEN_NUMBER
        char* string_val;    // Para TOKEN_STRING ou TOKEN_IDENT
    } data;

    int op_kind;             // Guarda o Tokenkind original (ex: TOKEN_ADD) para saber qual operação fazer

    struct ASTNode *left;    // Filho esquerdo
    struct ASTNode *right;   // Filho direito (ou próximo comando na lista)
} ASTNode;

// Funções da "Fábrica" (que vão no ast.c)
ASTNode* create_node(NodeKind kind);
void free_ast(ASTNode* node);
void debug_print_ast(ASTNode* node, int level);

#endif