# 6ix_os

6ix_os est un projet de système d'exploitation personnalisé, développé à partir de zéro. Ce projet vise à créer un OS léger.

## Caractéristiques

- Bootloader
- Mode protégé 32 bits
- Gestion des interruptions (IDT)
- Pilotes de base (écran)
- Timer système

## Structure du projet

- `boot/` : Contient les fichiers assembleur pour le processus de démarrage
- `include/` : Fichiers d'en-tête partagés
- `src/` : Code source principal
  - `drivers/` : Pilotes de périphériques
  - `idt/` : Gestion des interruptions
  - `libc/` : Implémentation de fonctions de bibliothèque standard
  - `timer/` : Gestion du timer système

## Prérequis

- GCC cross-compiler pour i386
- NASM (Netwide Assembler)
- QEMU pour l'émulation (optionnel)

## Compilation

(TODO)

## Exécution

(TODO)