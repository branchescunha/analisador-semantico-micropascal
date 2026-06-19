#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <strings.h>

// ============================================================================
// ANALISADOR LÉXICO (SCANNER)
// ============================================================================

typedef enum {
    TOKEN_PROGRAM,
    TOKEN_VAR,
    TOKEN_INTEGER,
    TOKEN_REAL,
    TOKEN_BEGIN,
    TOKEN_END,
    TOKEN_IF,
    TOKEN_THEN,
    TOKEN_ELSE,
    TOKEN_WHILE,
    TOKEN_DO,
    TOKEN_ID,
    TOKEN_NUM_INT,
    TOKEN_NUM_REAL,
    TOKEN_OP_EQ,   // =
    TOKEN_OP_NE,   // <>
    TOKEN_OP_LT,   // <
    TOKEN_OP_LE,   // <=
    TOKEN_OP_GT,   // >
    TOKEN_OP_GE,   // >=
    TOKEN_OP_AD,   // +
    TOKEN_OP_MIN,  // -
    TOKEN_OP_MUL,  // *
    TOKEN_OP_DIV,  // /
    TOKEN_OP_ASS,  // :=
    TOKEN_SMB_SEM, // ;
    TOKEN_SMB_COM, // ,
    TOKEN_SMB_OPA, // (
    TOKEN_SMB_CPA, // )
    TOKEN_SMB_COL, // :
    TOKEN_SMB_DOT, // .
    TOKEN_FIM,
    TOKEN_ERRO
} TipoToken;

typedef struct {
    TipoToken tipo;
    char *lexema;
    int linha;
    int coluna;
} Token;

typedef struct {
    const char *src;
    int i;
    int linha;
    int coluna;
    char c;
} Scanner;

const char *nome_token(TipoToken t) {
    switch (t) {
        case TOKEN_PROGRAM:   return "program";
        case TOKEN_VAR:       return "var";
        case TOKEN_INTEGER:   return "integer";
        case TOKEN_REAL:      return "real";
        case TOKEN_BEGIN:     return "begin";
        case TOKEN_END:       return "end";
        case TOKEN_IF:        return "if";
        case TOKEN_THEN:      return "then";
        case TOKEN_ELSE:      return "else";
        case TOKEN_WHILE:     return "while";
        case TOKEN_DO:        return "do";
        case TOKEN_ID:        return "identificador";
        case TOKEN_NUM_INT:   return "numero inteiro";
        case TOKEN_NUM_REAL:  return "numero real";
        case TOKEN_OP_EQ:     return "=";
        case TOKEN_OP_NE:     return "<>";
        case TOKEN_OP_LT:     return "<";
        case TOKEN_OP_LE:     return "<=";
        case TOKEN_OP_GT:     return ">";
        case TOKEN_OP_GE:     return ">=";
        case TOKEN_OP_AD:     return "+";
        case TOKEN_OP_MIN:    return "-";
        case TOKEN_OP_MUL:    return "*";
        case TOKEN_OP_DIV:    return "/";
        case TOKEN_OP_ASS:    return ":=";
        case TOKEN_SMB_SEM:   return ";";
        case TOKEN_SMB_COM:   return ",";
        case TOKEN_SMB_OPA:   return "(";
        case TOKEN_SMB_CPA:   return ")";
        case TOKEN_SMB_COL:   return ":";
        case TOKEN_SMB_DOT:   return ".";
        case TOKEN_FIM:       return "fim de arquivo";
        case TOKEN_ERRO:      return "erro lexico";
        default:              return "?";
    }
}

char *str_ndup(const char *s, size_t n) {
    char *p = (char*) malloc(n + 1);
    if (!p) {
        fprintf(stderr, "Memoria insuficiente\n");
        exit(1);
    }
    memcpy(p, s, n);
    p[n] = '\0';
    return p;
}

void iniciar(Scanner *sc, const char *texto) {
    sc->src = texto ? texto : "";
    sc->i = 0;
    sc->linha = 1;
    sc->coluna = 1;
    sc->c = sc->src[0];
}

void avancar(Scanner *sc) {
    if (sc->c == '\0') return;
    if (sc->c == '\n') {
        sc->linha++;
        sc->coluna = 1;
    } else {
        sc->coluna++;
    }
    sc->i++;
    sc->c = sc->src[sc->i];
}

Token criar_token_texto(Scanner *sc, TipoToken tipo, const char *ini, size_t n, int lin, int col) {
    (void) sc;
    Token t;
    t.tipo = tipo;
    t.lexema = str_ndup(ini, n);
    t.linha = lin;
    t.coluna = col;
    return t;
}

