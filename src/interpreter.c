#include "ast.h"
#include "interpreter.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <setjmp.h>

// ─── Env ─────────────────────────────────────────────────────────────────────

Env* env_create(void) {
    Env *env = malloc(sizeof(Env));
    env->count    = 0;
    env->capacity = 8;
    env->vars     = malloc(sizeof(Variable) * env->capacity);
    return env;
}

void env_set(Env *env, const char *name, int64_t value) {
    // atualiza se já existe
    for (int i = 0; i < env->count; i++) {
        if (strcmp(env->vars[i].name, name) == 0) {
            env->vars[i].value = value;
            return;
        }
    }
    // cresce se necessário
    if (env->count >= env->capacity) {
        env->capacity *= 2;
        env->vars = realloc(env->vars, sizeof(Variable) * env->capacity);
    }
    env->vars[env->count].name  = strdup(name);
    env->vars[env->count].value = value;
    env->count++;
}

int64_t env_get(Env *env, const char *name) {
    for (int i = 0; i < env->count; i++)
        if (strcmp(env->vars[i].name, name) == 0)
            return env->vars[i].value;
    fprintf(stderr, "Variavel nao definida: '%s'\n", name);
    exit(1);
}

void env_free(Env *env) {
    for (int i = 0; i < env->count; i++)
        free(env->vars[i].name);
    free(env->vars);
    free(env);
}

// ─── FuncTable ───────────────────────────────────────────────────────────────

FuncTable* functable_create(void) {
    FuncTable *ft = malloc(sizeof(FuncTable));
    ft->count    = 0;
    ft->capacity = 8;
    ft->funcs    = malloc(sizeof(Function) * ft->capacity);
    return ft;
}

void functable_register(FuncTable *ft, const char *name, ASTNode *node) {
    for (int i = 0; i < ft->count; i++) {
        if (strcmp(ft->funcs[i].name, name) == 0) {
            ft->funcs[i].node = node;
            return;
        }
    }
    if (ft->count >= ft->capacity) {
        ft->capacity *= 2;
        ft->funcs = realloc(ft->funcs, sizeof(Function) * ft->capacity);
    }
    ft->funcs[ft->count].name = strdup(name);
    ft->funcs[ft->count].node = node;
    ft->count++;
}

Function* functable_get(FuncTable *ft, const char *name) {
    for (int i = 0; i < ft->count; i++)
        if (strcmp(ft->funcs[i].name, name) == 0)
            return &ft->funcs[i];
    fprintf(stderr, "Funcao nao encontrada: '%s'\n", name);
    exit(1);
}

void functable_free(FuncTable *ft) {
    for (int i = 0; i < ft->count; i++)
        free(ft->funcs[i].name);
    free(ft->funcs);
    free(ft);
}

// ─── eval ────────────────────────────────────────────────────────────────────
// Resolve um node para um valor int64

int64_t eval(ASTNode *node, ExecCtx *ctx) {
    if (!node) return 0;

    switch (node->kind) {
        case NODE_NUMBER:
            return node->number;

        case NODE_IDENT:
            return env_get(ctx->env, node->value);

        case NODE_ARITH: {
            // left e right são os operandos
            int64_t a = eval(node->left,  ctx);
            int64_t b = eval(node->right, ctx);
            char   *op = node->value; // "add", "sub", "mul", "div"

            if (strcmp(op, "add") == 0) return a + b;
            if (strcmp(op, "sub") == 0) return a - b;
            if (strcmp(op, "mul") == 0) return a * b;
            if (strcmp(op, "div") == 0) {
                if (b == 0) { fprintf(stderr, "Divisao por zero\n"); exit(1); }
                return a / b;
            }
            fprintf(stderr, "Operacao desconhecida: '%s'\n", op);
            exit(1);
        }

        default:
            fprintf(stderr, "Node invalido em eval (kind=%d)\n", node->kind);
            exit(1);
    }
}

// ─── interpret_node ──────────────────────────────────────────────────────────

