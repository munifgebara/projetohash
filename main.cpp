#include <stdio.h>
#include <stdlib.h>

#define MAX 100

struct tipo_aluno {
    int ra;
    char nome[50];
    float nota;
};

struct tipo_registro {
    tipo_aluno aluno;
    int proximo; // -2 livre 
    // -1 ocupado não tem proximo 
    // posição do proximo
};

struct tipo_cabeca {
    int tamanho_hash;
    int versao;


};

union tipo_inicio {
    tipo_cabeca cabeca;
    char nada[100];
};

void inicializaHash(int tamanho) {
    printf("Criando arquvo hash...");
    FILE* arq = fopen("hash.arq", "wb");
    tipo_inicio inicio;
    inicio.cabeca.tamanho_hash = tamanho;
    inicio.cabeca.versao = 1;
    fwrite(&inicio, sizeof (inicio), 1, arq);
    for (int i = 0; i < tamanho; i++) {
        tipo_registro registro;
        registro.proximo = -2;
        fwrite(&registro, sizeof (registro), 1, arq);
    }
    fclose(arq);
    printf("Pronto.");
}

int main() {
    printf("Tamanho do Tipo_aluno:%d\n", sizeof (tipo_aluno));
    printf("Tamanho do Tipo_cabeca:%d\n", sizeof (tipo_cabeca));
    printf("Tamanho do Tipo_incio:%d\n", sizeof (tipo_inicio));
    printf("Tamanho do Tipo_registro:%d\n", sizeof (tipo_registro));

    inicializaHash(MAX);


    return 0;
}

