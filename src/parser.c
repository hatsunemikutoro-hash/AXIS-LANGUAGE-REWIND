#define _POSIX_C_SOURCE 200809L
#include "ast.h"
#include "lexer.h"
#include "parser.h" 
#include <stdio.h> 
#include <stdlib.h>
#include <string.h>

// passei 30 minutos sem fazer nada so pensando como vou estruturar isso aqui
// nunca me senti tao burro em toda minha vida
// pqp.

// a roda que move pra proximaparte
void advance(Lexer *l, Token *current_token) {
    *current_token = next_token(l);
}

void eat(Lexer *l, Token *current_token, Tokenkind expected_kind) {
    if (current_token->kind == expected_kind) // se token atual é igual ao token que era esperado
    // avançar senão o programa para e dá erro
    {
        advance(l, current_token);
    } else {
        fprintf(stderr, "Erro, esperado token %d veio %d", expected_kind, current_token->kind);
        exit(1);
    }
    
}

ASTNode* parse_factor(Lexer *l, Token *current_token) { // Verifica se é numero ou variavel
    ASTNode* node = NULL;

    if (current_token->kind == TOKEN_NUMBER) // se a kind for igual ao token numero cria um node do tipo number
    {
        node = create_node(NODE_NUMBER);
        node->data.number_val = atof(current_token->value);
        eat(l, current_token, TOKEN_NUMBER);
    } else if (current_token->kind == TOKEN_IDENT)
    {
        node = create_node(NODE_IDENT); // o mesmo so que pra variavel
        node->data.string_val = strdup(current_token->value);
        eat(l, current_token, TOKEN_IDENT);
    } else {
        fprintf(stderr, "Esperado variavel ou numero mas veio %d\n", current_token->kind);
        exit(1);
    }
    return node;
}

ASTNode* parse_expression(Lexer *l, Token *current_token) {
    ASTNode *left = parse_factor(l, current_token); // pega o primeiro numero ou variavel

    while (current_token->kind == TOKEN_ADD || current_token->kind == TOKEN_SUB) // enquanto for um sinal de + ou -
    {
        Token op = *current_token; // salva o sinal
        advance(l, current_token); // vai pro proximo
    

    ASTNode* newNode = create_node(NODE_ARITHMETIC); // cria um novo node
    newNode->kind = NODE_ARITHMETIC; // agora o tipo desse node é ARITIMETICAAA
    newNode->op_kind = op.kind; // a kind desse node é o mesmo que a kind do operator
    newNode->left = left; // a esquerda desse newnode vira a esquerda 
    newNode->right = parse_factor(l, current_token); // e avança pra pegar a direita
    
    left = newNode;
    }

    return left;
}

// a esse ponto eu ja estou insano se nao fosse a ia me explicar tudo igual se eu fosse um jumento :D

// TODO FUNCAO PARA CADA UM
ASTNode* parse_print(Lexer *l, Token *current_token) {
    ASTNode* node = create_node(NODE_PRINT);
    eat(l, current_token, TOKEN_PRINT);
    node->right = parse_factor(l, current_token);
    return node;
}

ASTNode* parse_var(Lexer *l, Token *current_token) {
    ASTNode* node = create_node(NODE_VAR_DECL);
    eat(l, current_token, TOKEN_VAR);
    node->left = parse_factor(l, current_token);
    if (current_token->kind == TOKEN_NUMBER || current_token->kind == TOKEN_IDENT) {
        node->right = parse_factor(l, current_token);
    }
    return node;
}

ASTNode* parse_exit(Lexer *l, Token *current_token) {
    ASTNode* node = create_node(NODE_EXIT);
    eat(l, current_token, TOKEN_EXIT);
    return node;
}

ASTNode* parse_add(Lexer *l, Token *current_token) {
    ASTNode* node = create_node(NODE_ARITHMETIC);
        
    eat(l, current_token, TOKEN_ADD);

    node->left = parse_factor(l, current_token); // ele vai adicionar como 1 argumento
    node->op_kind = TOKEN_ADD; // !!!!!! TEM QUE MUDAR O OP KIND!!!!!
    if (current_token->kind == TOKEN_NUMBER || current_token->kind == TOKEN_IDENT) {
        node->right = parse_factor(l, current_token); // adiciona como segundo 10
    }

    return node;
}

ASTNode* parse_sub(Lexer *l, Token *current_token) {
    ASTNode* node = create_node(NODE_ARITHMETIC);
        
    eat(l, current_token, TOKEN_SUB);

    node->left = parse_factor(l, current_token); // ele vai adicionar como 1 argumento
    node->op_kind = TOKEN_SUB; // !!!!!! TEM QUE MUDAR O OP KIND!!!!!
    if (current_token->kind == TOKEN_NUMBER || current_token->kind == TOKEN_IDENT) {
        node->right = parse_factor(l, current_token); // adiciona como segundo 10
    }

    return node;
}

ASTNode* parse_inc(Lexer *l, Token *current_token) {
    ASTNode* node = create_node(NODE_INC);
    eat(l, current_token, TOKEN_INC);
    node->left = parse_factor(l, current_token);
    return node;
}

ASTNode* parse_dec(Lexer *l, Token *current_token) {
    ASTNode* node = create_node(NODE_DEC);
    eat(l, current_token, TOKEN_DEC);
    node->left = parse_factor(l, current_token);
    return node;
}