Token obter_token(Scanner *sc) {
    while (sc->c != '\0') {
        // Ignorar espacos em branco
        if (isspace((unsigned char) sc->c)) {
            avancar(sc);
            continue;
        }

        // Ignorar comentarios delimitados por chaves { ... }
        if (sc->c == '{') {
            int lin_inicio = sc->linha;
            int col_inicio = sc->coluna;
            avancar(sc);
            while (sc->c != '\0' && sc->c != '}') {
                avancar(sc);
            }
            if (sc->c == '\0') {
                return criar_token_texto(sc, TOKEN_ERRO, "{", 1, lin_inicio, col_inicio);
            }
            avancar(sc); // Consome '}'
            continue;
        }

        int lin = sc->linha;
        int col = sc->coluna;

        // Identificadores e Palavras Reservadas (Suporta underscore '_')
        if (isalpha((unsigned char) sc->c) || sc->c == '_') {
            int ini = sc->i;
            while (isalnum((unsigned char) sc->c) || sc->c == '_') {
                avancar(sc);
            }
            int tam = sc->i - ini;
            char *lexema = str_ndup(sc->src + ini, tam);

            TipoToken tipo = TOKEN_ID;
            if (strcasecmp(lexema, "program") == 0) tipo = TOKEN_PROGRAM;
            else if (strcasecmp(lexema, "var") == 0) tipo = TOKEN_VAR;
            else if (strcasecmp(lexema, "integer") == 0) tipo = TOKEN_INTEGER;
            else if (strcasecmp(lexema, "real") == 0) tipo = TOKEN_REAL;
            else if (strcasecmp(lexema, "begin") == 0) tipo = TOKEN_BEGIN;
            else if (strcasecmp(lexema, "end") == 0) tipo = TOKEN_END;
            else if (strcasecmp(lexema, "if") == 0) tipo = TOKEN_IF;
            else if (strcasecmp(lexema, "then") == 0) tipo = TOKEN_THEN;
            else if (strcasecmp(lexema, "else") == 0) tipo = TOKEN_ELSE;
            else if (strcasecmp(lexema, "while") == 0) tipo = TOKEN_WHILE;
            else if (strcasecmp(lexema, "do") == 0) tipo = TOKEN_DO;

            Token t = {tipo, lexema, lin, col};
            return t;
        }

        // Numeros (Inteiros e Reais)
        if (isdigit((unsigned char) sc->c)) {
            int ini = sc->i;
            while (isdigit((unsigned char) sc->c)) {
                avancar(sc);
            }

            TipoToken tipo = TOKEN_NUM_INT;
            if (sc->c == '.') {
                // Se o proximo for digito, e um numero real valido
                if (isdigit((unsigned char) sc->src[sc->i + 1])) {
                    tipo = TOKEN_NUM_REAL;
                    avancar(sc); // consome '.'
                    while (isdigit((unsigned char) sc->c)) {
                        avancar(sc);
                    }
                    // Opcional: expoente e/E
                    if (sc->c == 'e' || sc->c == 'E') {
                        avancar(sc); // consome e/E
                        if (sc->c == '+' || sc->c == '-') {
                            avancar(sc);
                        }
                        if (isdigit((unsigned char) sc->c)) {
                            while (isdigit((unsigned char) sc->c)) {
                                avancar(sc);
                            }
                        } else {
                            // Expoente malformado
                            char *lexema = str_ndup(sc->src + ini, sc->i - ini);
                            Token t = {TOKEN_ERRO, lexema, lin, col};
                            return t;
                        }
                    }
                } else {
                    // Ponto imediatamente após digito sem digitos depois (ex: 123.)
                    avancar(sc);
                    int tam = sc->i - ini;
                    char *lexema = str_ndup(sc->src + ini, tam);
                    Token t = {TOKEN_ERRO, lexema, lin, col};
                    return t;
                }
            }

            int tam = sc->i - ini;
            char *lexema = str_ndup(sc->src + ini, tam);
            Token t = {tipo, lexema, lin, col};
            return t;
        }

        // Operadores e Delimitadores Compostos (:=, <=, <>, >=)
        if (sc->c == ':') {
            avancar(sc);
            if (sc->c == '=') {
                avancar(sc);
                Token t = {TOKEN_OP_ASS, str_ndup(":=", 2), lin, col};
                return t;
            } else {
                Token t = {TOKEN_SMB_COL, str_ndup(":", 1), lin, col};
                return t;
            }
        }

        if (sc->c == '<') {
            avancar(sc);
            if (sc->c == '=') {
                avancar(sc);
                Token t = {TOKEN_OP_LE, str_ndup("<=", 2), lin, col};
                return t;
            } else if (sc->c == '>') {
                avancar(sc);
                Token t = {TOKEN_OP_NE, str_ndup("<>", 2), lin, col};
                return t;
            } else {
                Token t = {TOKEN_OP_LT, str_ndup("<", 1), lin, col};
                return t;
            }
        }

        if (sc->c == '>') {
            avancar(sc);
            if (sc->c == '=') {
                avancar(sc);
                Token t = {TOKEN_OP_GE, str_ndup(">=", 2), lin, col};
                return t;
            } else {
                Token t = {TOKEN_OP_GT, str_ndup(">", 1), lin, col};
                return t;
            }
        }

        // Operadores e Delimitadores Simples
        switch (sc->c) {
            case '=': {
                avancar(sc);
                Token t = {TOKEN_OP_EQ, str_ndup("=", 1), lin, col};
                return t;
            }
            case '+': {
                avancar(sc);
                Token t = {TOKEN_OP_AD, str_ndup("+", 1), lin, col};
                return t;
            }
            case '-': {
                avancar(sc);
                Token t = {TOKEN_OP_MIN, str_ndup("-", 1), lin, col};
                return t;
            }
            case '*': {
                avancar(sc);
                Token t = {TOKEN_OP_MUL, str_ndup("*", 1), lin, col};
                return t;
            }
            case '/': {
                avancar(sc);
                Token t = {TOKEN_OP_DIV, str_ndup("/", 1), lin, col};
                return t;
            }
            case ';': {
                avancar(sc);
                Token t = {TOKEN_SMB_SEM, str_ndup(";", 1), lin, col};
                return t;
            }
            case ',': {
                avancar(sc);
                Token t = {TOKEN_SMB_COM, str_ndup(",", 1), lin, col};
                return t;
            }
            case '(': {
                avancar(sc);
                Token t = {TOKEN_SMB_OPA, str_ndup("(", 1), lin, col};
                return t;
            }
            case ')': {
                avancar(sc);
                Token t = {TOKEN_SMB_CPA, str_ndup(")", 1), lin, col};
                return t;
            }
            case '.': {
                avancar(sc);
                Token t = {TOKEN_SMB_DOT, str_ndup(".", 1), lin, col};
                return t;
            }
            default: {
                char msg[2] = {sc->c, '\0'};
                avancar(sc);
                Token t = {TOKEN_ERRO, str_ndup(msg, 1), lin, col};
                return t;
            }
        }
    }

    Token t = {TOKEN_FIM, str_ndup("", 0), sc->linha, sc->coluna};
    return t;
}

