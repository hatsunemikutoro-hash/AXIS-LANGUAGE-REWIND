#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "ast.h"
#include <stdint.h>
#include <setjmp.h>

// ─── variáveis ───────────────────────────────────────────────────────────────

typedef struct {
    char    *name;
    int64_t  value;
} Variable;

typedef struct {
    Variable *vars;
    int       count;
    int       capacity;
} Env;

// ─── funções ─────────────────────────────────────────────────────────────────

typedef struct {
    char    *name;
    ASTNode *node; // o NODE_FUNC_DEF inteiro
} Function;

typedef struct {
    Function *funcs;
    int       count;
    int       capacity;
} FuncTable;

// ─── contexto de execução ────────────────────────────────────────────────────
// Agrupa env + functable + mecanismo de return num só lugar

typedef struct {
    Env       *env;
    FuncTable *ft;
    jmp_buf    return_jump;   // para o RETURN escapar do bloco
    int64_t    return_value;  // valor que o RETURN carrega
    int        returning;     // flag: estamos em processo de return?
} ExecCtx;

// ─── interface pública ───────────────────────────────────────────────────────

Env*       env_create(void);
void       env_set(Env *env, const char *name, int64_t value);
int64_t    env_get(Env *env, const char *name);
void       env_free(Env *env);

FuncTable* functable_create(void);
void       functable_register(FuncTable *ft, const char *name, ASTNode *node);
Function*  functable_get(FuncTable *ft, const char *name);
void       functable_free(FuncTable *ft);

int64_t    eval(ASTNode *node, ExecCtx *ctx);
void       interpret_node(ASTNode *node, ExecCtx *ctx);
void       interpret(ASTNode *program);

#endif