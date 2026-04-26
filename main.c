#include "lexer.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
    // Um código de exemplo na sua linguagem AXIS
    const char *codigo_teste = "ADD X 10";

    // Inicializa seu Lexer (ajuste conforme o nome da sua struct/função)
    Lexer l;
    l.src = codigo_teste;
    l.i = 0;
    l.line = 1;

    printf("--- INICIANDO LEXER DA AXIS ---\n\n");

    Token t;
    do {
        t = next_token(&l);
        
        // Printa o tipo do token e o valor que o malloc guardou
        printf("Linha %d | Tipo: %d | Valor: [%s]\n", t.line, t.kind, t.value);

        // OBRIGATÓRIO: Como a gente usou malloc no make_token, 
        // temos que dar free aqui para não vazar memória!
        free(t.value); 

    } while (t.kind != TOKEN_EOF);

    printf("\n--- TESTE FINALIZADO COM SUCESSO ---\n");
    return 0;
}