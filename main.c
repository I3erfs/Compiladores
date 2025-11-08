#include <stdio.h>
#include <stdlib.h>
#define MAX_ERRORS 100

extern int yyparse();
extern FILE *yyin;

typedef struct {
    int linha;
    char token[256];
} Erro;

Erro erros[MAX_ERRORS];  // Lista para armazenar os erros
int num_erros = 0;       // Contador de erros

void imprimir_erros() {
    if (num_erros > 0) {
        fprintf(stderr, "Foram encontrados %d erros sintáticos:\n", num_erros);
        for (int i = 0; i < num_erros; i++) {
            fprintf(stderr, "Erro na linha %d, Token: '%s'\n", erros[i].linha, erros[i].token);
        }
    } else {
        printf("Nenhum erro sintático encontrado.\n");
    }
}

int main(int argc, char **argv) {
    // Verifica se um arquivo foi passado como argumento
    if (argc > 1) {
        yyin = fopen(argv[1], "r");
        if (!yyin) {
            perror("Erro ao abrir arquivo");
            return EXIT_FAILURE;
        }
    } else {
        yyin = stdin; // Lê da entrada padrão se nenhum arquivo for passado
    }

    // Executa o parser
    if (yyparse() == 0) {
        printf("Análise sintática concluída com sucesso!\n");
    } else {
        imprimir_erros();
    }

    if (yyin != stdin) {
        fclose(yyin);
    }

    return EXIT_SUCCESS;
}