// ============================================================================
// ANALISADOR SEMÂNTICO (TABELA DE SÍMBOLOS)
// ============================================================================

typedef enum {
    TYPE_INT,
    TYPE_REAL,
    TYPE_BOOL,
    TYPE_VOID
} TipoDado;

const char *nome_tipo(TipoDado t) {
    switch (t) {
        case TYPE_INT:  return "INTEIRO";
        case TYPE_REAL: return "REAL";
        case TYPE_BOOL: return "BOOLEANO";
        default:        return "VOID (ERRO)";
    }
}

typedef struct Simbolo {
    char nome[100];
    TipoDado tipo;
    struct Simbolo *prox;
} Simbolo;

Simbolo *tabela_simbolos = NULL;

void liberar_tabela() {
    Simbolo *atual = tabela_simbolos;
    while (atual != NULL) {
        Simbolo *aux = atual->prox;
        free(atual);
        atual = aux;
    }
    tabela_simbolos = NULL;
}

Token tok;
Scanner scanner;

void erro_lexico() {
    printf("%d:erro lexico [%s]\n", tok.linha, tok.lexema);
    liberar_tabela();
    if (tok.lexema) free(tok.lexema);
    exit(1);
}

void erro_sintatico() {
    if (tok.tipo == TOKEN_FIM) {
        printf("%d:fim de arquivo nao esperado\n", tok.linha);
    } else {
        printf("%d:token nao esperado [%s]\n", tok.linha, tok.lexema);
    }
    liberar_tabela();
    if (tok.lexema) free(tok.lexema);
    exit(1);
}

void inserir_simbolo(const char *nome, TipoDado tipo, int linha, int coluna) {
    Simbolo *atual = tabela_simbolos;
    while (atual != NULL) {
        if (strcasecmp(atual->nome, nome) == 0) {
            printf("Erro semantico na linha %d, coluna %d: Variavel '%s' ja declarada.\n",
                linha, coluna, nome);
            liberar_tabela();
            if (tok.lexema) free(tok.lexema);
            exit(1);
        }
        atual = atual->prox;
    }

    Simbolo *novo = (Simbolo *) malloc(sizeof(Simbolo));
    strncpy(novo->nome, nome, sizeof(novo->nome) - 1);
    novo->nome[sizeof(novo->nome) - 1] = '\0';
    novo->tipo = tipo;
    novo->prox = tabela_simbolos;
    tabela_simbolos = novo;
}

TipoDado buscar_simbolo(const char *nome, int linha, int coluna) {
    Simbolo *atual = tabela_simbolos;
    while (atual != NULL) {
        if (strcasecmp(atual->nome, nome) == 0) {
            return atual->tipo;
        }
        atual = atual->prox;
    }
    printf("Erro semantico na linha %d, coluna %d: Variavel '%s' nao declarada.\n",
        linha, coluna, nome);
    liberar_tabela();
    if (tok.lexema) free(tok.lexema);
    exit(1);
}

// ============================================================================
// ESTRUTURA E CRIAÇÃO DA AST (ÁRVORE SINTÁTICA ABSTRATA)
// ============================================================================

typedef enum {
    AST_PROGRAMA,
    AST_BLOCO,
    AST_DECLARACOES,
    AST_DECLARACAO,
    AST_VARIAVEL,
    AST_NUM_INT,
    AST_NUM_REAL,
    AST_ATRIBUICAO,
    AST_IF,
    AST_WHILE,
    AST_SOMA,
    AST_SUBTRACAO,
    AST_MULTIPLICACAO,
    AST_DIVISAO,
    AST_MAIOR,
    AST_MENOR,
    AST_MAIOR_IGUAL,
    AST_MENOR_IGUAL,
    AST_IGUAL,
    AST_DIFERENTE,
    AST_COMANDOS
} TipoAST;

typedef struct AST {
    TipoAST tipo;
    char valor[100];
    struct AST *esq;
    struct AST *dir;
    struct AST *centro;
    struct AST *prox;
    TipoDado tipo_dado;
} AST;

AST *criar_no(TipoAST tipo, const char *valor, AST *esq, AST *dir, AST *centro) {
    AST *novo = (AST *) malloc(sizeof(AST));
    novo->tipo = tipo;
    if (valor) {
        strncpy(novo->valor, valor, sizeof(novo->valor) - 1);
        novo->valor[sizeof(novo->valor) - 1] = '\0';
    } else {
        novo->valor[0] = '\0';
    }
    novo->esq = esq;
    novo->dir = dir;
    novo->centro = centro;
    novo->prox = NULL;
    novo->tipo_dado = TYPE_VOID;
    return novo;
}

