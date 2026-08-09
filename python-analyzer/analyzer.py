import hashlib
import json
import os
import sqlite3
import sys

DB_PATH = "data/distrocheck.db"

def calculate_sha256(filepath):
    """Calcula o Hash SHA-256 de um arquivo grande em blocos de 4MB."""
    sha256_hash = hashlib.sha256()
    print("Calculando Hash SHA-256 da ISO... (Isso pode levar alguns segundos)")

    try:
        with open(filepath, "rb") as f:
            for byte_block in iter(lambda: f.read(4194304), b""):
                sha256_hash.update(byte_block)
        return sha256_hash.hexdigest().lower()
    except Exception as e:
        print(f"Erro ao ler a ISO: {e}")
        return None

def get_distro_by_hash(sha256_hash):
    """Consulta a distro no banco SQLite pelo Hash SHA-256."""
    if not os.path.exists(DB_PATH):
        print("Banco de dados não encontrado! Rode 'python3 init_db.py' primeiro.")
        return None

    conn = sqlite3.connect(DB_PATH)
    conn.row_factory = sqlite3.Row
    cursor = conn.cursor()

    cursor.execute("SELECT * FROM distros WHERE sha256 = ?", (sha256_hash,))
    distro = cursor.fetchone()
    conn.close()

    return distro

def check_hardware_alerts(hardware, distro_family):
    """Cruza os dados do C (GPU, Wi-Fi, CPU) com a tabela de regras do SQLite!"""
    if not os.path.exists(DB_PATH):
        return []

    conn = sqlite3.connect(DB_PATH)
    conn.row_factory = sqlite3.Row
    cursor = conn.cursor()

    # Busca todas as regras de alerta no banco
    cursor.execute("SELECT * FROM hardware_rules")
    rules = cursor.fetchall()
    conn.close()

    alerts_found = []

    # Extrai modelos coletados pelo C (com tratamento para campos nulos)
    gpu_model = hardware.get('gpu', {}).get('model', '') if isinstance(hardware.get('gpu'), dict) else ''
    cpu_model = hardware.get('cpu', {}).get('model', '')

    # Se o C mandou em formato de texto genérico ou dict
    system_info = str(hardware.get('system', ''))

    # Compara o hardware do PC com as regras cadastradas
    for rule in rules:
        pattern = rule['pattern'].lower()
        family_match = (rule['distro_family'] == 'ALL' or rule['distro_family'] in distro_family)

        # Checa se o padrão do alerta bate com alguma peça do PC
        if family_match:
            if pattern in gpu_model.lower() or pattern in cpu_model.lower() or pattern in system_info.lower():
                alerts_found.append(rule)

    return alerts_found

def analyze_iso(iso_path):
    # Procura pelo hardware.json gerado pelo C
    hardware_path = "../c-collector/output/hardware.json"
    if not os.path.exists(hardware_path):
        hardware_path = "../c-collector/output/hardware.json"

    if not os.path.exists(hardware_path):
        print("Arquivo 'hardware.json' não encontrado! Compile e rode o c-collector primeiro.")
        return

    # 1. Calcula o Hash da ISO
    iso_hash = calculate_sha256(iso_path)
    if not iso_hash:
        return

    # 2. Busca a distro no SQLite
    matched_distro = get_distro_by_hash(iso_hash)

    # 3. Lê o Hardware coletado pelo C
    with open(hardware_path, 'r', encoding='utf-8') as f:
        hardware = json.load(f)

    print("\n====================================================")
    print("      DISTROCHECK - ANALISADOR DE ISO & HARDWARE    ")
    print("====================================================\n")

    if matched_distro:
        print(f"ISO IDENTIFICADA COM SUCESSO!")
        print(f"Distribuição: {matched_distro['name']}")
        print(f"Família:      {matched_distro['family']}")
        print(f"Hash SHA-256: {iso_hash}\n")

        print("----------------------------------------------------")
        print("📊 VERIFICAÇÃO BÁSICA DE REQUISITOS (RAM / CPU / DISCO)")
        print("----------------------------------------------------")

        ram_ok = hardware['ram']['total_gb'] >= matched_distro['min_ram_gb']
        cores_ok = hardware['cpu']['cores'] >= matched_distro['min_cores']
        storage_ok = hardware['storage']['free_gb'] >= matched_distro['min_storage_gb']

        if ram_ok and cores_ok and storage_ok:
            print("REQUISITOS MÍNIMOS: ATENDIDOS COM SUCESSO!")
        else:
            print("ATENÇÃO: Seu PC pode ter gargalos de desempenho:")
            if not ram_ok:
                print(f"   └─ RAM: {hardware['ram']['total_gb']} GB (Exige: {matched_distro['min_ram_gb']} GB)")
            if not storage_ok:
                print(f"   └─ Disco: {hardware['storage']['free_gb']} GB livre (Exige: {matched_distro['min_storage_gb']} GB)")
            if not cores_ok:
                print(f"   └─ CPU: {hardware['cpu']['cores']} núcleos (Exige: {matched_distro['min_cores']})")

        # 4. Análise Avançada de Compatibilidade (Regras Especiais)
        print("\n----------------------------------------------------")
        print("🔍 ANÁLISE AVANÇADA DE DRIVERS E COMPATIBILIDADE")
        print("----------------------------------------------------")

        alerts = check_hardware_alerts(hardware, matched_distro['family'])

        if alerts:
            for alert in alerts:
                print(f" ALERTA: {alert['title']}")
                print(f"   └─ {alert['description']}\n")
        else:
            print("Nenhum conflito de driver ou incompatibilidade crítica de GPU ou rede detectada!")

    else:
        print("⚠ISO NÃO ENCONTRADA NO BANCO DE DADOS!")
        print(f"Hash gerado: {iso_hash}")
        print("Rode o script 'scraper_distros.py' para atualizar o banco com novas ISOs da internet")

if __name__ == "__main__":
    if len(sys.argv) > 1:
        path = sys.argv[1]
    else:
        path = input("Digite o caminho completo da ISO: ")

    analyze_iso(path)