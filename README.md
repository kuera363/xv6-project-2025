# Projet XV6 – INF4097

Projet de modification du noyau xv6 dans le cadre du cours
**INF4097 – Conception d’un Système d’Exploitation (2025)**

## Fonctionnalités implémentées
- Appel système `getprocinfo()`
- Ordonnanceur LowPower (quantum étendu)
- Allocation mémoire paresseuse (Lazy Allocation)

## Structure du dépôt
- `kernel/` : code noyau modifié
- `user/` : programmes de test
- `docs/` : rapports PDF

## Compilation
```bash
make clean
make qemu

Tests

Dans QEMU :

testprocinfo
testsched
testlazy

Auteur

Nom : Nanvou Franck

Module : INF4097

Année : 2025
