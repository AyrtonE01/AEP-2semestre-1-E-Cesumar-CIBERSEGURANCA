#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <unistd.h>

#define MAX_USUARIOS 50
#define MAX_LEN_NOME 50
#define MAX_LEN_SENHA 50
#define MAX_LEN_EMAIL 50
#define SENHA_ADMIN "admin123" // Senha de admin
#define CHAVE_CRIPTO 0xAA

// cores
#define COR_FUNDO_CAIXA "\033[31m" // vermelho
#define COR_TEXTO_CAIXA "\033[97m" // branco
#define COR_RESET "\033[0m" // reset das cores

// armazenamento de dados
typedef struct {
    char nome[MAX_LEN_NOME];
    char senha[MAX_LEN_SENHA];
    char nivelPermissao; // 'A' administrador, 'P' comum
    char email[MAX_LEN_EMAIL]; // Emails
    int idade; // Idade
    char cpf[15]; // CPF fictício
} Usuario;

// criptografia xor
void criptografarSenha(char *senha, char chave) {
    int i;
    for (i = 0; i < strlen(senha); i++) {
        senha[i] = senha[i] ^ chave; 
    }
}

// descriptografar senha
void descriptografarSenha(char *senha, char chave) {
    int i;
    for (i = 0; i < strlen(senha); i++) {
        senha[i] = senha[i] ^ chave; 
    }
}

// caixa
void desenharCaixa(const char* titulo) {
    int tamanhoTitulo = strlen(titulo);
    int largura = tamanhoTitulo + 7; // Largura

    // Impressão superior
    printf(COR_FUNDO_CAIXA);
    int i;
    for (i = 0; i < largura; i++) {
        printf("="); 
        fflush(stdout);
        usleep(10000);
    }
    printf(" \n");

    printf("="); 
    fflush(stdout);
    usleep(10000);

    // Impressão do título
    printf("|| %s ||\n", titulo);

    printf(" ||\n");
    usleep(10000);

    // Impressão inferior
    for (i = 0; i < largura; i++) {
        printf("="); 
        fflush(stdout);
        usleep(10000);
    }
    printf(COR_RESET"\n");
}

// salvar no arquivo
void salvarUsuarios(Usuario *usuarios, int numUsuarios) {
    FILE *arquivo = fopen("usuarios.txt", "w");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo para salvar os usuarios.\n");
        return;
    }

    int i;
    for (i = 0; i < numUsuarios; i++) {
        fprintf(arquivo, "%s %s %c %s %d %s\n", usuarios[i].nome, usuarios[i].senha, usuarios[i].nivelPermissao, usuarios[i].email, usuarios[i].idade, usuarios[i].cpf);
    }

    fclose(arquivo);
}

// carregamento dos usuários do arquivo
int carregarUsuarios(Usuario *usuarios) {
    FILE *arquivo = fopen("usuarios.txt", "r");
    if (arquivo == NULL) {
        printf("Nenhum arquivo de usuarios encontrado.\n");
        return 0;
    }

    int numUsuarios = 0;
    while (fscanf(arquivo, "%s %s %c %s %d %s", usuarios[numUsuarios].nome, usuarios[numUsuarios].senha, 
                  &usuarios[numUsuarios].nivelPermissao, usuarios[numUsuarios].email, &usuarios[numUsuarios].idade, 
                  usuarios[numUsuarios].cpf) != EOF) {
        numUsuarios++;
    }

    fclose(arquivo);
    return numUsuarios;
}

// Token
void gerarTokenMFA(char* token, int tamanho) {
    int i;
    for (i = 0; i < tamanho - 1; i++) {
        token[i] = '0' + rand() % 10; 
    }
    token[tamanho - 1] = '\0'; 
}

// MfA
int autenticacaoMultifatorial() {
    char tokenMFA[7];
    gerarTokenMFA(tokenMFA, sizeof(tokenMFA));

    printf("Token de autenticação multifatorial gerado: %s\n", tokenMFA);

    char codigoMFA[7];
    printf("Digite o código de autenticação multifatorial (MFA): ");
    scanf("%6s", codigoMFA); 

    if (strcmp(codigoMFA, tokenMFA) == 0) {
        return 1; 
    } else {
        printf("Código MFA incorreto! Acesso negado.\n");
        return 0;
    }
}

