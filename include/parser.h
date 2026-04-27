#ifndef PARSER_H
#define PARSER_H

#include "ast.h"
#include "lexer.h"

// A função principal que o interpretador vai chamar para começar tudo
ASTNode* parse_program(Lexer* l);
ASTNode* parse_expression(Lexer* l, Token* current);

// pra ver os errinho se caso der
void parser_error(const char* message);

#endif