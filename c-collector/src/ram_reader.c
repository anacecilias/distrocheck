#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ram_reader.h"

void print_ram_info(void) {
    FILE *file = fopen("/proc/meminfo", "r");
    if (file == NULL) {
        printf("Erro ao abrir /proc/meminfo\n");
        return;
    }

    char line[256];
    long total_ram_kb = 0;
    long free_ram_kb = 0;

    while (fgets(line, sizeof(line), file)) {
        // Pega a RAM total
        if (strncmp(line, "MemTotal:", 9) == 0) {
            sscanf(line, "MemTotal: %ld kB", &total_ram_kb);
        }
        // Pega a RAM disponível para uso
        if (strncmp(line, "MemAvailable:", 13) == 0) {
            sscanf(line, "MemAvailable: %ld kB", &free_ram_kb);
        }
    }

    fclose(file);

    // Converte os valores de kilobytes (kB) para gigabytes (GB)
    double total_gb = (double)total_ram_kb / (1024 * 1024);
    double free_gb = (double)free_ram_kb / (1024 * 1024);

    printf("=== INFORMAÇÕES DA RAM ===\n");
    printf("RAM Total:       %.2f GB\n", total_gb);
    printf("RAM Disponível:  %.2f GB\n\n", free_gb);
}