# chess-project

Simple échiquier

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