void liberar_ast(AST *raiz) {
    if (raiz == NULL) return;
    liberar_ast(raiz->esq);
    liberar_ast(raiz->centro);
    liberar_ast(raiz->dir);
    liberar_ast(raiz->prox);
    free(raiz);
}

const char *nome_no_ast(TipoAST t) {
    switch (t) {
        case AST_PROGRAMA:      return "PROGRAMA";
        case AST_BLOCO:         return "BLOCO";
        case AST_DECLARACOES:   return "DECLARACOES";
        case AST_DECLARACAO:    return "DECLARACAO";
        case AST_VARIAVEL:      return "VARIAVEL";
        case AST_NUM_INT:       return "NUM_INTEIRO";
        case AST_NUM_REAL:      return "NUM_REAL";
        case AST_ATRIBUICAO:    return ":= (ATRIBUICAO)";
        case AST_IF:            return "IF";
        case AST_WHILE:         return "WHILE";
        case AST_SOMA:          return "+ (SOMA)";
        case AST_SUBTRACAO:     return "- (SUBTRACAO)";
        case AST_MULTIPLICACAO: return "* (MULTIPLICACAO)";
        case AST_DIVISAO:       return "/ (DIVISAO)";
        case AST_MAIOR:         return "> (MAIOR)";
        case AST_MENOR:         return "< (MENOR)";
        case AST_MAIOR_IGUAL:   return ">= (MAIOR_IGUAL)";
        case AST_MENOR_IGUAL:   return "<= (MENOR_IGUAL)";
        case AST_IGUAL:         return "= (IGUAL)";
        case AST_DIFERENTE:     return "<> (DIFERENTE)";
        case AST_COMANDOS:      return "SEQUENCIA DE COMANDOS";
        default:                return "NO DESCONHECIDO";
    }
}

void gerar_dot_nos(AST *raiz, FILE *f) {
    if (raiz == NULL) return;

    char label[200] = "";
    if (raiz->tipo == AST_VARIAVEL) {
        snprintf(label, sizeof(label), "%s\\n(%s)\\n[Tipo: %s]", nome_no_ast(raiz->tipo), raiz->valor, nome_tipo(raiz->tipo_dado));
    } else if (raiz->tipo == AST_PROGRAMA || raiz->tipo == AST_DECLARACAO || raiz->tipo == AST_NUM_INT || raiz->tipo == AST_NUM_REAL) {
        snprintf(label, sizeof(label), "%s\\n(%s)", nome_no_ast(raiz->tipo), raiz->valor);
    } else {
        snprintf(label, sizeof(label), "%s", nome_no_ast(raiz->tipo));
    }

    const char *shape = "box";
    const char *fillcolor = "lightblue";
    if (raiz->tipo == AST_VARIAVEL) {
        shape = "ellipse";
        fillcolor = "lightgreen";
    } else if (raiz->tipo == AST_NUM_INT || raiz->tipo == AST_NUM_REAL) {
        shape = "ellipse";
        fillcolor = "lightyellow";
    } else if (raiz->tipo == AST_ATRIBUICAO || raiz->tipo == AST_IF || raiz->tipo == AST_WHILE) {
        shape = "hexagon";
        fillcolor = "lightpink";
    }

    fprintf(f, "    n%p [label=\"%s\", shape=%s, style=filled, fillcolor=%s];\n", (void*)raiz, label, shape, fillcolor);

    gerar_dot_nos(raiz->esq, f);
    gerar_dot_nos(raiz->centro, f);
    gerar_dot_nos(raiz->dir, f);
    gerar_dot_nos(raiz->prox, f);
}

void gerar_dot_arestas(AST *raiz, FILE *f) {
    if (raiz == NULL) return;

    if (raiz->esq) {
        fprintf(f, "    n%p -> n%p [label=\"esq\"];\n", (void*)raiz, (void*)raiz->esq);
        gerar_dot_arestas(raiz->esq, f);
    }
    if (raiz->centro) {
        fprintf(f, "    n%p -> n%p [label=\"centro\"];\n", (void*)raiz, (void*)raiz->centro);
        gerar_dot_arestas(raiz->centro, f);
    }
    if (raiz->dir) {
        fprintf(f, "    n%p -> n%p [label=\"dir\"];\n", (void*)raiz, (void*)raiz->dir);
        gerar_dot_arestas(raiz->dir, f);
    }
    if (raiz->prox) {
        fprintf(f, "    n%p -> n%p [style=dashed, constraint=false, label=\"prox\"];\n", (void*)raiz, (void*)raiz->prox);
        gerar_dot_arestas(raiz->prox, f);
    }
}

void exportar_ast_dot(AST *raiz, const char *nome_arquivo) {
    FILE *f = fopen(nome_arquivo, "w");
    if (!f) {
        printf("Erro ao criar arquivo DOT '%s'\n", nome_arquivo);
        return;
    }

    fprintf(f, "digraph AST {\n");
    fprintf(f, "    node [fontname=\"Arial\"];\n");
    fprintf(f, "    edge [fontname=\"Arial\"];\n");
    fprintf(f, "    label=\"Abstract Syntax Tree (AST)\";\n");
    fprintf(f, "    labelloc=\"t\";\n");
    fprintf(f, "    fontsize=20;\n\n");

    gerar_dot_nos(raiz, f);
    fprintf(f, "\n");
    gerar_dot_arestas(raiz, f);

    fprintf(f, "}\n");
    fclose(f);
    printf("Arquivo DOT gerado em '%s'\n", nome_arquivo);
}

