# Projet XV6 – INF4097  
## Conception d’un Système d’Exploitation

**Année académique : 2024–2025**  
**Encadrant : Dr Adamou Hamza**

---

## Description du projet

Ce projet consiste en la modification du noyau pédagogique **xv6-riscv** afin d’implémenter, d’analyser et de tester plusieurs fonctionnalités avancées d’un système d’exploitation.  
L’objectif est de mieux comprendre les mécanismes internes du noyau, notamment la gestion des processus, l’ordonnancement et la gestion mémoire.

Les modifications portent à la fois sur le **noyau (kernel)** et sur des **programmes utilisateurs de test** permettant de valider les fonctionnalités implémentées.

---

## Fonctionnalités implémentées

### 1. Appel système `getprocinfo()`

Un nouvel appel système a été ajouté afin de permettre à un programme utilisateur d’obtenir des informations détaillées sur un processus.

Informations retournées :
- PID du processus
- État du processus
- Nombre de ticks CPU consommés
- Compteur d’appels système

L’appel système est implémenté dans le noyau et accessible depuis l’espace utilisateur via un programme de test dédié.

---

### 2. Ordonnanceur LowPower

L’ordonnanceur de xv6 a été modifié afin d’augmenter le **quantum d’exécution** :
- Quantum par défaut : **1 tick**
- Quantum modifié : **5 ticks**

Objectifs :
- Réduire le nombre de changements de contexte
- Améliorer les performances des processus CPU-bound

Des tests comparatifs ont montré :
- Une réduction d’environ **80 % des changements de contexte**
- Une légère amélioration du temps CPU par processus
- Un compromis assumé entre performance et réactivité

---

### 3. Allocation mémoire paresseuse (Lazy Allocation)

Une stratégie d’allocation mémoire paresseuse a été implémentée :
- La mémoire n’est pas allouée lors de l’appel à `sbrk`
- L’allocation réelle est effectuée lors du premier accès mémoire
- Les fautes de page sont interceptées et traitées par le noyau

Cette fonctionnalité permet :
- Une utilisation plus efficace de la mémoire
- Une meilleure compréhension du mécanisme des **page faults**

---

## Structure du dépôt

.
├── kernel/ # Code source du noyau modifié
├── user/ # Programmes utilisateur de test
├── docs/ # Rapports (PDF et LaTeX)
│ ├── rapport_complet.pdf
│ 
│ ├── rapport_synthese.pdf
|
├── Makefile # Makefile xv6 modifié
└── README.md # Documentation du projet


---

## Compilation et exécution

### Prérequis
- Ubuntu 20.04 ou 22.04
- QEMU 6.2
- Toolchain RISC-V (`gcc-riscv64-linux-gnu`)

### Compilation
```bash
make clean
make qemu

Tests (dans QEMU)

testprocinfo   # Test de l’appel système getprocinfo
testsched      # Test de l’ordonnanceur LowPower
testlazy       # Test de l’allocation paresseuse

Résultats expérimentaux (Scheduler)
Scénario	Avant	Après	Gain
Processus unique	45 ticks	48 ticks	+6.7 %
2 processus	68 ticks	72 ticks	+5.9 %
3 processus	92 ticks	98 ticks	+6.5 %
Changements/s	100	20	−80 %

Ces résultats confirment l’impact positif du quantum étendu sur la réduction des changements de contexte.
Problèmes rencontrés et solutions
1. Problèmes de compatibilité xv6 / QEMU

Symptômes :

    Démarrage incomplet de xv6

    Le shell ne démarre pas ou le système se bloque après :

    hart 2 starting

Cause identifiée :

    Incompatibilités entre certaines versions récentes de QEMU (≥ 7.x) et la version officielle de xv6-riscv utilisée comme base du projet

    Différences de comportement dans la gestion des interruptions et des fautes de page

Solution appliquée :

    Utilisation de QEMU 6.2, version recommandée dans la documentation xv6

    Tests réalisés sur Ubuntu 20.04 et 22.04

2. Problèmes liés à l’allocation paresseuse

Symptômes :

    Échec du boot après implémentation de la lazy allocation

    Les processus système init et sh sont tués suite à des fautes de page

Cause :

    La politique d’allocation paresseuse était appliquée indistinctement à tous les processus

Solution proposée :

    Exclusion des processus système critiques (init, sh)

    Gestion spécifique des page faults dans trap.c

    Limitation documentée et justifiée dans le rapport

3. Problèmes de compilation liés à l’API xv6

Problème : argint()

    argint() retourne void dans xv6

    Toute tentative de vérification de retour provoque une erreur

Solution :

argint(0, &pid);

Problème : formats printf avec adresses 64 bits

Cause :

    %p attend un pointeur, pas un entier 64 bits

Solution :

printf("va=%p\n", (void*)va);

4. Problèmes liés à l’ordonnanceur

Observation :

    Le quantum étendu favorise les processus CPU-bound

    Légère baisse de réactivité pour les processus interactifs

Décision :

    Choix assumé et analysé comme compromis performance / réactivité

    Justification détaillée dans le rapport

5. Difficultés de débogage

    Absence de débogueur noyau avancé

    Utilisation intensive de printf dans le noyau

    Programmes utilisateurs de test dédiés

    Analyse des messages noyau au démarrage

Documentation

Tous les rapports sont disponibles dans le dossier docs/ :

    rapport_complet.pdf : rapport détaillé

    rapport_synthese.pdf : version synthétique

Auteur

    Nom : Nanvou Franck

    Matricule : 18T2596

    Module : INF4097 – Conception d’un Système d’Exploitation

    Année : 2025

Licence

Ce projet est réalisé exclusivement à des fins académiques dans le cadre du cours INF4097.