ASTNode* parse_debug(Lexer *l, Token *current_token) {
    ASTNode* node = create_node(NODE_DEBUG);
    eat(l, current_token, TOKEN_DEBUG);
    return node;
}

ASTNode* parse_nodebug(Lexer *l, Token *current_token) {
    ASTNode* node = create_node(NODE_NODEBUG);
    eat(l, current_token, TOKEN_NODEBUG);
    return node;
}

ASTNode* parse_mark(Lexer *l, Token *current_token) {
    ASTNode* node = create_node(NODE_MARK);
    eat(l, current_token, TOKEN_MARK);
    node->left = parse_factor(l, current_token);
    return node;
}

ASTNode* parse_jump(Lexer *l, Token *current_token) {
    ASTNode* node = create_node(NODE_JUMP);
    eat(l, current_token, TOKEN_JUMP);
    node->left = parse_factor(l, current_token);
    return node;
}

ASTNode* parse_if(Lexer *l, Token *current_token) {
    ASTNode* node = create_node(NODE_IF);
    eat(l, current_token, TOKEN_IF);
    node->left = parse_factor(l, current_token);
    node->right = parse_factor(l, current_token);
    return node;
}

ASTNode* parse_eq(Lexer *l, Token *current_token) {
    ASTNode* node = create_node(NODE_EQ);
    eat(l, current_token, TOKEN_EQ);
    node->left = parse_factor(l, current_token);
    node->right = parse_factor(l, current_token);
    return node;
}

ASTNode* parse_lt(Lexer *l, Token *current_token) {
    ASTNode* node = create_node(NODE_LT);
    eat(l, current_token, TOKEN_LT);
    node->left = parse_factor(l, current_token);
    node->right = parse_factor(l, current_token);
    return node;
}

ASTNode* parse_gt(Lexer *l, Token *current_token) {
    ASTNode* node = create_node(NODE_GT);
    eat(l, current_token, TOKEN_GT);
    node->left = parse_factor(l, current_token);
    node->right = parse_factor(l, current_token);
    return node;
}

ASTNode* parse_read(Lexer *l, Token *current_token) {
    ASTNode* node = create_node(NODE_READ);
    eat(l, current_token, TOKEN_READ);
    node->left = parse_factor(l, current_token); // variavel onde guarda o input
    return node;
}


// parse statement LETS FUCKING GO BITCHES

ASTNode* parse_statement(Lexer *l, Token *current_token) {
    printf("[DEBUG] TOKEN_PRINT vale: %d\n", TOKEN_PRINT);
    printf("[DEBUG] current kind vale: %d\n", current_token->kind);
    /*===== OS BASICOS =====*/

    if      (current_token->kind == TOKEN_PRINT) return parse_print(l, current_token);
    else if (current_token->kind == TOKEN_VAR)   return parse_var(l, current_token);
    else if (current_token->kind == TOKEN_EXIT)   return parse_exit(l, current_token);


    /*===== OPERACOES ARITMETICA =====*/
    else if (current_token->kind == TOKEN_ADD)   return parse_add(l, current_token);
    else if (current_token->kind == TOKEN_SUB)   return parse_sub(l, current_token);

    else if (current_token->kind == TOKEN_INC)   return parse_inc(l, current_token);

    else if (current_token->kind == TOKEN_DEC)   return parse_dec(l, current_token);

    // ===== system =====
    else if (current_token->kind == TOKEN_DEBUG) return parse_debug(l, current_token);

    else if (current_token->kind == TOKEN_NODEBUG) return parse_nodebug(l, current_token);

    // LABELS
    else if (current_token->kind == TOKEN_MARK) return parse_mark(l, current_token);

    else if (current_token->kind == TOKEN_JUMP) return parse_jump(l, current_token);

    // CONDITIONS
    else if (current_token->kind == TOKEN_IF) return parse_if(l, current_token);
    
    else if (current_token->kind == TOKEN_EQ) return parse_eq(l, current_token);

    else if (current_token->kind == TOKEN_LT) return parse_lt(l, current_token);

    else if (current_token->kind == TOKEN_GT) return parse_gt(l, current_token);

    // I/O

    else if (current_token->kind == TOKEN_READ) return parse_read(l, current_token);
        
    // ! tem mais pra fazer so que eu vou deixar pra depois
    // ! por limitações que eu to sem saco pra fazer
    // ! por agora vamos dar esse erro so por finalidades de debug
    fprintf(stderr, "Comando nao reconhecido no parse statement\n");
    exit(1);
}

// Deus é pai todo poderoso criador dos ceus e da terra me ajude neste momento

ASTNode* parse_program(Lexer *l) {
    Token current;
    advance(l, &current); // pega o primeiro token pra dar tranco no carro


    ASTNode* root = create_node(NODE_PROGRAM); // ja cria as nossas raizes da arovore abstrata
    ASTNode* current_pos = root;

    while (current.kind != TOKEN_EOF) // enquanto nao for o fim do arquivo contnua rodando
    {
        current_pos->left = parse_statement(l, &current); // deixa o comando atual pra esquerda

        if (current.kind != TOKEN_EOF) // se ainda n for o fim do arquivo cria outro nó pra continuar
        {
            current_pos->right = create_node(NODE_PROGRAM);
            current_pos = current_pos->right;
        }
        
    }
    return root;
}