Bonjour monsieur, 

je vous renvoie mon projet CUDA, avec cette fois les fichiers contenant les fonctions CPU et un descriptif du projet, comme demandé dans votre mail.

Bonne journée et bonnes fêtes !

Rémi Cazoulat

Description du projet :
- Version CPU : Deux fonctions complémentaires à faire, une pour générer un bits_t* à partir d'un long, et une pour générer un long à partir d'un bits_t*.
- Version GPU : Générer des bits_t* en fonction de l'id du thread, les rapatrier sur CPU, et vérifier leur valeur sur CPU avec la fonction qui transforme un bits_t* en un long.

Execution du programme
- Rien de particulier.

Difficultés rencontrées :
- Création de la fonction CPU : 
	- Il a fallu que je trouve une façon pour que la création du bits_t* soit faite de manière à ce qu'elle soit adaptée à la fonction fournie bits_print() afin d'avoir un bel affichage, et de débugger correctement. Il a donc fallu que j'inverse la plupart des boucles, pour que dernier byte soit le byte le moins significatif.
- Passage de la fonction de CPU à GPU :
	- Pas de réelles difficultés.
- Passage du tableau bits_t** en mémoire globale :
	- J'ai pris un certain temps avant de bien comprendre la logique pour passer correctement la mémoire du CPU au GPU, et ensuite du GPU au CPU, pour faire la vérification sur CPU.
	- Une autre difficulté a été de gérer les fuites mémoires, et de bien placer les allocations, les free et cudaFree.

Remarques :
- J'ai testé les performances de mon kernel en passant seulement un tableau de char, et ensuite en passant un tableau de bits_t*. 
	- J'ai pu noter que l'allocation mémoire sur GPU est beaucoup plus longue en allouant sur GPU la mémoire nécessaire à un tableau de bits_t*. Je ne pense pas que ce soit surprenant, étant donné que le nombre d'opérations à réaliser est plus élevé que pour passer un tableau de char plus basique. 
	- Le temps d'exécution du kernel est aussi plus long quand celui-ci a un tableau de bits_t*. On peut supposer que c'est parce que la mémoire est plus fragmentée qu'avec un tableau de char, et qu'il est donc plus complexe d'accéder à la mémoire des chars du bits_t*.
	- Le rapatriement mémoire est aussi plus long, car il y a encore une fois plus d'opération à réaliser qu'avec un tableau de char.