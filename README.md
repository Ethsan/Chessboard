# chess-project

Ce projet d'échec utilise les bitboards pour représenter un échiquier
permettant une manipulation efficace en temps et en mémoire de celui-ci.
Le plus long dans ce projet aura été l'implémentation de l'échiquier en
lui-même mais elle permet une vrai séparation entre la logique de l'échiquier
et la logique d'ensemble d'une partie. Après cette étape de passer, il devient
facile d'implémenter des comportements plus poussé pour les joueurs comme un
ordinateur où l'implémentation devient alors très facile [cf player_bot.cpp].

Le projet a été pour être le plus modulable possible et pouvoir l'améliorer
facilement. Je n'ai pas pu le temps d'implémenter une interface graphique mais
un démultiplexeur est implémenter pour démontrer cette modularité pour les views.
J'ai cependant eu le temps d'implémenter deux ordinateurs l'un jouant des
coups aléatoires et l'autre utilisant un algorithme de minmax mettant ainsi à
profit l'utilisation des bitboards.

## Construire le projet

```bash
mkdir build
cd build
cmake .. && cmake --build .
```

## Tester

Pour tester :

```bash
cd build
cmake .. & cmake --build .
ctest .
```

Pour ajouter des tests ajouter les dans le CMakeLists.txt ils seront
automatiquement exécutés par les jobs.
