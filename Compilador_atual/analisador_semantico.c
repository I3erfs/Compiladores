#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "node.h"
#include "analisador_semantico.h"
#include "symbol_table.h"

extern SymbolTable tabela;
extern treeNode *syntaxTree;

static char *currentScope = "global";

static Symbol* lookupSymbol(char *name) {
    Symbol *sym = findSymbol(&tabela, name, currentScope);
    
    if (sym == NULL && strcmp(currentScope, "global") != 0) {
        sym = findSymbol(&tabela, name, "global");
    }
    
    return sym;
}

static void semanticError(treeNode *t, char *msg) {
    if (t != NULL)
        fprintf(stderr, "ERRO SEMANTICO (Linha %d): %s\n", t->line, msg);
    else
        fprintf(stderr, "ERRO SEMANTICO: %s\n", msg);
}

static void checkNode(treeNode *t) {
    Symbol *sym;
    Symbol *funcSym;

    switch (t->node) {
        
        case exp:
            switch (t->nodeSubType.exp) {
                case expOp:
                    if ((t->child[0]->type != Integer) || (t->child[1]->type != Integer)) {
                        semanticError(t, "Operacao requer operandos do tipo Integer");
                    }
                    
                    t->type = Integer; 
                    break;

                case expNum:
                    t->type = Integer;
                    break;

                case expId:
                    if (t->key.name != NULL && strcmp(t->key.name, "void") == 0) {
                        t->type = Void; 
                        break; 
                    }
                    sym = lookupSymbol(t->key.name);
                    if (sym == NULL) {
                        char erro[100];
                        sprintf(erro, "Variavel '%s' nao declarada", t->key.name);
                        semanticError(t, erro);
                        t->type = Integer; 
                    } else {
                        t->type = sym->dataType;
                    }
                    break;
            }
            break;

        case stmt:
            switch (t->nodeSubType.stmt) {
                case stmtIf:
                case stmtWhile:
                    if (t->child[0]->type == Void) {
                        semanticError(t->child[0], "Condicao do IF/WHILE nao pode ser Void");
                    }
                    break;

                case stmtAttrib:
                    if (t->child[0]->nodeSubType.exp == expId) {
                        sym = lookupSymbol(t->child[0]->key.name);
                        if (sym == NULL) {
                        } else {
                            if (sym->dataType != t->child[1]->type) {
                                semanticError(t, "Atribuicao com tipos incompatíveis");
                            }
                            if (sym->type == ARRAY) {
                                semanticError(t, "Atribuicao direta para Array nao permitida (use indices)");
                            }
                        }
                    }
                    break;

                case stmtFunc:
                    sym = findSymbol(&tabela, t->key.name, "global");
                    if (sym == NULL) {
                        char erro[100];
                        sprintf(erro, "Funcao '%s' nao declarada", t->key.name);
                        semanticError(t, erro);
                    } else {
                        t->type = sym->dataType;
                    }
                    break;

                case stmtReturn:
                    if (strcmp(currentScope, "global") == 0) {
                        semanticError(t, "Return fora de funcao");
                    } else {
                        funcSym = findSymbol(&tabela, currentScope, "global");
                        
                        if (t->child[0] != NULL) {
                            if (funcSym->dataType == Void)
                                semanticError(t, "Funcao Void nao pode retornar valor");
                            else if (t->child[0]->type != funcSym->dataType)
                                semanticError(t, "Tipo de retorno incompativel com a funcao");
                        } 
                        else {
                            if (funcSym->dataType != Void)
                                semanticError(t, "Funcao nao-Void deve retornar um valor");
                        }
                    }
                    break;
            }
            break;

        case decl:
            switch (t->nodeSubType.decl) {
                case declVar:
                    if (t->type == Void) {
                        semanticError(t, "Variavel nao pode ser declarada como Void");
                    }
                    break;
            }
            break;
    }
}

static void traverse(treeNode *t) {
    if (t != NULL) {
        
        char *savedScope = currentScope; 
        
        if (t->node == decl && t->nodeSubType.decl == declFunc) {
            currentScope = t->key.name;
        }

        int i;
        for (i = 0; i < CHILD_MAX_NODES; i++) {
            traverse(t->child[i]);
        }

        checkNode(t);

        currentScope = savedScope;

        traverse(t->sibling);
    }
}

void semanticAnalysis(treeNode *tree) {
    if (findSymbol(&tabela, "main", "global") == NULL) {
        fprintf(stderr, "ERRO SEMANTICO: Funcao 'main' nao encontrada.\n");
    }

    traverse(tree);
}
