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
    printf("%5d %-20s %4.1f\n", aluno.ra, aluno.nome, aluno.nota);
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

int calculaHash(int ra) { //Função que transforma a chave (RA) em posição
    return ra % MAX;
}

FILE* inicializaHash(int tamanho) {
    //TAREFA Verificar se o arquivo existe?
    printf("Criando arquvo hash...");
    FILE* arq = fopen("hash.arq", "wb");
    tipo_inicio inicio;
    inicio.cabeca.tamanho_hash = tamanho;
    inicio.cabeca.versao = 1;
    fwrite(&inicio, sizeof (tipo_inicio), 1, arq);
    for (int i = 0; i < tamanho; i++) {
        tipo_registro registro;
        registro.proximo = -2;
        fwrite(&registro, sizeof (tipo_registro), 1, arq);
    }
    printf("Pronto.\n");
    return arq;
}

FILE* abrirArquivo() {
    FILE* arq = fopen("hash.arq", "rb+");
    if (arq == NULL) {
        printf("Arquivo inexistente...");
        arq = inicializaHash(MAX);
    }
    return arq;
}

void fecharArquivo(FILE* arq) {
    //Tarefa Porque fechar o arquivo?
    fclose(arq);
}

int calculaPosFisica(int posLogica) {
    return sizeof (tipo_inicio) + posLogica * sizeof (tipo_registro);
}

int calculaPosLogica(int posFisica) {
    return (posFisica - sizeof (tipo_inicio)) / sizeof (tipo_registro);
}

void inserir(FILE* arq) {
    //Tarefa NAO DEIXAR INSERIR RA DUPLICADO
    tipo_registro novoRegistro, registroEmDisco;
    recebeAluno(novoRegistro.aluno);
    novoRegistro.proximo = -1;
    int posLogica = calculaHash(novoRegistro.aluno.ra);
    int posFisica = calculaPosFisica(posLogica);
    fseek(arq, posFisica, SEEK_SET);
    fread(&registroEmDisco, sizeof (tipo_registro), 1, arq);

    if (registroEmDisco.proximo == -2) {
        fseek(arq, -sizeof (tipo_registro), SEEK_CUR);
        fwrite(&novoRegistro, sizeof (tipo_registro), 1, arq);
    } else {
        printf("A posicao %d esta ocupada.\n", posLogica);
        fseek(arq, 0, SEEK_END);
        int posicaoLogicaNovo = calculaPosLogica(ftell(arq));
        novoRegistro.proximo = registroEmDisco.proximo;
        fwrite(&novoRegistro, sizeof (tipo_registro), 1, arq);
        fseek(arq, posFisica, SEEK_SET);
        registroEmDisco.proximo = posicaoLogicaNovo;
        fwrite(&registroEmDisco, sizeof (tipo_registro), 1, arq);
    }
}

void alterar(FILE* arq) { //Tarefa Alterar NAO PODE ALTERAR A CHAVE

}

void excluir(FILE* arq) { //Pensar como fazer

}

//MELHORAR O CÓDIGO DESTA FUNÇÃO
void procurar(FILE* arq) { //Tarefa digita o RA e diz e se existe mostra os dados
    int ra;
    printf("RA    :");
    scanf("%i", &ra);
    int posicaoLogica = calculaHash(ra);
    fseek(arq, calculaPosFisica(posicaoLogica), SEEK_SET);
    tipo_registro reg;
    fread(&reg, sizeof (tipo_registro), 1, arq);
    if (reg.proximo == -2) {
        printf("RA nao encontrado\n");
    } else {
        if (reg.aluno.ra == ra) {
            printf("Registro na area de hash\n");
            mostraAluno(reg.aluno);
        } else {
            printf("Procurando na area de colisao.\n");
            while (reg.proximo != -1) {
                fseek(arq, calculaPosFisica(reg.proximo), SEEK_SET);
                fread(&reg, sizeof (tipo_registro), 1, arq);
                if (reg.aluno.ra == ra) {
                    mostraAluno(reg.aluno);
                    break;
                }
            }
            if (reg.aluno.ra!=ra){
                printf ("Ra nao encontrado nem na area de colisao\n");
            }
        }
    }

}

void listar(FILE* arq) {
    fseek(arq, calculaPosFisica(0), SEEK_SET);
    tipo_registro reg;
    int posicao = 0;
    for (;;) {
        if (fread(&reg, sizeof (tipo_registro), 1, arq) == 0) {
            break;
        }
        if (reg.proximo != -2) {
            printf("Posicao %3d Proximo:%3d:", posicao, reg.proximo);
            mostraAluno(reg.aluno);
        } else {
            // printf ("Livre\n");
        }
        posicao++;
    }
}

int main() {
    //inicializaHash(MAX);
    FILE *arq = abrirArquivo();
    int o;
    do {
        o = menu();
        switch (o) {
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
        }
    } while (o != 0);
    fecharArquivo(arq);
}

