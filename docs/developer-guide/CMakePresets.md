# Utilisation des CMake Presets pour le développement

Cette page explique comment utiliser les CMake Presets dans le projet Antares Simulator, en local, avec Docker, et dans
CLion. Elle détaille aussi la gestion du cache de build et du cache vcpkg.

## 1. Introduction

Les CMake Presets permettent de configurer facilement la génération du projet avec des paramètres prédéfinis. Pour plus
d’informations: [Documentation officielle CMake Presets](https://cmake.org/cmake/help/latest/manual/cmake-presets.7.html)

## 2. Utilisation simple des presets

Le projet fournit des fichiers `CMakePresets.json` et `CMakeUserPresets.json` dans le dossier `src/`. Pour configurer le
projet avec un preset:

```sh
cmake --preset=default
```

Pour lister les presets disponibles:

```sh
cmake --list-presets
```

## 3. Utilisation avec Docker

Dans les images Docker (voir `docker/gcc/Dockerfile` et `docker/clang/Dockerfile`), les variables d’environnement et les
chemins sont adaptés pour le cache et vcpkg. Pour utiliser un preset dans un conteneur:

```sh
docker run --rm -v "$PWD:/work" <image> cmake --preset=default
```

Les variables comme `VCPKG_ROOT`, `CCACHE_DIR`, etc. sont définies dans le preset et propagées dans l’environnement du
conteneur.

## 4. Utilisation dans CLion (avec ou sans Docker)

CLion supporte les CMake Presets:

- Ouvrir le projet dans CLion.
- Aller dans _File > Settings > Build, Execution, Deployment > CMake_.
- Sélectionner le preset désiré.

Pour utiliser Docker comme environnement de build, configurez le toolchain dans CLion et assurez-vous que le preset
référence le bon toolchain (voir la clé `vendor.jetbrains.com/clion.toolchain` dans le preset).

## 5. Caching du build

Le preset configure l’utilisation de `ccache`:

- Variable: `CMAKE_CXX_COMPILER_LAUNCHER=ccache`
- Répertoire du cache: `CCACHE_DIR=/tmp/deps/ccache`

Cela accélère les recompilations en réutilisant les objets compilés.

## 6. Caching vcpkg

Pour accélérer l’installation des dépendances vcpkg:

- Les binaires sont stockés dans `/tmp/deps/vcpkg_cache/binary-cache`.
- Les options d’installation sont configurées via `VCPKG_INSTALL_OPTIONS` dans le preset.
- Les variables d’environnement `VCPKG_BINARY_SOURCES` et `VCPKG_INSTALL_OPTIONS` permettent de réutiliser les binaires
  et d’éviter de recompiler inutilement.

## 7. Conseils et bonnes pratiques

- Modifiez ou ajoutez vos propres presets dans `CMakeUserPresets.json` (non versionné).
- Pour des builds reproductibles, utilisez les presets partagés dans `CMakePresets.json`.
- Vérifiez que les chemins de cache sont persistants si vous utilisez des volumes Docker.

---

Pour toute question ou problème, consultez la documentation officielle ou contactez les mainteneurs du projet.