// ============================================================================
// ANALISADOR SINTÁTICO COM GERAÇÃO DE AST
// ============================================================================

bool imprimir_regras = true;

void print_regra(const char *str) {
    if (imprimir_regras) {
        printf("%s", str);
    }
}

void proximo() {
    if (tok.lexema) {
        free(tok.lexema);
    }
    tok = obter_token(&scanner);
    if (tok.tipo == TOKEN_ERRO) {
        erro_lexico();
    }
}

void CasaToken(TipoToken tipo_esperado) {
    if (tok.tipo == tipo_esperado) {
        proximo();
    } else {
        erro_sintatico();
    }
}

// Compartilhados para a declaração de variáveis
char declaracao_ids[100][100];
int declaracao_linhas[100];
int declaracao_colunas[100];
int declaracao_qtd = 0;

// Protótipos das funções do parser
AST *programa();
AST *bloco();
AST *declaracoes();
AST *declaracao();
void lista_ids();
TipoDado tipo();
AST *comando_composto();
AST *lista_comandos();
AST *comando();
AST *atribuicao();
AST *comando_condicional();
AST *comando_repetitivo();
AST *expressao_relacional();
AST *expressao();
AST *termo();
AST *fator();

// programa -> program id ; bloco .
AST *programa() {
    print_regra("programa -> program id ; bloco .\n");
    char nome_prog[100] = "";
    if (tok.tipo == TOKEN_ID) {
        strncpy(nome_prog, tok.lexema, sizeof(nome_prog) - 1);
    }
    CasaToken(TOKEN_PROGRAM);
    CasaToken(TOKEN_ID);
    CasaToken(TOKEN_SMB_SEM);
    AST *bl = bloco();
    CasaToken(TOKEN_SMB_DOT);
    return criar_no(AST_PROGRAMA, nome_prog, bl, NULL, NULL);
}

// bloco -> declaracoes comando_composto
AST *bloco() {
    print_regra("bloco -> declaracoes comando_composto\n");
    AST *decl = declaracoes();
    AST *cmd = comando_composto();
    return criar_no(AST_BLOCO, NULL, decl, cmd, NULL);
}

// declaracoes -> var declaracao { ; declaracao } ; | vazio
AST *declaracoes() {
    if (tok.tipo == TOKEN_VAR) {
        print_regra("declaracoes -> var declaracao { ; declaracao } ;\n");
        CasaToken(TOKEN_VAR);
        AST *raiz_decl = declaracao();
        AST *atual = raiz_decl;
        while (atual != NULL && atual->prox != NULL) {
            atual = atual->prox;
        }
        while (tok.tipo == TOKEN_SMB_SEM) {
            CasaToken(TOKEN_SMB_SEM);
            if (tok.tipo == TOKEN_ID) {
                AST *prox_decl = declaracao();
                atual->prox = prox_decl;
                while (atual->prox != NULL) {
                    atual = atual->prox;
                }
            } else {
                break;
            }
        }
        return criar_no(AST_DECLARACOES, NULL, raiz_decl, NULL, NULL);
    } else {
        print_regra("declaracoes -> vazio\n");
        return NULL;
    }
}

// declaracao -> lista_ids : tipo
AST *declaracao() {
    print_regra("declaracao -> lista_ids : tipo\n");
    lista_ids();
    CasaToken(TOKEN_SMB_COL);
    TipoDado t = tipo();

    AST *raiz = NULL;
    AST *ultimo = NULL;
    for (int i = 0; i < declaracao_qtd; i++) {
        AST *var_no = criar_no(AST_VARIAVEL, declaracao_ids[i], NULL, NULL, NULL);
        var_no->tipo_dado = t;
        AST *no = criar_no(AST_DECLARACAO, nome_tipo(t), var_no, NULL, NULL);
        
        if (raiz == NULL) {
            raiz = no;
        } else {
            ultimo->prox = no;
        }
        ultimo = no;
    }
    return raiz;
}

// lista_ids -> id { , id }
void lista_ids() {
    print_regra("lista_ids -> id { , id }\n");
    declaracao_qtd = 0;

    if (tok.tipo == TOKEN_ID) {
        if (declaracao_qtd < 100) {
            strncpy(declaracao_ids[declaracao_qtd], tok.lexema, 99);
            declaracao_ids[declaracao_qtd][99] = '\0';
            declaracao_linhas[declaracao_qtd] = tok.linha;
            declaracao_colunas[declaracao_qtd] = tok.coluna;
            declaracao_qtd++;
        }
    }
    CasaToken(TOKEN_ID);

    while (tok.tipo == TOKEN_SMB_COM) {
        CasaToken(TOKEN_SMB_COM);
        if (tok.tipo == TOKEN_ID) {
            if (declaracao_qtd < 100) {
                strncpy(declaracao_ids[declaracao_qtd], tok.lexema, 99);
                declaracao_ids[declaracao_qtd][99] = '\0';
                declaracao_linhas[declaracao_qtd] = tok.linha;
                declaracao_colunas[declaracao_qtd] = tok.coluna;
                declaracao_qtd++;
            }
        }
        CasaToken(TOKEN_ID);
    }
}

