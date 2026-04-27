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

// parse statement LETS FUCKING GO BITCHES

ASTNode* parse_statement(Lexer *l, Token *current_token) {
    printf("[DEBUG] TOKEN_PRINT vale: %d\n", TOKEN_PRINT);
    printf("[DEBUG] current kind vale: %d\n", current_token->kind);
    if (current_token->kind == TOKEN_PRINT) { // verifica se a kind (tipo) é igual ao token print
        ASTNode* node = create_node(NODE_PRINT); // cria um node dele

        eat(l, current_token, TOKEN_PRINT); // come ele (la eli huhuhuhu)

        node->right = parse_expression(l, current_token); // avança pra proxima uma variavel um numero fodase

        return node;
    }
    
    // por agora vamos dar esse erro so por finalidades de debug
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