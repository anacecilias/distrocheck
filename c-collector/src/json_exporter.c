#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/utsname.h>
#include <sys/statvfs.h>
#include "json_exporter.h"

// Função auxiliar para ler arquivos do /sys/
static void read_sys(const char *path, char *buf, size_t sz) {
    FILE *f = fopen(path, "r");
    if (f) {
        if (fgets(buf, sz, f)) buf[strcspn(buf, "\n")] = 0;
        fclose(f);
    } else {
        strcpy(buf, "Desconhecido");
    }
}

void export_to_json(const char *filename) {
    FILE *json = fopen(filename, "w");
    if (!json) {
        printf("Erro ao criar o arquivo %s\n", filename);
        return;
    }

    // --- Sistema & Kernel ---
    struct utsname sys_info;
    uname(&sys_info);
    char board_vendor[128], board_name[128];
    read_sys("/sys/class/dmi/id/board_vendor", board_vendor, sizeof(board_vendor));
    read_sys("/sys/class/dmi/id/board_name", board_name, sizeof(board_name));

    // --- CPU ---
    char cpu_model[256] = "Desconhecido";
    int cores = 0;
    FILE *f_cpu = fopen("/proc/cpuinfo", "r");
    if (f_cpu) {
        char line[256];
        while (fgets(line, sizeof(line), f_cpu)) {
            if (strncmp(line, "processor", 9) == 0) cores++;
            if (strncmp(line, "model name", 10) == 0 && strcmp(cpu_model, "Desconhecido") == 0) {
                char *colon = strchr(line, ':');
                if (colon) {
                    strncpy(cpu_model, colon + 2, sizeof(cpu_model) - 1);
                    cpu_model[strcspn(cpu_model, "\n")] = 0;
                }
            }
        }
        fclose(f_cpu);
    }

    // --- RAM ---
    long total_ram_kb = 0, free_ram_kb = 0;
    FILE *f_ram = fopen("/proc/meminfo", "r");
    if (f_ram) {
        char line[256];
        while (fgets(line, sizeof(line), f_ram)) {
            if (strncmp(line, "MemTotal:", 9) == 0) sscanf(line, "MemTotal: %ld kB", &total_ram_kb);
            if (strncmp(line, "MemAvailable:", 13) == 0) sscanf(line, "MemAvailable: %ld kB", &free_ram_kb);
        }
        fclose(f_ram);
    }
    double ram_total_gb = (double)total_ram_kb / (1024 * 1024);
    double ram_free_gb = (double)free_ram_kb / (1024 * 1024);

    // --- ARMAZENAMENTO ---
    struct statvfs stat;
    double disk_total_gb = 0, disk_free_gb = 0;
    if (statvfs("/", &stat) == 0) {
        disk_total_gb = (double)(stat.f_blocks * stat.f_frsize) / (1024 * 1024 * 1024);
        disk_free_gb = (double)(stat.f_bfree * stat.f_frsize) / (1024 * 1024 * 1024);
    }

    // --- MONTAGEM DO JSON ---
    fprintf(json, "{\n");
    fprintf(json, "  \"system\": {\n");
    fprintf(json, "    \"os\": \"%s\",\n", sys_info.sysname);
    fprintf(json, "    \"kernel\": \"%s\",\n", sys_info.release);
    fprintf(json, "    \"arch\": \"%s\",\n", sys_info.machine);
    fprintf(json, "    \"motherboard_vendor\": \"%s\",\n", board_vendor);
    fprintf(json, "    \"motherboard_model\": \"%s\"\n", board_name);
    fprintf(json, "  },\n");

    fprintf(json, "  \"cpu\": {\n");
    fprintf(json, "    \"model\": \"%s\",\n", cpu_model);
    fprintf(json, "    \"cores\": %d\n", cores);
    fprintf(json, "  },\n");

    fprintf(json, "  \"ram\": {\n");
    fprintf(json, "    \"total_gb\": %.2f,\n", ram_total_gb);
    fprintf(json, "    \"available_gb\": %.2f\n", ram_free_gb);
    fprintf(json, "  },\n");

    fprintf(json, "  \"storage\": {\n");
    fprintf(json, "    \"total_gb\": %.2f,\n", disk_total_gb);
    fprintf(json, "    \"free_gb\": %.2f\n", disk_free_gb);
    fprintf(json, "  }\n");
    fprintf(json, "}\n");

    fclose(json);
    printf("Dados exportados para o arquivo '%s'.\n", filename);
}