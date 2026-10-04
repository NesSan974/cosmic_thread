# 🧵 Cosmic Thread

> Une petite démo interactive en C autour des **threads**, de la **synchronisation** et d'un système de **production de ressources**.

## 📖 Description

**Cosmic Thread** est une démo interactive écrite en C, conçue comme un petit jeu de crafting permettant d'expérimenter concrètement la programmation concurrente avec les threads POSIX.

Au démarrage, le programme crée un pool fixe de **4 threads workers**. Le joueur peut ensuite lancer différentes commandes depuis le terminal afin de lancer la fabrication de matériaux.

Chaque action de crafting génère une tâche qui est placée dans une **file FIFO**, implémentée sous la forme d'un **buffer circulaire**. Lorsqu'une nouvelle tâche est ajoutée, les threads workers sont réveillés grâce à une **variable de condition**. Un worker récupère alors une tâche, accède aux ressources partagées de manière synchronisée, puis effectue le crafting.

Le temps de fabrication est volontairement simulé par une attente afin de rendre l'activité des threads visible dans le terminal.

Le projet dispose également d'un petit **TUI (Terminal User Interface)** minimalist permettant de suivre l'état des ressources, de la file de tâches et des workers.

```text
                              DUST    |    ROCK|   METAL|   LIGHT| CRYSTAL|  METOR CORE
                              461     |       1|       0|      45|       2|           0

w0: CRAFTING_ROCK                      'a': 3 × dust                                -> 1 × rock           
w1: CRAFTING_METEOR_CORE               'z': 3 × rock + 1 × dust                    -> 1 × metal          
w2: CRAFTING_CRYSTAL                   'e': 2 × dust + 1 × light                   -> 1 × crystal        
w3: NOT_ENOUGHT_RESSOURCES             'r': 2 × metal + 2 × crystal + 1 × rock    -> 1 × meteor_core    
                                  'space' : to have random task, can find light and dust

Next Task (tot : 4):
- 0x55cc2feeec71
- 0x55cc2feeed30
- 0x55cc2feeeba4
- 0x55cc2feeedfd
- (nil)


                             'q' for quit

```

---

## 🎯 Objectif

L'objectif principal de Cosmic Thread n'est pas de créer un jeu complet, mais d'utiliser une mécanique de crafting simple pour mettre en pratique plusieurs concepts importants de la programmation concurrente en C :

- Création et utilisation de threads POSIX
- Thread pool
- Modèle producteur / consommateurs
- Mutex et sections critiques
- Variables de condition
- File FIFO
- Buffer circulaire
- Gestion de ressources partagées
- Simulation de travail concurrent
- Affichage d'un état partagé dans un terminal


Le projet a principalement été conçu comme un terrain d'expérimentation pour observer les problèmes et mécanismes liés à la concurrence.

Cosmic Thread permet ainsi de mettre en pratique des concepts qui sont souvent plus abstraits, lorsqu'ils sont étudiés uniquement de manière théorique.

## 🧵 Thread Pool

Cosmic Thread utilise un pool fixe de **quatre threads**.

Plutôt que de créer un nouveau thread à chaque demande de crafting, le programme réutilise les mêmes workers.

Le fonctionnement d'un worker peut être résumé ainsi :

```text
Tant que le programme fonctionne :

    1. Attendre qu'une tâche soit disponible
    2. Récupérer une tâche dans la file
    3. Récupérer les ressources nécessaires
    4. Effectuer le crafting
    5. Produire le nouveau matériau
    6. Revenir en attente d'une nouvelle tâche
```

Cette approche permet de séparer la **création des threads** de l'**exécution des tâches**.

---

## 📦 File FIFO

Les demandes de crafting sont stockées dans une **file FIFO**.

La file est implémentée avec un **buffer circulaire**. Deux positions permettent notamment de suivre les éléments à lire et la prochaine position disponible pour écrire.

Lorsqu'une extrémité du buffer est atteinte, les indices reviennent au début du tableau.

Cela permet de réutiliser efficacement l'espace disponible sans déplacer les éléments déjà présents dans la file.

Comme la file est partagée entre plusieurs threads, ses accès sont protégés par des mécanismes de synchronisation.

---

## 🔐 Synchronisation

La synchronisation est au cœur du projet.

Plusieurs threads peuvent accéder simultanément aux mêmes ressources. Sans protection, cela pourrait provoquer des **race conditions** et rendre l'état du programme incohérent.

### Mutex

Les mutex sont utilisés pour protéger les données partagées et les sections critiques.

Lorsqu'un worker doit modifier une ressource partagée, il verrouille le mutex, effectue son opération, puis libère le mutex.

Cela garantit qu'un autre thread ne puisse pas modifier simultanément les mêmes données pendant l'opération protégée.

### Variables de condition

Les variables de condition permettent aux workers d'attendre efficacement l'arrivée de nouvelles tâches.

Lorsqu'un worker constate que la file est vide, il se met en attente.

## ⚒️ Crafting

Le système repose sur une liste de matériaux pouvant être transformés les uns en les autres.

Une commande utilisateur déclenche un nouveau travail de crafting.

Le déroulement général est :

```text
Commande utilisateur
        │
        ▼
Ajout tâche dans la FIFO
        │
        ▼
Réveil d'un worker
        │
        ▼
Récupération des ressources
        │
        ▼
Simulation du craft
        │
        ▼
Mise à jour des ressources
```

Le temps de fabrication est volontairement simulé afin de rendre le fonctionnement concurrent observable.


## 🖥️ Interface

Cosmic Thread possède une **TUI minimaliste**.

L'interface permet notamment de visualiser l'état du système pendant que les workers travaillent.

L'idée est de rendre visibles des concepts qui sont normalement difficiles à observer directement :

- les workers en attente ;
- les workers en train de travailler ;
- les tâches présentes dans la file ;
- l'évolution des ressources ;
- l'activité du système lors de plusieurs commandes successives.


## 🎮 Commandes

Les commandes sont déclenchées au clavier.

| Touche |  Action     |              Retire               | Ajoute
|:------:|-------------|-----------------------------------|-----------|
| 'a' | tache de craft | 3 × dust 	                       |1 × rock   |
| 'z' | tache de craft | 3 × rock + 1 × dust               |1 × metal  |
| 'e' | tache de craft | 2 × dust + 1 × light              |1 × crystal|
| 'r' | tache de craft | 2 × metal + 2 × crystal + 1 × rock| 1 × meteor core|
| `ESPACE` | ajoute une tache aléatoire | rien | une tache |

Chaque touche correspond à une action de crafting définie par le programme.

## 🛠️ Technologies

**Langage**

- C

**Principales fonctionnalités utilisées**

- `pthread`
- Mutex
- Variables de condition
- Structures de données
- Buffer circulaire
- Entrée clavier
- Caractère d'échappement ANSI

---

## 🔨 Compilation

Le projet utilise les outils classiques de compilation C ainsi que la bibliothèque `pthread`.

### Compiler

```bash
gcc *.c -o cosmic_thread
```

### Lancer

```bash
./cosmic_thread
```


## 🚀 Pistes d'amélioration

Quelques évolutions possibles :

- [ ] Rendre le nombre de workers configurable à l'exécution
- [ ] Ajouter des priorités aux tâches
- [ ] Améliorer le TUI
- [ ] Afficher des statistiques sur les workers
- [ ] Afficher le temps de traitement des tâches
- [ ] Ajouter une gestion complète de l'arrêt du thread pool
- [ ] Ajouter des tests de charge
- [ ] Tester le programme avec **ThreadSanitizer**

