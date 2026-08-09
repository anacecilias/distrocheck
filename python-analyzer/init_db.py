import sqlite3
import json
import os

DB_PATH = "data/distrocheck.db"
JSON_PATH = "data/distros.json"

def init_database():
    os.makedirs("data", exist_ok=True)
    conn = sqlite3.connect(DB_PATH)
    cursor = conn.cursor()

    print("Criando tabelas no SQLite...")

    # 1. Tabela de Distros
    cursor.execute('''
                   CREATE TABLE IF NOT EXISTS distros (
                                                          id INTEGER PRIMARY KEY AUTOINCREMENT,
                                                          name TEXT NOT NULL,
                                                          family TEXT NOT NULL,
                                                          sha256 TEXT UNIQUE NOT NULL,
                                                          min_ram_gb REAL NOT NULL,
                                                          min_cores INTEGER NOT NULL,
                                                          min_storage_gb REAL NOT NULL,
                                                          arch TEXT NOT NULL
                   )
                   ''')

    # 2. Tabela de Regras de Compatibilidade de Hardware
    cursor.execute('''
                   CREATE TABLE IF NOT EXISTS hardware_rules (
                                                                 id INTEGER PRIMARY KEY AUTOINCREMENT,
                                                                 category TEXT NOT NULL,
                                                                 pattern TEXT NOT NULL,
                                                                 distro_family TEXT,
                                                                 severity TEXT NOT NULL,
                                                                 title TEXT NOT NULL,
                                                                 description TEXT NOT NULL
                   )
                   ''')

    # Insere as regras padrão
    rules = [
        (
            'gpu', 'GT 610', 'RedHat/Fedora', 'CRITICAL',
            'GPU Nvidia Fermi / Driver Legado',
            'A GT 610 exige o driver descontinuado 390.xx. Distros com Wayland por padrão (como Fedora) podem apresentar tela preta.'
        ),
        (
            'gpu', 'GT 710', 'NixOS', 'WARNING',
            'GPU Nvidia Kepler Legada',
            'Requer drivers da série 470.xx. Verifique se o repositório da distro provê suporte a drivers legados.'
        ),
        (
            'wifi', 'RTL8821', 'Debian', 'WARNING',
            'Firmware Wi-Fi Proprietário',
            'Este chip Realtek precisa do pacote firmware-realtek. Certifique-se de usar a ISO do Debian que já inclui firmwares non-free.'
        )
    ]

    for r in rules:
        try:
            cursor.execute('''
                           INSERT INTO hardware_rules (category, pattern, distro_family, severity, title, description)
                           VALUES (?, ?, ?, ?, ?, ?)
                           ''', r)
        except sqlite3.IntegrityError:
            pass

    # Migra distros.json se existir
    if os.path.exists(JSON_PATH):
        with open(JSON_PATH, 'r', encoding='utf-8') as f:
            distros = json.load(f)

        for d in distros:
            try:
                cursor.execute('''
                               INSERT INTO distros (name, family, sha256, min_ram_gb, min_cores, min_storage_gb, arch)
                               VALUES (?, ?, ?, ?, ?, ?, ?)
                               ''', (
                                   d['name'], d.get('family', 'Linux'), d['sha256'].lower(),
                                   d['min_ram_gb'], d['min_cores'], d['min_storage_gb'], d['arch']
                               ))
            except sqlite3.IntegrityError:
                pass

    conn.commit()
    conn.close()
    print("Banco de dados atualizado com todas as tabelas e regras!")

if __name__ == "__main__":
    init_database()