// tipo -> integer | real
TipoDado tipo() {
    TipoDado t = TYPE_VOID;
    if (tok.tipo == TOKEN_INTEGER) {
        print_regra("tipo -> integer\n");
        t = TYPE_INT;
        CasaToken(TOKEN_INTEGER);
    } else if (tok.tipo == TOKEN_REAL) {
        print_regra("tipo -> real\n");
        t = TYPE_REAL;
        CasaToken(TOKEN_REAL);
    } else {
        erro_sintatico();
    }

    for (int i = 0; i < declaracao_qtd; i++) {
        inserir_simbolo(declaracao_ids[i], t, declaracao_linhas[i], declaracao_colunas[i]);
    }
    return t;
}

// comando_composto -> begin lista_comandos end
AST *comando_composto() {
    print_regra("comando_composto -> begin lista_comandos end\n");
    CasaToken(TOKEN_BEGIN);
    AST *lista = lista_comandos();
    CasaToken(TOKEN_END);
    return lista;
}

// lista_comandos -> comando { ; comando }
AST *lista_comandos() {
    print_regra("lista_comandos -> comando { ; comando }\n");
    AST *cmd_raiz = comando();
    AST *atual = cmd_raiz;

    while (tok.tipo == TOKEN_SMB_SEM) {
        CasaToken(TOKEN_SMB_SEM);
        AST *cmd_prox = comando();
        if (cmd_raiz == NULL) {
            cmd_raiz = cmd_prox;
            atual = cmd_prox;
        } else if (cmd_prox != NULL) {
            while (atual->prox != NULL) {
                atual = atual->prox;
            }
            atual->prox = cmd_prox;
        }
    }
    return criar_no(AST_COMANDOS, NULL, cmd_raiz, NULL, NULL);
}

// comando -> atribuicao | comando_condicional | comando_repetitivo | comando_composto | vazio
AST *comando() {
    if (tok.tipo == TOKEN_ID) {
        print_regra("comando -> atribuicao\n");
        return atribuicao();
    } else if (tok.tipo == TOKEN_IF) {
        print_regra("comando -> comando_condicional\n");
        return comando_condicional();
    } else if (tok.tipo == TOKEN_WHILE) {
        print_regra("comando -> comando_repetitivo\n");
        return comando_repetitivo();
    } else if (tok.tipo == TOKEN_BEGIN) {
        print_regra("comando -> comando_composto\n");
        return comando_composto();
    } else {
        print_regra("comando -> vazio\n");
        return NULL;
    }
}

// atribuicao -> id := expressao
AST *atribuicao() {
    print_regra("atribuicao -> id := expressao\n");
    int lin = tok.linha;
    int col = tok.coluna;
    char nome_var[100];
    strncpy(nome_var, tok.lexema, sizeof(nome_var) - 1);
    nome_var[sizeof(nome_var) - 1] = '\0';

    CasaToken(TOKEN_ID);

    TipoDado tipo_esq = buscar_simbolo(nome_var, lin, col);

    CasaToken(TOKEN_OP_ASS); // ':='

    AST *expr = expressao();
    TipoDado tipo_dir = expr->tipo_dado;

    if (tipo_esq != TYPE_VOID && tipo_dir != TYPE_VOID) {
        if (tipo_esq == TYPE_INT && tipo_dir == TYPE_REAL) {
            printf("Erro semantico na linha %d, coluna %d: Incompatibilidade de tipos na atribuicao. Nao e possivel atribuir REAL a variavel INTEIRA '%s'.\n",
                lin, col, nome_var);
            liberar_tabela();
            liberar_ast(expr);
            if (tok.lexema) free(tok.lexema);
            exit(1);
        }
    }

    AST *var_no = criar_no(AST_VARIAVEL, nome_var, NULL, NULL, NULL);
    var_no->tipo_dado = tipo_esq;

    AST *atrib = criar_no(AST_ATRIBUICAO, NULL, var_no, expr, NULL);
    atrib->tipo_dado = tipo_esq;
    return atrib;
}

// comando_condicional -> if expressao_relacional then comando [ else comando ]
AST *comando_condicional() {
    print_regra("comando_condicional -> if expressao_relacional then comando [ else comando ]\n");
    CasaToken(TOKEN_IF);
    AST *cond = expressao_relacional();
    TipoDado t = cond->tipo_dado;

    if (t != TYPE_VOID && t != TYPE_BOOL) {
        printf("Erro semantico na linha %d: a condicao do IF deve ser do tipo BOOLEANO.\n", tok.linha);
        liberar_tabela();
        liberar_ast(cond);
        if (tok.lexema) free(tok.lexema);
        exit(1);
    }

    CasaToken(TOKEN_THEN);
    AST *cmd_then = comando();
    AST *cmd_else = NULL;

    if (tok.tipo == TOKEN_ELSE) {
        CasaToken(TOKEN_ELSE);
        cmd_else = comando();
    }

    return criar_no(AST_IF, NULL, cond, cmd_then, cmd_else);
}

// comando_repetitivo -> while expressao_relacional do comando
AST *comando_repetitivo() {
    print_regra("comando_repetitivo -> while expressao_relacional do comando\n");
    CasaToken(TOKEN_WHILE);
    AST *cond = expressao_relacional();
    TipoDado t = cond->tipo_dado;

    if (t != TYPE_VOID && t != TYPE_BOOL) {
        printf("Erro semantico na linha %d: a condicao do WHILE deve ser do tipo BOOLEANO.\n", tok.linha);
        liberar_tabela();
        liberar_ast(cond);
        if (tok.lexema) free(tok.lexema);
        exit(1);
    }

    CasaToken(TOKEN_DO);
    AST *cmd = comando();

    return criar_no(AST_WHILE, NULL, cond, cmd, NULL);
}

