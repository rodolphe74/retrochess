# Retrochess
Play chess against computer in a pure retro style.

Chess Engine coded in C.

IHM made with love2d.

Works on Windows and Android.

<img src="phone_screen.jpg" alt="teasing" style="width:300px;"/>


# Qu'est ce qu'il nous faut pour qu'un ordinateur joue aux échecs ?
* Un générateur de coups en fonction de l'état de l'échiquier.
* Un algo minimax permettant permettant de construire un arbre des possibilités de jeu entre l'ordinateur et son adversaire.
* Une fonction d'évaluation qui donne la qualité de l'échiquier résultant d'un coup joué.
* Une IHM.

Le principe :

Etant l'ordinateur, je cherche à minimiser le coup de mon adversaire sur une recherche de profondeur 5:
```
Appeler MIN_MAX avec 5 et MIN
```

```
Fonction MINI_MAX(profondeur, min_ou_max)
  Si profondeur = 0 -> retourner l'évaluation de l'échiquier

  Si MAX
    Sauvegarder l'échiquier
    Générer les coups possibles pour l'échiquier
      Pour chaque coup
        Jouer le coup
        Appeler récursivement la fonction MINI_MAX avec profondeur - 1 et MIN
        Récupérer l'evaluation du coup joué au retour de la fonction
        Garder l'évaluation MAX
        Restaurer l'échiquier sauvegardé
      Fin Pour
      Retourner l'évaluation MAX

  Si MIN
    Générer les coups possibles pour l'échiquier
      Pour chaque coup
        Jouer le coup
        Appeler récursivement la fonction MINI_MAX avec profondeur - 1 et MAX
        Récupérer l'evaluation du coup joué au retour de la fonction
        Garder l'évaluation MIN
        Restaurer l'échiquier sauvegardé
      Fin Pour
      Retourner l'évaluation MIN
Fin fonction
```

Pour en savoir plus:

* https://fr.wikipedia.org/wiki/Programming_a_Computer_for_Playing_Chess
* https://fr.wikipedia.org/wiki/Algorithme_minimax
* [ProgrammingaComputerforPlayingChess.pdf](./ProgrammingaComputerforPlayingChess.pdf) 


# Dépendances
* Le moteur est développé en C.
* Il utilise un générateur de coups de type ["BitBoard"](https://github.com/pioz/chess), autrement dit les échiquiers sont codés sur 64 bits et les calculs des trajectoires de pièces se font avec des opérateurs bit à bit. (https://www.chessprogramming.org/Bitboards).
* Le stockage en mémoire utilise des [Containers](https://github.com/bkthomps/Containers)
* L'interface est développée en [Lua](https://www.lua.org/) avec [Löve2D](https://love2d.org/), un framework 2D, qui facilite le portage entre différentes machines.
* Le look console provient de [LV100](https://github.com/Eiyeron/LV-100). 
* Le style retro provient de [moonshine](https://github.com/vrld/moonshine), une api permettant de créer des effets avec des shaders GLSL en lua.
