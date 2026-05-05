#define _POSIX_C_SOURCE 200809L
#include "ast.h"
#include "lexer.h"
#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ─── helpers ────────────────────────────────────────────────────────────────

static void advance(Lexer *l, Token *cur) {
    *cur = next_token(l);
}

static void skip_newlines(Lexer *l, Token *cur) {
    while (cur->kind == TOKEN_NEWLINE)
        advance(l, cur);
}

static void eat(Lexer *l, Token *cur, Tokenkind expected) {
    if (cur->kind != expected) {
        fprintf(stderr, "Erro na linha %d: esperado token %d, veio %d ('%s')\n",
                cur->line, expected, cur->kind, cur->value ? cur->value : "?");
        exit(1);
    }
    advance(l, cur);
}

// ─── átomos ─────────────────────────────────────────────────────────────────

// Lê um IDENT, NUMBER ou STRING e retorna o node
static ASTNode* parse_atom(Lexer *l, Token *cur) {
    ASTNode *node = NULL;
    if (cur->kind == TOKEN_NUMBER) {
        node = create_node(NODE_NUMBER);
        node->number = strtoull(cur->value, NULL, 10);
        eat(l, cur, TOKEN_NUMBER);
    } else if (cur->kind == TOKEN_IDENT) {
        node = create_node(NODE_IDENT);
        node->value = strdup(cur->value);
        eat(l, cur, TOKEN_IDENT);
    } else if (cur->kind == TOKEN_STRING) {
        node = create_node(NODE_STRING);
        node->value = strdup(cur->value);
        eat(l, cur, TOKEN_STRING);
    } else {
        fprintf(stderr, "Erro na linha %d: esperado valor, veio '%s'\n",
                cur->line, cur->value ? cur->value : "?");
        exit(1);
    }
    return node;
}

// ─── statements ─────────────────────────────────────────────────────────────

static ASTNode* parse_block(Lexer *l, Token *cur);

// store x 10
static ASTNode* parse_store(Lexer *l, Token *cur) {
    ASTNode *node = create_node(NODE_STORE);
    eat(l, cur, TOKEN_STORE);
    node->left  = parse_atom(l, cur); // nome da variável
    node->right = parse_atom(l, cur); // valor
    return node;
}

// print x
static ASTNode* parse_print(Lexer *l, Token *cur) {
    ASTNode *node = create_node(NODE_PRINT);
    eat(l, cur, TOKEN_PRINT);
    node->left = parse_atom(l, cur);
    return node;
}

// return valor
static ASTNode* parse_return(Lexer *l, Token *cur) {
    ASTNode *node = create_node(NODE_RETURN);
    eat(l, cur, TOKEN_RETURN);
    node->left = parse_atom(l, cur);
    return node;
}

// add a b -> result   (vale também pra sub, mul, div)
static ASTNode* parse_arith(Lexer *l, Token *cur) {
    ASTNode *node = create_node(NODE_ARITH);
    node->value = strdup(cur->value); // "add", "sub", "mul", "div"
    advance(l, cur); // consome o token do operador

    node->left  = parse_atom(l, cur); // operando esquerdo
    node->right = parse_atom(l, cur); // operando direito

    // -> destino  (opcional: nem todo arith precisa de destino)
    if (cur->kind == TOKEN_ARROW) {
        eat(l, cur, TOKEN_ARROW);
        add_child(node, parse_atom(l, cur)); // destino em children[0]
    }
    return node;
}

// call soma x y -> z
static ASTNode* parse_call(Lexer *l, Token *cur) {
    ASTNode *node = create_node(NODE_CALL);
    eat(l, cur, TOKEN_CALL);

    node->left = parse_atom(l, cur); // nome da função

    // argumentos: qualquer IDENT ou NUMBER que não seja ->
    while (cur->kind == TOKEN_IDENT || cur->kind == TOKEN_NUMBER) {
        add_child(node, parse_atom(l, cur));
    }

    // -> destino (opcional)
    if (cur->kind == TOKEN_ARROW) {
        eat(l, cur, TOKEN_ARROW);
        node->right = parse_atom(l, cur);
    }
    return node;
}

// if cond
//     bloco indentado
// end
static ASTNode* parse_if(Lexer *l, Token *cur) {
    ASTNode *node = create_node(NODE_IF);
    eat(l, cur, TOKEN_IF);

    node->left = parse_atom(l, cur); // condição

    eat(l, cur, TOKEN_NEWLINE);

    node->right = parse_block(l, cur); // bloco indentado

    skip_newlines(l, cur);
    eat(l, cur, TOKEN_END); // consome o "end"
    return node;
}

// function soma a b {
//     corpo
// }
static ASTNode* parse_func_def(Lexer *l, Token *cur) {
    ASTNode *node = create_node(NODE_FUNC_DEF);
    eat(l, cur, TOKEN_FUNCTION);

    node->left = parse_atom(l, cur); // nome da função

    // parâmetros: idents até o {
    while (cur->kind == TOKEN_IDENT) {
        add_child(node, parse_atom(l, cur));
    }

    eat(l, cur, TOKEN_LBRACE);
    eat(l, cur, TOKEN_NEWLINE);

    node->right = parse_block(l, cur); // corpo

    skip_newlines(l, cur);
    eat(l, cur, TOKEN_RBRACE); // consome o }
    return node;
}

// ─── bloco ──────────────────────────────────────────────────────────────────

// Lê statements até encontrar END, } ou EOF (não consome o terminador)
static ASTNode* parse_block(Lexer *l, Token *cur) {
    ASTNode *block = create_node(NODE_BLOCK);

    while (1) {
        skip_newlines(l, cur);

        // terminadores de bloco
        if (cur->kind == TOKEN_EOF  ||
            cur->kind == TOKEN_END  ||
            cur->kind == TOKEN_RBRACE) {
            break;
        }

        ASTNode *stmt = NULL;

        switch (cur->kind) {
            case TOKEN_STORE:    stmt = parse_store(l, cur);    break;
            case TOKEN_PRINT:    stmt = parse_print(l, cur);    break;
            case TOKEN_RETURN:   stmt = parse_return(l, cur);   break;
            case TOKEN_ADD:
            case TOKEN_SUB:
            case TOKEN_MUL:
            case TOKEN_DIV:      stmt = parse_arith(l, cur);    break;
            case TOKEN_CALL:     stmt = parse_call(l, cur);     break;
            case TOKEN_IF:       stmt = parse_if(l, cur);       break;
            case TOKEN_FUNCTION: stmt = parse_func_def(l, cur); break;
            default:
                fprintf(stderr, "Erro na linha %d: statement inesperado '%s'\n",
                        cur->line, cur->value ? cur->value : "?");
                exit(1);
        }

        add_child(block, stmt);
    }

    return block;
}

// ─── entrada ────────────────────────────────────────────────────────────────

ASTNode* parse_program(Lexer *l) {
    Token cur;
    advance(l, &cur);

    ASTNode *root = parse_block(l, &cur);
    root->kind = NODE_PROGRAM;

    if (cur.kind != TOKEN_EOF) {
        fprintf(stderr, "Erro na linha %d: token inesperado no fim '%s'\n",
                cur.line, cur.value ? cur.value : "?");
        exit(1);
    }

    return root;
}