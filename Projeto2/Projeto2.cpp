#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Estrutura para representar uma imagem
typedef struct {
    int width;
    int height;
    int **pixels;
} Image;

// Declaração de funções
void printUsage();
int readPBMFile(const char *filename, Image *img);
void manualInput(Image *img);
void freeImage(Image *img);
void generateCode(Image *img, char *result);
void checkHomogeneity(int **matrix, int startRow, int startCol, int rows, int cols, char *result);

// Função principal
int main(int argc, char *argv[]) {
    if (argc < 2) {
        printUsage();
        return 1;
    }

    Image img;
    char code[1024] = ""; // Buffer para armazenar o código gerado

    if (strcmp(argv[1], "-?") == 0) {
        printUsage();
    } else if (strcmp(argv[1], "-m") == 0) {
        manualInput(&img);
        generateCode(&img, code);
        printf("Código gerado: %s\n", code);
        freeImage(&img);
    } else if (strcmp(argv[1], "-f") == 0 && argc == 3) {
        if (readPBMFile(argv[2], &img) == 0) {
            generateCode(&img, code);
            printf("Código gerado: %s\n", code);
            freeImage(&img);
        } else {
            printf("Erro ao ler o arquivo PBM.\n");
        }
    } else {
        printf("Parâmetro desconhecido.\n");
        printUsage();
    }

    return 0;
}

// Função para exibir as instruções de uso
void printUsage() {
    printf("Uso: ./programa [opção]\n");
    printf("  -?   : Mostra esta ajuda\n");
    printf("  -m   : Entrada manual de dados\n");
    printf("  -f [arquivo.pbm] : Carrega dados do arquivo PBM\n");
}

// Função para entrada manual de dados
void manualInput(Image *img) {
    printf("Digite as dimensões da imagem (largura altura): ");
    scanf("%d %d", &img->width, &img->height);

    img->pixels = (int **)malloc(img->height * sizeof(int *));
    for (int i = 0; i < img->height; i++) {
        img->pixels[i] = (int *)malloc(img->width * sizeof(int));
    }

    printf("Digite os valores dos pixels (0 ou 1):\n");
    for (int i = 0; i < img->height; i++) {
        for (int j = 0; j < img->width; j++) {
            scanf("%d", &img->pixels[i][j]);
        }
    }
}

// Função para ler um arquivo PBM
int readPBMFile(const char *filename, Image *img) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Erro ao abrir o arquivo");
        return 1;
    }

    char magicNumber[3];
    fscanf(file, "%2s", magicNumber);
    if (strcmp(magicNumber, "P1") != 0) {
        printf("Arquivo não é do formato PBM válido.\n");
        fclose(file);
        return 1;
    }

    // Ignorar comentários
    char c;
    while ((c = fgetc(file)) == '#') {
        while (fgetc(file) != '\n');
    }
    ungetc(c, file);

    fscanf(file, "%d %d", &img->width, &img->height);

    img->pixels = (int **)malloc(img->height * sizeof(int *));
    for (int i = 0; i < img->height; i++) {
        img->pixels[i] = (int *)malloc(img->width * sizeof(int));
    }

    for (int i = 0; i < img->height; i++) {
        for (int j = 0; j < img->width; j++) {
            fscanf(file, "%d", &img->pixels[i][j]);
        }
    }

    fclose(file);
    return 0;
}

// Função para liberar a memória alocada para a imagem
void freeImage(Image *img) {
    for (int i = 0; i < img->height; i++) {
        free(img->pixels[i]);
    }
    free(img->pixels);
}

// Função para gerar o código da imagem
void generateCode(Image *img, char *result) {
    checkHomogeneity(img->pixels, 0, 0, img->height, img->width, result);
}

// Função recursiva para verificar a homogeneidade e gerar o código
void checkHomogeneity(int **matrix, int startRow, int startCol, int rows, int cols, char *result) {
    int firstPixel = matrix[startRow][startCol];
    int homogeneous = 1;

    for (int i = startRow; i < startRow + rows; i++) {
        for (int j = startCol; j < startCol + cols; j++) {
            if (matrix[i][j] != firstPixel) {
                homogeneous = 0;
                break;
            }
        }
        if (!homogeneous) break;
    }

    if (homogeneous) {
        char code = (firstPixel == 0) ? 'P' : 'B';
        strncat(result, &code, 1);
    } else {
        strncat(result, "X", 1);
        int midRow = rows / 2;
        int midCol = cols / 2;

        // Quadrante superior esquerdo
        checkHomogeneity(matrix, startRow, startCol, midRow, midCol, result);

        // Quadrante superior direito
        checkHomogeneity(matrix, startRow, startCol + midCol, midRow, cols - midCol, result);

        // Quadrante inferior esquerdo
        checkHomogeneity(matrix, startRow + midRow, startCol, rows - midRow, midCol, result);

        // Quadrante inferior direito
        checkHomogeneity(matrix, startRow + midRow, startCol + midCol, rows - midRow, cols - midCol, result);
    }
}