void interpret_node(ASTNode *node, ExecCtx *ctx) {
    if (!node) return;
    // se estamos retornando, não executa mais nada no bloco atual
    if (ctx->returning) return;

    switch (node->kind) {

        case NODE_PROGRAM:
        case NODE_BLOCK: {
            for (int i = 0; i < node->child_count; i++) {
                interpret_node(node->children[i], ctx);
                if (ctx->returning) return; // para o bloco imediatamente
            }
            break;
        }

        // store x 10  →  left=nome, right=valor
        case NODE_STORE: {
            int64_t val = eval(node->right, ctx);
            env_set(ctx->env, node->left->value, val);
            break;
        }

        // print x  →  left=valor
        case NODE_PRINT: {
            int64_t val = eval(node->left, ctx);
            printf("%lld\n", (long long)val);
            break;
        }

        // add a b -> result  →  left=a, right=b, children[0]=dest
        case NODE_ARITH: {
            int64_t result = eval(node, ctx);
            // destino é opcional (children[0] se existir)
            if (node->child_count > 0) {
                ASTNode *dest = node->children[0];
                if (dest->kind != NODE_IDENT) {
                    fprintf(stderr, "Destino de arith invalido\n");
                    exit(1);
                }
                env_set(ctx->env, dest->value, result);
            }
            break;
        }

        // function soma a b { corpo }
        // left=nome, children[]=params, right=bloco
        case NODE_FUNC_DEF: {
            functable_register(ctx->ft, node->left->value, node);
            break;
        }

        // call soma x y -> z
        // left=nome, children[]=args, right=dest (opcional)
        case NODE_CALL: {
            Function *fn = functable_get(ctx->ft, node->left->value);
            ASTNode  *fn_node = fn->node;

            // fn_node->left       = nome
            // fn_node->children[] = params (NODE_IDENT)
            // fn_node->right      = corpo (NODE_BLOCK)

            int param_count = fn_node->child_count;
            int arg_count   = node->child_count;

            if (param_count != arg_count) {
                fprintf(stderr, "Funcao '%s' espera %d args, recebeu %d\n",
                        fn->name, param_count, arg_count);
                exit(1);
            }

            // cria escopo novo para a função
            Env *fn_env = env_create();
            for (int i = 0; i < param_count; i++) {
                const char *param_name = fn_node->children[i]->value;
                int64_t     arg_val    = eval(node->children[i], ctx);
                env_set(fn_env, param_name, arg_val);
            }

            // executa o corpo no escopo novo
            ExecCtx fn_ctx;
            fn_ctx.env        = fn_env;
            fn_ctx.ft         = ctx->ft;  // mesma tabela de funções
            fn_ctx.returning  = 0;
            fn_ctx.return_value = 0;

            // setjmp: o RETURN vai fazer longjmp pra cá
            if (setjmp(fn_ctx.return_jump) == 0) {
                interpret_node(fn_node->right, &fn_ctx);
            }

            int64_t ret_val = fn_ctx.return_value;
            env_free(fn_env);

            // guarda resultado no destino se tiver ->
            if (node->right) {
                env_set(ctx->env, node->right->value, ret_val);
            }
            break;
        }

        // if cond \n bloco end
        // left=cond, right=bloco
        case NODE_IF: {
            int64_t cond = eval(node->left, ctx);
            if (cond) {
                interpret_node(node->right, ctx);
            }
            break;
        }

        // return valor  →  left=valor
        case NODE_RETURN: {
            ctx->return_value = eval(node->left, ctx);
            ctx->returning    = 1;
            longjmp(ctx->return_jump, 1);
            break;
        }

        default:
            fprintf(stderr, "Node nao tratado: %d\n", node->kind);
            exit(1);
    }
}

// ─── entrada pública ─────────────────────────────────────────────────────────

void interpret(ASTNode *program) {
    ExecCtx ctx;
    ctx.env          = env_create();
    ctx.ft           = functable_create();
    ctx.returning    = 0;
    ctx.return_value = 0;

    interpret_node(program, &ctx);

    env_free(ctx.env);
    functable_free(ctx.ft);
}