// expressao_relacional -> expressao [ relacao expressao ]
AST *expressao_relacional() {
    AST *esq = expressao();
    TipoDado tipo_esq = esq->tipo_dado;

    if (tok.tipo == TOKEN_OP_EQ || tok.tipo == TOKEN_OP_NE ||
        tok.tipo == TOKEN_OP_LT || tok.tipo == TOKEN_OP_LE ||
        tok.tipo == TOKEN_OP_GT || tok.tipo == TOKEN_OP_GE) {
        
        print_regra("expressao_relacional -> expressao relacao expressao\n");
        if (imprimir_regras) {
            printf("relacao -> %s\n", tok.lexema);
        }

        TipoToken op = tok.tipo;
        char lex_op[10] = "";
        strncpy(lex_op, tok.lexema, sizeof(lex_op) - 1);

        CasaToken(op);
        AST *dir = expressao();
        TipoDado tipo_dir = dir->tipo_dado;

        TipoAST tipo_op_ast;
        if (op == TOKEN_OP_EQ) tipo_op_ast = AST_IGUAL;
        else if (op == TOKEN_OP_NE) tipo_op_ast = AST_DIFERENTE;
        else if (op == TOKEN_OP_LT) tipo_op_ast = AST_MENOR;
        else if (op == TOKEN_OP_LE) tipo_op_ast = AST_MENOR_IGUAL;
        else if (op == TOKEN_OP_GT) tipo_op_ast = AST_MAIOR;
        else tipo_op_ast = AST_MAIOR_IGUAL;

        AST *rel = criar_no(tipo_op_ast, lex_op, esq, dir, NULL);

        if (tipo_esq == TYPE_VOID || tipo_dir == TYPE_VOID) {
            rel->tipo_dado = TYPE_VOID;
        } else if (tipo_esq == TYPE_BOOL || tipo_dir == TYPE_BOOL) {
            printf("Erro semantico na linha %d: operacao relacional invalida envolvendo tipo BOOLEANO.\n", tok.linha);
            liberar_tabela();
            liberar_ast(rel);
            if (tok.lexema) free(tok.lexema);
            exit(1);
        } else {
            rel->tipo_dado = TYPE_BOOL; // Comparações resultam em BOOLEANO
        }
        return rel;
    }
    
    print_regra("expressao_relacional -> expressao\n");
    return esq;
}

// expressao -> [ + | - ] termo { ( + | - ) termo }
AST *expressao() {
    print_regra("expressao -> [ + | - ] termo { ( + | - ) termo }\n");
    bool tem_sinal = false;
    TipoToken sinal = TOKEN_FIM;
    if (tok.tipo == TOKEN_OP_AD || tok.tipo == TOKEN_OP_MIN) {
        tem_sinal = true;
        sinal = tok.tipo;
        CasaToken(tok.tipo);
    }

    AST *esq = termo();
    TipoDado tipo_esq = esq->tipo_dado;

    if (tem_sinal) {
        if (tipo_esq == TYPE_BOOL) {
            printf("Erro semantico na linha %d: operacao unaria invalida para tipo BOOLEANO.\n", tok.linha);
            liberar_tabela();
            liberar_ast(esq);
            if (tok.lexema) free(tok.lexema);
            exit(1);
        }
        esq = criar_no(sinal == TOKEN_OP_AD ? AST_SOMA : AST_SUBTRACAO, NULL, esq, NULL, NULL);
        esq->tipo_dado = tipo_esq;
    }

    while (tok.tipo == TOKEN_OP_AD || tok.tipo == TOKEN_OP_MIN) {
        TipoToken op = tok.tipo;
        CasaToken(op);
        AST *dir = termo();
        TipoDado tipo_dir = dir->tipo_dado;

        AST *novo_esq = criar_no(op == TOKEN_OP_AD ? AST_SOMA : AST_SUBTRACAO, NULL, esq, dir, NULL);

        if (tipo_esq == TYPE_VOID || tipo_dir == TYPE_VOID) {
            novo_esq->tipo_dado = TYPE_VOID;
        } else if (tipo_esq == TYPE_BOOL || tipo_dir == TYPE_BOOL) {
            printf("Erro semantico na linha %d: operacao aritmetica invalida envolvendo tipo BOOLEANO.\n", tok.linha);
            liberar_tabela();
            liberar_ast(novo_esq);
            if (tok.lexema) free(tok.lexema);
            exit(1);
        } else if (tipo_esq == TYPE_REAL || tipo_dir == TYPE_REAL) {
            novo_esq->tipo_dado = TYPE_REAL;
        } else {
            novo_esq->tipo_dado = TYPE_INT;
        }

        esq = novo_esq;
        tipo_esq = esq->tipo_dado;
    }
    return esq;
}

