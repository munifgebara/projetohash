#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

struct tipo_no_arvore_nome {
    char nome[50];
    int posicao_no_arquivoHash;
    int esquerda;
    int direita;
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

void mostraIndice(tipo_no_arvore_nome tnan) {
    printf("%-20s %4i %4i %4i\n", tnan.nome, tnan.posicao_no_arquivoHash,
            tnan.esquerda, tnan.direita);
}

int menu() {
    printf("[1] Inserir\n");
    printf("[2] Alterar\n");
    printf("[3] Excluir\n");
    printf("[4] Procurar\n");
    printf("[5] Listar\n");
    printf("[6] Mostra Indice\n");
    printf("[7] Mostra Ordenado por nome\n");
    printf("[0] Sair\n");
    printf("\nOPCAO:");
    int opcao;
    scanf("%d", &opcao);
    return opcao;
}

int calculaHash(int ra) { //Função que transforma a chave (RA) em posição
    return ra % MAX;
}

void mostraIndicePorNome(FILE *arqIndice) {
    fseek(arqIndice, 0, SEEK_SET);
    int pos = 0;
    for (;;) {
        tipo_no_arvore_nome tnan;
        if (0 == fread(&tnan, sizeof (tipo_no_arvore_nome), 1, arqIndice)) {
            break;
        }
        printf("%03i ", pos);
        pos++;
        mostraIndice(tnan);
    }
}



void insereNoIndice(FILE* arqIndi, int posRaiz, int posDados,
        tipo_no_arvore_nome dados) {
    tipo_no_arvore_nome tnan;
    fseek(arqIndi, sizeof (tipo_no_arvore_nome) * posRaiz, SEEK_SET);
    fread(&tnan, sizeof (tipo_no_arvore_nome), 1, arqIndi);
    if (strcmp(dados.nome,tnan.nome)<0){
        if (tnan.esquerda==-1){
            fseek(arqIndi, -sizeof (tipo_no_arvore_nome) , SEEK_CUR);
            tnan.esquerda=posDados;
            fwrite(&tnan, sizeof (tipo_no_arvore_nome), 1, arqIndi);
        }
        else{
            insereNoIndice(arqIndi,tnan.esquerda,posDados,dados);
        }
    }
    else{
        if (tnan.direita==-1){
            fseek(arqIndi, -sizeof (tipo_no_arvore_nome) , SEEK_CUR);
            tnan.direita=posDados;
            fwrite(&tnan, sizeof (tipo_no_arvore_nome), 1, arqIndi);
        }
        else{
            insereNoIndice(arqIndi,tnan.direita,posDados,dados);
        }
    }
}

FILE* inicializaIndicePorNome(FILE* arqHash) {
    printf("Criando indice por nome...");
    FILE* arqInd = fopen("indice.arq", "wb+");
    fseek(arqHash, sizeof (tipo_inicio), SEEK_SET);
    int pos = 0;
    int tamIndice=0;
    for (;;) {
        tipo_registro reg;
        if (0 == fread(&reg, sizeof (tipo_registro), 1, arqHash)) {
            break;
        }
        if (reg.proximo != -2) {
            tamIndice++;
            tipo_no_arvore_nome tnan;
            strcpy(tnan.nome, reg.aluno.nome); //tnan.nome = reg.aluno.nome;
            tnan.posicao_no_arquivoHash = pos;
            tnan.direita = -1;
            tnan.esquerda = -1;
            fwrite(&tnan, sizeof (tipo_no_arvore_nome), 1, arqInd);
        }
        pos++;
    }
    printf("inseridos...arrumando...\n");
    for (int i = 1; i < tamIndice; i++) {
        tipo_no_arvore_nome tnan;
        fseek(arqInd, sizeof (tipo_no_arvore_nome) * i, SEEK_SET);
        fread(&tnan, sizeof (tipo_no_arvore_nome), 1, arqInd);
        insereNoIndice(arqInd, 0, i, tnan);
    }
    printf("pronto.\n");
    return arqInd;
}

FILE* inicializaHash(int tamanho) {
    printf("Criando arquvo hash...");
    FILE* arq = fopen("hash.arq", "wb+");
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
    fclose(arq);
}

int calculaPosFisica(int posLogica) {
    return sizeof (tipo_inicio) + posLogica * sizeof (tipo_registro);
}

int calculaPosLogica(int posFisica) {
    return (posFisica - sizeof (tipo_inicio)) / sizeof (tipo_registro);
}


void mostraOrdenado(FILE* arqIndi,FILE* arqDados,int pos) {
    if (pos==-1){
        return;
    }
    
    
    tipo_no_arvore_nome tnan;
    fseek(arqIndi, sizeof (tipo_no_arvore_nome) * pos, SEEK_SET);
    fread(&tnan, sizeof (tipo_no_arvore_nome), 1, arqIndi);
    
    mostraOrdenado(arqIndi,arqDados,tnan.esquerda);
    tipo_registro reg;
    fseek(arqDados, calculaPosFisica(tnan.posicao_no_arquivoHash), SEEK_SET);
    fread(&reg, sizeof (tipo_registro), 1, arqDados);
    mostraAluno(reg.aluno);
    mostraOrdenado(arqIndi,arqDados,tnan.direita);
    
    
}


void inserir(FILE* arq) {
    //Trabalho NAO DEIXAR INSERIR RA DUPLICADO
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

//TRABALHO COMPLETAR O CÓDIGO DESTA FUNÇÃO

void alterar(FILE* arq) { //Tarefa Alterar NAO PODE ALTERAR A CHAVE
    int ra;
    printf("RA    :");
    scanf("%i", &ra);
    int posicaoLogica = calculaHash(ra);
    fseek(arq, calculaPosFisica(posicaoLogica), SEEK_SET);
    tipo_registro reg;
    fread(&reg, sizeof (tipo_registro), 1, arq);
    if (reg.aluno.ra == ra) {
        printf("Registro encontrado\n");
        fseek(arq, -sizeof (tipo_registro), SEEK_CUR);
        printf("NOME  :");
        scanf("%s", reg.aluno.nome); //TODO TROCAR POR FUNÇÃO
        printf("NOTA  :");
        scanf("%f", &reg.aluno.nota);
        fwrite(&reg, sizeof (tipo_registro), 1, arq);
    } else {
        printf("Registro nao encontrado\n");
    }

}

//TRABALHO COMPLETAR O CÓDIGO DESTA FUNÇÃO

void excluir(FILE* arq) { //Pensar como fazer
    int ra;
    printf("RA    :");
    scanf("%i", &ra);
    int posicaoLogica = calculaHash(ra);
    fseek(arq, calculaPosFisica(posicaoLogica), SEEK_SET);
    tipo_registro reg;
    fread(&reg, sizeof (tipo_registro), 1, arq);
    if (reg.aluno.ra == ra && reg.proximo == -1) {
        printf("Registro encontrado e excluido\n");
        fseek(arq, -sizeof (tipo_registro), SEEK_CUR);
        reg.proximo = -2;
        fwrite(&reg, sizeof (tipo_registro), 1, arq);
    } else {
        printf("Registro nao encontrado\n");
    }
}


//TRABALHO MELHORAR O CÓDIGO DESTA FUNÇÃO

void procurar(FILE * arq) { //Tarefa digita o RA e diz e se existe mostra os dados
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
            if (reg.aluno.ra != ra) {
                printf("Ra nao encontrado nem na area de colisao\n");
            }
        }
    }

}

void listar(FILE * arq) {
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
    FILE *arqInd = inicializaIndicePorNome(arq);
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
            case 6:mostraIndicePorNome(arqInd);
                break;
            case 7:mostraOrdenado(arqInd,arq,0);
                break;

        }
    } while (o != 0);
    fecharArquivo(arq);
    fecharArquivo(arqInd);
}

