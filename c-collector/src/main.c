#include <stdio.h>
#include "json_exporter.h"

int main(void) {
    printf("===========================================\n");
    printf("   DISTROCHECK - GERADOR DE HARDWARE JSON  \n");
    printf("===========================================\n\n");

    // Gera o arquivo hardware.json na raiz da execução
    export_to_json("hardware.json");

    return 0;
}