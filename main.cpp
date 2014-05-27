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

void recebeAluno(tipo_aluno &aluno) {
    printf("RA    :");
    scanf("%i", &aluno.ra);
    printf("NOME  :");
    scanf("%s", aluno.nome); //TODO TROCAR POR FUNÇÃO
    printf("NOTA  :");
    scanf("%f", &aluno.nota);
}

void mostraAluno(tipo_aluno aluno) {
    printf("%5d %-50s %4.1f\n", aluno.ra, aluno.nome, aluno.nota);
}

void inicializaHash(int tamanho) {
    //TAREFA Verificar se o arquivo existe?



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
    printf("Pronto.\n");
}

int menu() {
    printf("[1] Inserir\n");
    printf("[2] Alterar\n");
    printf("[3] Excluir\n");
    printf("[4] Procurar\n");
    printf("[5] Listar\n");
    printf("[0] Sair\n");
    printf("\nOPCAO:");
    int opcao;
    scanf("%d", &opcao);
    return opcao;
    
    
}

int main() {
    inicializaHash(MAX);
    FILE *arq = abrirArquivo();
    int o =;
    do {
        o = menu();
        switch o :
            case 1:inserir(arq);
        break;
        case 2:alterar(arq);
        break;
        case 3:excluir(arq);
        break;
        case 4:procurar(arq);
        break;
        case 5:listar(arq);
        break;
        case
    } while (o != 0);
    fecharArquivo();
}