// adicionar usuário
void adicionarUsuario(Usuario *usuarios, int *numUsuarios, char chaveCripto) {
    desenharCaixa("Adicionar Novo Usuario");

    if (*numUsuarios >= MAX_USUARIOS) {
        printf("Limite de usuarios alcançado.\n");
        return;
    }

    printf("Digite o nome do usuario: ");
    scanf("%s", usuarios[*numUsuarios].nome);
    
    printf("Digite a senha: ");
    scanf("%s", usuarios[*numUsuarios].senha);

    // Criptografia da senha
    criptografarSenha(usuarios[*numUsuarios].senha, chaveCripto);

    printf("Digite o nivel de permissao (A para administrador, P para comum): ");
    scanf(" %c", &usuarios[*numUsuarios].nivelPermissao);

    // Validamento do nível de permissão
    while (usuarios[*numUsuarios].nivelPermissao != 'A' && usuarios[*numUsuarios].nivelPermissao != 'P') {
        printf("Nivel invalido! Digite novamente (A para administrador, P para comum): ");
        scanf(" %c", &usuarios[*numUsuarios].nivelPermissao);
    }

    printf("Digite o email do usuario: ");
    scanf("%s", usuarios[*numUsuarios].email);

    printf("Digite a idade do usuario: ");
    scanf("%d", &usuarios[*numUsuarios].idade);

    printf("Digite o CPF do usuario (exemplo fictício): ");
    scanf("%s", usuarios[*numUsuarios].cpf);

    (*numUsuarios)++;
    salvarUsuarios(usuarios, *numUsuarios);
    printf("Usuario adicionado com sucesso!\n");
}

// alterar usuário
void alterarUsuario(Usuario *usuarios, int numUsuarios, char chaveCripto, int idUsuario) {
    desenharCaixa("Alterar Usuario");

    // Comum, pode alterar seus próprios dados
    if (usuarios[idUsuario].nivelPermissao == 'P') {
        printf("Você está alterando seu próprio perfil.\n");
    }

    char nome[MAX_LEN_NOME];
    printf("Digite o nome do usuario que deseja alterar: ");
    scanf("%s", nome);

    // Comum
    if (usuarios[idUsuario].nivelPermissao == 'P' && strcmp(usuarios[idUsuario].nome, nome) != 0) {
        printf("Usuarios comuns não podem alterar outros usuarios.\n");
        return;
    }

    int i;
    for (i = 0; i < numUsuarios; i++) {
        if (strcmp(usuarios[i].nome, nome) == 0) {
            printf("Digite o novo nome (ou pressione Enter para manter): ");
            char novoNome[MAX_LEN_NOME];
            getchar();
            fgets(novoNome, MAX_LEN_NOME, stdin);
            if (novoNome[0] != '\n') {
                novoNome[strcspn(novoNome, "\n")] = '\0';
                strcpy(usuarios[i].nome, novoNome);
            }

            printf("Digite a nova senha (ou pressione Enter para manter): ");
            char novaSenha[MAX_LEN_SENHA];
            fgets(novaSenha, MAX_LEN_SENHA, stdin);
            if (novaSenha[0] != '\n') {
                novaSenha[strcspn(novaSenha, "\n")] = '\0';
                strcpy(usuarios[i].senha, novaSenha);
                criptografarSenha(usuarios[i].senha, chaveCripto);
            }

            printf("Digite o novo email (ou pressione Enter para manter): ");
            char novoEmail[MAX_LEN_EMAIL];
            fgets(novoEmail, MAX_LEN_EMAIL, stdin);
            if (novoEmail[0] != '\n') {
                novoEmail[strcspn(novoEmail, "\n")] = '\0';
                strcpy(usuarios[i].email, novoEmail);
            }

            printf("Digite a nova idade (ou pressione Enter para manter): ");
            int novaIdade;
            char buffer[10];
            fgets(buffer, sizeof(buffer), stdin);
            if (buffer[0] != '\n') {
                sscanf(buffer, "%d", &novaIdade);
                usuarios[i].idade = novaIdade;
            }

            printf("Digite o novo CPF (ou pressione Enter para manter): ");
            char novoCpf[15];
            fgets(novoCpf, sizeof(novoCpf), stdin);
            if (novoCpf[0] != '\n') {
                novoCpf[strcspn(novoCpf, "\n")] = '\0';
                strcpy(usuarios[i].cpf, novoCpf);
            }

            salvarUsuarios(usuarios, numUsuarios);
            printf("Usuario alterado com sucesso!\n");
            return;
        }
    }

    printf("Usuario nao encontrado.\n");
}
void excluirUsuario(Usuario *usuarios, int *numUsuarios) {
    desenharCaixa("Excluir Usuario");

    char nome[MAX_LEN_NOME];
    printf("Digite o nome do usuario que deseja excluir: ");
    scanf("%s", nome);

    int i, j, achado = 0;
    for (i = 0; i < *numUsuarios; i++) {
        if (strcmp(usuarios[i].nome, nome) == 0) {
            for (j = i; j < *numUsuarios - 1; j++) {
                usuarios[j] = usuarios[j + 1];
            }
            (*numUsuarios)--;
            achado = 1;
            break;
        }
    }

    if (achado) {
        salvarUsuarios(usuarios, *numUsuarios);
        printf("Usuario excluido com sucesso!\n");
    } else {
        printf("Usuario nao encontrado.\n");
    }
}

