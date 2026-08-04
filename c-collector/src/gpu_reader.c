#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gpu_reader.h"

void print_gpu_info(void) {
    // Executa o comando lspci do Linux buscando por placas de vídeo (VGA / 3D)
    FILE *fp = popen("lspci | grep -i -E 'vga|3d|2d'", "r");
    if (fp == NULL) {
        printf("Erro ao obter informações da GPU\n");
        return;
    }

    char buffer[256];
    printf("=== INFORMAÇÕES DA GPU ===\n");

    int found = 0;
    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        // Remove quebra de linha no final
        buffer[strcspn(buffer, "\n")] = 0;

        // Pega apenas a descrição da GPU após o código PCI (que fica depois dos dois pontos ':')
        char *gpu_name = strchr(buffer, ':');
        if (gpu_name != NULL) {
            // Avança após os dois pontos e o tipo de dispositivo
            char *clean_name = strchr(gpu_name + 1, ':');
            if (clean_name != NULL) {
                printf("GPU: %s\n", clean_name + 1);
            } else {
                printf("GPU: %s\n", gpu_name + 1);
            }
        } else {
            printf("GPU: %s\n", buffer);
        }
        found = 1;
    }

    if (!found) {
        printf("GPU: Não identificada ou integrada padrão\n");
    }
    printf("\n");

    pclose(fp);
}