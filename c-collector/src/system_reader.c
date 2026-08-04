#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/utsname.h>
#include "system_reader.h"

// Função auxiliar para ler uma única linha de arquivos do /sys/
void read_sys_file(const char *path, char *buffer, size_t size) {
    FILE *f = fopen(path, "r");
    if (f != NULL) {
        if (fgets(buffer, size, f) != NULL) {
            buffer[strcspn(buffer, "\n")] = 0; // Remove quebra de linha
        }
        fclose(f);
    } else {
        strcpy(buffer, "Não disponível");
    }
}

void print_system_info(void) {
    struct utsname sys_info;

    printf("=== INFORMAÇÕES DO SISTEMA & KERNEL ===\n");
    if (uname(&sys_info) == 0) {
        printf("Sistema Operacional: %s\n", sys_info.sysname);
        printf("Versão do Kernel:    %s\n", sys_info.release);
        printf("Arquitetura:         %s\n", sys_info.machine);
    }

    // Leitura da Placa-Mãe via /sys/class/dmi/id/
    char vendor[128], board_name[128];
    read_sys_file("/sys/class/dmi/id/board_vendor", vendor, sizeof(vendor));
    read_sys_file("/sys/class/dmi/id/board_name", board_name, sizeof(board_name));

    printf("Fabricante Placa:    %s\n", vendor);
    printf("Modelo Placa-Mãe:    %s\n", board_name);

    // Checa Bateria (se for Notebook)
    char capacity[16];
    read_sys_file("/sys/class/power_supply/BAT0/capacity", capacity, sizeof(capacity));
    if (strcmp(capacity, "Não disponível") != 0) {
        printf("Bateria (BAT0):      %s%%\n", capacity);
    } else {
        printf("Dispositivo:         Desktop (Sem bateria detectada)\n");
    }
    printf("\n");
}