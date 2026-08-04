#include <stdio.h>
#include <sys/statvfs.h>
#include "storage_reader.h"

void print_storage_info(void) {
    struct statvfs stat;

    // Checa as estatísticas do diretório raiz "/"
    if (statvfs("/", &stat) != 0) {
        printf("Erro ao ler informações do disco\n");
        return;
    }

    // Cálculo de espaço em Gigabytes (GB)
    unsigned long long total_bytes = stat.f_blocks * stat.f_frsize;
    unsigned long long free_bytes = stat.f_bfree * stat.f_frsize;

    double total_gb = (double)total_bytes / (1024 * 1024 * 1024);
    double free_gb = (double)free_bytes / (1024 * 1024 * 1024);

    printf("=== ARMAZENAMENTO (Diretório Raiz /) ===\n");
    printf("Espaço Total:     %.2f GB\n", total_gb);
    printf("Espaço Livre:     %.2f GB\n\n", free_gb);
}