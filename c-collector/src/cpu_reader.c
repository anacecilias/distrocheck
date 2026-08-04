#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_cpu_info() {
    FILE *file = fopen("/proc/cpuinfo", "r");
    if (file == NULL) {
        printf("Erro ao abrir /proc/cpuinfo\n");
        return;
    }

    char line[256];
    char cpu_model[256] = "Desconhecido";
    int cores = 0;

    while (fgets(line, sizeof(line), file)) {
        if (strncmp(line, "processor", 9) == 0) {
            cores++;
        }
        if (strncmp(line, "model name", 10) == 0 && strcmp(cpu_model, "Desconhecido") == 0) {
            char *colon = strchr(line, ':');
            if (colon != NULL) {
                strncpy(cpu_model, colon + 2, sizeof(cpu_model) - 1);
                cpu_model[strcspn(cpu_model, "\n")] = 0;
            }
        }
    }
    fclose(file);

    printf("=== INFORMAÇÕES DA CPU ===\n");
    printf("Modelo:  %s\n", cpu_model);
    printf("Núcleos: %d\n", cores);
}