// listar usuários
void listarUsuarios(Usuario *usuarios, int numUsuarios, int isAdmin) {
    desenharCaixa("Lista de Usuarios");
    printf("Lista de usuarios:\n");

    int i;
    for (i = 0; i < numUsuarios; i++) {
        // comuns
        printf("Nome: %s \t| Nivel: %c", usuarios[i].nome, usuarios[i].nivelPermissao);

        if (isAdmin) {
            //adm
            char senhaDescriptografada[MAX_LEN_SENHA];
            strncpy(senhaDescriptografada, usuarios[i].senha, MAX_LEN_SENHA - 1);
            senhaDescriptografada[MAX_LEN_SENHA - 1] = '\0';
            descriptografarSenha(senhaDescriptografada, CHAVE_CRIPTO);

            printf(" \t| Senha: %s \t| Email: %s \t| Idade: %d \t| CPF: %s", 
                   senhaDescriptografada, usuarios[i].email, usuarios[i].idade, usuarios[i].cpf);
        }
        
        printf("\n");
    }
}

int main() {
    setlocale(LC_ALL, "Portuguese");

    Usuario usuarios[MAX_USUARIOS];
    int numUsuarios = carregarUsuarios(usuarios);

    int opcao;
    int idUsuario = -1;
    int isAdmin = 0; 
    char senhaAdmin[MAX_LEN_SENHA];
    char chaveCripto = 0xAA; 

    // Login do adm
    printf("Digite a senha de administrador para acessar o sistema: ");
    scanf("%s", senhaAdmin);
    if (strcmp(senhaAdmin, SENHA_ADMIN) == 0) {
        printf("Senha de admin correta! Agora, por favor, insira o código MFA.\n");

        if (autenticacaoMultifatorial()) {
            isAdmin = 1;
            printf("Acesso de administrador concedido.\n");
        } else {
            printf("Acesso de administrador negado.\n");
            return 0; 
        }
    } else {
        printf("Senha de admin incorreta.\n");
        printf("Você está logado como usuário comum.\n");
    }

    do {
        desenharCaixa("Menu de gerenciamento de usuarios");

        printf("\n1. Adicionar Usuario\n");
        printf("2. Alterar Usuario\n");
        printf("3. Excluir Usuario\n");
        printf("4. Listar Usuarios\n");
        printf("5. Sair\n");
        printf("Escolha uma opcao: ");
        scanf("%d", &opcao);

        switch (opcao) {
            case 1:
                if (isAdmin) {
                    adicionarUsuario(usuarios, &numUsuarios, chaveCripto);
                } else {
                    printf("Acesso restrito. Somente administradores podem adicionar usuarios.\n");
                }
                break;
            case 2:
                if (isAdmin) {
                    alterarUsuario(usuarios, numUsuarios, chaveCripto, idUsuario);
                } else {
                    printf("Acesso restrito. Somente administradores podem alterar usuarios.\n");
                }
                break;
            case 3:
                if (isAdmin) {
                    excluirUsuario(usuarios, &numUsuarios);
                } else {
                    printf("Acesso restrito. Somente administradores podem excluir usuarios.\n");
                }
                break;
            case 4:
    			listarUsuarios(usuarios, numUsuarios, isAdmin);
    			break;
            case 5:
                printf("Saindo...\n");
                break;
            default:
                printf("Opcao invalida.\n");
        }
    } while (opcao != 5);

    return 0;
}