// termo -> fator { ( * | / ) fator }
AST *termo() {
    print_regra("termo -> fator { ( * | / ) fator }\n");
    AST *esq = fator();
    TipoDado tipo_esq = esq->tipo_dado;

    while (tok.tipo == TOKEN_OP_MUL || tok.tipo == TOKEN_OP_DIV) {
        TipoToken op = tok.tipo;
        CasaToken(op);
        AST *dir = fator();
        TipoDado tipo_dir = dir->tipo_dado;

        AST *novo_esq = criar_no(op == TOKEN_OP_MUL ? AST_MULTIPLICACAO : AST_DIVISAO, NULL, esq, dir, NULL);

        if (tipo_esq == TYPE_VOID || tipo_dir == TYPE_VOID) {
            novo_esq->tipo_dado = TYPE_VOID;
        } else if (tipo_esq == TYPE_BOOL || tipo_dir == TYPE_BOOL) {
            printf("Erro semantico na linha %d: operacao aritmetica invalida envolvendo tipo BOOLEANO.\n", tok.linha);
            liberar_tabela();
            liberar_ast(novo_esq);
            if (tok.lexema) free(tok.lexema);
            exit(1);
        } else if (op == TOKEN_OP_DIV) {
            novo_esq->tipo_dado = TYPE_REAL;
        } else {
            if (tipo_esq == TYPE_REAL || tipo_dir == TYPE_REAL) {
                novo_esq->tipo_dado = TYPE_REAL;
            } else {
                novo_esq->tipo_dado = TYPE_INT;
            }
        }

        esq = novo_esq;
        tipo_esq = esq->tipo_dado;
    }
    return esq;
}

// fator -> id | num_int | num_real | ( expressao )
AST *fator() {
    if (tok.tipo == TOKEN_ID) {
        print_regra("fator -> id\n");
        char nome_var[100];
        strncpy(nome_var, tok.lexema, sizeof(nome_var) - 1);
        nome_var[sizeof(nome_var) - 1] = '\0';
        int lin = tok.linha;
        int col = tok.coluna;
        CasaToken(TOKEN_ID);

        TipoDado t = buscar_simbolo(nome_var, lin, col);
        AST *no = criar_no(AST_VARIAVEL, nome_var, NULL, NULL, NULL);
        no->tipo_dado = t;
        return no;
    } 
    else if (tok.tipo == TOKEN_NUM_INT) {
        print_regra("fator -> num_int\n");
        char val[100];
        strncpy(val, tok.lexema, sizeof(val) - 1);
        val[sizeof(val) - 1] = '\0';
        CasaToken(TOKEN_NUM_INT);
        AST *no = criar_no(AST_NUM_INT, val, NULL, NULL, NULL);
        no->tipo_dado = TYPE_INT;
        return no;
    } 
    else if (tok.tipo == TOKEN_NUM_REAL) {
        print_regra("fator -> num_real\n");
        char val[100];
        strncpy(val, tok.lexema, sizeof(val) - 1);
        val[sizeof(val) - 1] = '\0';
        CasaToken(TOKEN_NUM_REAL);
        AST *no = criar_no(AST_NUM_REAL, val, NULL, NULL, NULL);
        no->tipo_dado = TYPE_REAL;
        return no;
    } 
    else if (tok.tipo == TOKEN_SMB_OPA) {
        print_regra("fator -> ( expressao )\n");
        CasaToken(TOKEN_SMB_OPA);
        AST *no = expressao_relacional();
        CasaToken(TOKEN_SMB_CPA);
        return no;
    } 
    else {
        erro_sintatico();
        return NULL;
    }
}

// ============================================================================
// FUNÇÃO PRINCIPAL E LEITURA DE ARQUIVO
// ============================================================================

char *ler_arquivo(const char *nome_arquivo) {
    FILE *f = fopen(nome_arquivo, "r");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);
    char *buf = (char *) malloc(size + 1);
    if (!buf) {
        fclose(f);
        return NULL;
    }
    size_t read_bytes = fread(buf, 1, size, f);
    buf[read_bytes] = '\0';
    fclose(f);
    return buf;
}

void imprimir_tabela_simbolos() {
    printf("\n=== TABELA DE SIMBOLOS ===\n");
    Simbolo *atual = tabela_simbolos;
    if (atual == NULL) {
        printf("(Tabela vazia)\n");
        return;
    }
    while (atual != NULL) {
        printf("Variavel: %-15s | Tipo: %s\n", atual->nome, nome_tipo(atual->tipo));
        atual = atual->prox;
    }
    printf("==========================\n");
}

int main(int argc, char **argv) {
    const char *nome_arquivo = "program.pas";

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--silent") == 0 || strcmp(argv[i], "-s") == 0) {
            imprimir_regras = false;
        } else {
            nome_arquivo = argv[i];
        }
    }

    char *conteudo = ler_arquivo(nome_arquivo);
    if (!conteudo) {
        printf("Erro: nao foi possivel abrir ou ler o arquivo '%s'\n", nome_arquivo);
        return 1;
    }

    iniciar(&scanner, conteudo);
    tok.lexema = NULL;
    
    proximo();

    AST *raiz_ast = programa();

    if (tok.tipo != TOKEN_FIM) {
        erro_sintatico();
    }

    imprimir_tabela_simbolos();

    // Exportar e gerar imagem com Graphviz
    exportar_ast_dot(raiz_ast, "ast.dot");
    #ifdef __APPLE__
        system("dot -Tpng ast.dot -o ast.png 2>/dev/null");
    #elif _WIN32
        system("dot -Tpng ast.dot -o ast.png");
    #else
        system("dot -Tpng ast.dot -o ast.png 2>/dev/null");
    #endif

    printf("\n=== RESULTADO DA ANALISE ===\n");
    printf("Compilacao terminada com SUCESSO! Programa sintatica e semanticamente correto.\n");
    printf("============================\n");

    liberar_ast(raiz_ast);
    if (tok.lexema) {
        free(tok.lexema);
    }
    free(conteudo);
    liberar_tabela();

    return 0;
}
