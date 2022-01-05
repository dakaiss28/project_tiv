# Projet Traitement images et vidéos

Groupe 7 : HU Romain - MALLAM GARBA Dakini - NICAUDIE Charlotte - ROBERT Mathis


## Liste des classes

Le projet se base sur plusieurs classes ayant un rôle spécifique.

**DataPathGenerator** : classe utilisée pour générer les chemins de fichiers correspondant aux images d'un dossier contenant la base à traiter.

**TextExtractionManager** : classe chargée de l'extraction du texte d'une image correspondant à l'identifiant d'un formulaire en utilisant un algorithme d'OCR.

**ImageRecognitionManager** : classe utilisée pour déterminer le label et la taille d'une image référençant une ligne d'un formulaire en s'appuyant sur l'algorithme ORB.

**SnippetExtractor :** classe utilisée pour extraire les snippets des images (les snippets sont les petits carrés sans les bords extraits des formulaires). Cette classe permet également de récupérer l'image avec uniquement l'ID et les images référençant les lignes (qui sont fournies aux classes d'analyse TextExtractionManager et ImageRecognitionManager).

**QualityChecker** : classe gérant l'évaluation des résultats (calcul de la précision et du rappel). Une méthode permet notamment, sur un nombre donné d'images tirées aléatoirement, de faire vérifier à l'utilisateur que le label et la taille extraites sont correctes, ce qui permet de faire des estimations semi-automatiques sur la qualité de notre algorithme.


## Explication de la méthode utilisée

### SnippetExtractor : extraction des snippets et des labels

1. Binarisation et extraction des contours externes 

Passage en niveaux de gris par transformation linéaire, puis application d’un flou gaussien pour éliminer une partie du bruit, suivi d’un seuil pour ne garder que les contours noirs, qui apparaissent alors en blanc. Ensuite, on extrait les contours externes des différents éléments qui la composent. 

2. Création de la grille de snippets

On cherche d'abord le premier snippet de l'image (en ne gardant que les carrés) puis pour créer la grille, on regarde les snippets les plus proches du premier snippet, on détermine alors 2 vecteurs, un pour se décaler d’une colonne, et un pour se décaler d’une ligne.

3. Extraction des labels
On extrait les positions des labels grâce aux vecteurs qui ont permis de construire la grille des snippets, grâce à une translation. Les labels sont reconnus puis utilisés pour l’annotation des différentes lignes.

Les snippets sont extraits ligne par ligne depuis l’image d’origine (avant transformation), en précisant le numéro de la page, du scripter, etc. Les snippets sont extraits individuellement grâce à leurs contours. On les transforme pour les remettre droits avant de leur retirer une marge et de les sauvegarder. 

### ImageRecognitionManager : Reconnaissance des références avec ORB (Oriented FAST and Rotated BRIEF)

ORB s’appuie sur deux autres algorithmes : FAST et BRIEF. FAST gère la partie pour la détection des points clés. BRIEF récupère des informations de FAST et s’occupe de la génération de descripteurs (on utilise notamment un “matcher” de type “Brute Force Hamming", un algorithme KNN, la distance de Hamming et le test de Lowe).

Une fois qu’on a tous ces résultats, on va mesurer la pertinence de l’image de la base que l’on compare à l’image à reconnaître. On se base premièrement sur un ratio très simple qui consiste à diviser les correspondances pertinentes (après filtrage) par l’ensemble des résultats obtenus avec le matching KNN.

De manière générale c’était suffisant, mais on a eu des confusions entre des labels similaires avec une rotation différente. Pour parer à cela, on détermine aussi la rotation entre l’image à traiter et l’image de référence en utilisant la matrice d’homographie que l’on normalise.


## Étapes générales de l'algorithme (main)

- Initialisation du QualityChecker (qui récupère des informations au cours de l'exécution)
- Chargement des chemins vers les formulaires de la base (DataPathGenerator)

===

Pour chaque image :
- Ouverture de l'image
- Extraction l'ID du formulaire avec l'OCR (TextExtractionManager)
- Extraction des labels de référence (et leur taille si présente) en 1ère colonne (SnippetExtractor)

Pour chaque ligne d'un formulaire :
  - Reconnaissance du label et de la taille (ImageRecognitionManager)
  - Extraction des snippets de toute la ligne avec le label et la taille donnés (SnippetExtractor)
  
- Répétition des étapes précédentes jusqu'à ce que toutes les images soient traitées.

===

- Lancement des mesures de qualité (QualityChecker)


## Résultats sur la base de test et la base finale

#### Base initiale (NicIcon)

**Résultats**
- Temps d'exécution total : 49,88 **minutes**
- Nombre de snippets à extraire : 24 500 ;
- Nombre de snippets à extraire : 24 451 ;
- Au vu de la quantité d'images très importante, nous n'avons pu déterminer des mesures de précision et de rappel sur cette base.

**Problème(s) rencontré(s)**
- Nous n'avons pas pu faire de vérification pour l'entièreté de la base. Nous avons essayé de traiter un maximum d'exemples (en considérant les cas particuliers) mais il nous était pas possible de tout vérifier. Il se peut donc qu'il demeure quelques confusions, notamment sur la reconnaissance des labels.

#### Base finale

**Résultats**
- Temps d'exécution total : 46,92 **secondes** ;
- Nombre de snippets à extraire : 420 ;
- Nombre de snippets à extraire : 413 ;
- Précision : 90% ;
- Rappel : 87%.

**Problème(s) rencontré(s)**
- Reconnaissance des labels : quelques confusions observées ;
- Extraction des snippets : un seul snippet non reconnu sur 420 attendus, soit 99,8 % de réussite.

Ces problèmes sont probablement dus à la qualité du scan (les scans de la base d'entraînement ayant été de bien meilleure qualité de manière générale) et donc à des erreurs de paramétrage. Notre algorithme semble assez robuste mais de nombreux points peuvent être améliorés, en particulier sur la reconnaissance d'image.
