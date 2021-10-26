# Retrochess
Play chess against computer in a pure retro style

Chess Engine coded in C

IHM made with love2d

Works on Windows and Android 

<img src="phone_screen.jpg" alt="teasing" style="width:300px;"/>


# Qu'est ce qu'il nous faut pour qu'un ordinateur joue aux échecs ?
* Un générateur de coups en fonction de l'état de l'échiquier
* Un algo minimax permettant permettant de construire un arbre des possibilités de jeu entre l'ordinateur et son adversaire
* Une fonction d'évaluation qui donne la qualité de l'échiquier résultant d'un coup joué
* Une IHM

Le principe :
```
Fonction MINI_MAX(profondeur, min_ou_max)
  Si profondeur = 0 -> retourner l'évaluation de l'échiquier

  Si MAX
    Sauvegarder l'échiquier
    Générer les coups possibles pour l'échiquier
      Pour chaque coup
        Jouer le coup
        Appeler récursivement la fonction MINI_MAX avec profondeur - 1 et MIN
        Recuperer l'evaluation du coup joué au retour de la fonction
        Garder l'évaluation MAX
      Fin Pour
      Restaurer l'échiquier sauvegardé
      Retourner l'évaluation MAX

  Si MIN
    Générer les coups possibles pour l'échiquier
      Pour chaque coup
        Jouer le coup
        Appeler récursivement la fonction MINI_MAX avec profondeur - 1 et MAX
        Recuperer l'evaluation du coup joué au retour de la fonction
        Garder l'évaluation MIN
      Fin Pour
      Restaurer l'échiquier sauvegardé
      Retourner l'évaluation MIN
Fin fonction
```



