https://github.com/mucherino/teaching

# CM 1

Moyens de récupérer l'id d'un thread :
- `blockIdx.x/y/z` : récupère l'id du block courant
- `threadIdx.x/y/z` : récupère l'id du thread DANS le block
- `gridDim.x/y/z` : récupère la dimension de la grille (en terme de block)
- `blockDim.x/y/z` : récupère la dimension d'un block (en terme de thread)

Trucs à savoir :
- `void` : rien
- `void*` : pointeur à un type quelconque
- `void**` : point à un pointeur à un type quelconque (=tableau de pointeurs de types quelconque)

Exemple de syntaxe CUDA :
- `gpu_kernel<<<nblocks, nthreads>>>(args)`
pour synchroniser tous les blocks après l'appel d'un kernel : `cudaDeviceSynchronize()`

# CM 2
Optimisation problem : find the "global optimum" of f(x)
g(x) = f(x) if x >= 0, f(0) - x if x < 0  

Meta-heuristic methods (not gradient descent)
- Genetic algorithms
- ant colony optimisations
- Simulated annealing (1983) -> Quantum technology (qbit -> light, atoms, ions, ...)
- D-Wave : "Quantum annealer"
- A guy in Japan said : Digital annealer (with transistors)
- Taiwan : digital annealer with GPUs

# CM 4

Vector sum :
Pour faire des calculs coalesced, si chaque thread a m opérations à traiter, il vaut mieux qu'il traite la n-ième opération (n étant son id) fois i (i = 0; i < m; i ++) que de traiter la n-ième opération plus i.
# CM 20/11/2024

```c
if (cell[i][j] == 0) { // dead
	if(neighbours[i][j] == 3) {
		newcell[i][j] == 1; // alive
	} else {
		newcell[i][j] == 0; // dead
	}
} else {
	if(neighbours[i][j] < 2 || neighbours[i][j] > 3) {
		newcell[i][j] == 0; // dead
	} else {
		newcell[i][j] == 1; // alive
 	}
}
```

SIMD : Single Instruction Multiple Data
- une seule unité de contrôle
- traite en parallèle plusieurs données
- regarder "vectorial computing"
- Warp de GPU NVIDIA est en mode SIMD / registres d'un core


Deux bits : x et y
XOR pour la sortie, AND pour la retenue

Si on utilise booleen : 8 bits pris pour 1 bit utile -> d'utilisation de SIMD difficile
On peut utiliser 1 byte pour représenter 8 cases
Pour regarder les voisins, on regarde les 2 bits voisins et les 2 bytes voisins

| i - 1 : |  1  |  0  |  0  |  1  |  0  |  0  |  1  |  0  |
| ------- | :-: | :-: | :-: | :-: | :-: | :-: | :-: | :-: |
| i :     |  0  |  1  |  0  |  1  |  0  |  0  |  0  |  1  |
| i + 1 : |  0  |  0  |  1  |  0  |  1  |  1  |  0  |  1  |

Comment faire ? 
```c
x = cell[i - 1];                             //  i/ j
y = (cell[i - 1] << 1) | (cell[i - 1] >> 7); // Voisin -1/-1
z = (cell[i - 1] >> 1) | (cell[i - 1] << 7); // Voisin -1/+1
sum1 = x^y; // ^ : XOR
carry1 = x&y; // & : AND
sumUpperRow = sum1^z;
carryUpperRow = carry1 | (sum1&z);
x = cell[i + 1];
y = (cell[i + 1] << 1) | (cell[i + 1] >> 7); // Voisin +1/-1
z = (cell[i + 1] >> 1) | (cell[i + 1] << 7); // Voisin +1/+1
sum1 = x^y;
carry1 = x&y;
sumLowerRow = sum1^z;
carryLowerRow = carry1 | (sum1&z);
y = (cell[i] << 1) | (cell[i] >> 7) // Voisin 0/-1
z = (cell[i] >> 1) | (cell[i] << 7) // Voisin 0/+1
sumCentralRow = y^z;
carryCentralRow = y&z;
```

On somme :
carry | ~~sumUpperRow~~
carry | ~~sumCentralRow~~
carry | ~~sumLowerRow~~
carry | sum
Les 4 carry sont sur 3 bits et la sum sur 1 bit (mais on a 4 bytes en sortie !) -> On rassemble les bytes pour former 1 byte avec les 4 bits les moins significatifs contenant les 4 bits intéressants :

```c
if(bit3 is 1) { // voisins : 8
	dead
} else if (bit2 is 1) { // voisins : 4 - 5 - 6 - 7
	dead
} else if (bit1 is 1) { // voisins : 2 - 3
	if(cell is 0) { // cellule actuellement morte
		if(bit0 is 1) { // voisins : 3 
			alive // On rend la cellule vivante
		}
	} else { // cellule vivante
		alive // on la laisse vivante
	}
} else { // nombre de voisins : 0 - 1
	dead
}

// EQUIVALENT A :
newcell = ~bits3 & ~bit2 & (bit1 & (cell | (~cell & bit0))); // ~ : NOT pour bits
```

Attention :
- Quand il y a if/else if/else, le Warp CUDA n'aime pas car ça découpe le code en plusieurs étapes (une étape par bloc) -> séquenciel (pas très pertinent pour jeu de la vie car synchro globale à chaque génération) -> donc c'est pas mal la syntaxe sans if pour la newcell

# CM 27/11/2024


CM format :
- lecture 1 : subset sum 1D grid
	- every thread
	- has one unique identifier
- lecture 2 and 3 : 
	- operators on matrices
	- 2D -> every thread has 2 identifiers (idx, idy)

NVIDIA GPU Architecture :
- blocks (`blockIdx {x, y}`)
	- warps : threads in warps are controlled by one controlling unit -> perfect synchronisation
		- threads (`threadIdx {x, y, z}`)

Coalesced memory -> faster :
- when the warp doesn't perform a memory switch to compute all his threads -> not sequential

Shared memory -> twice as fast for matrices multiplications :
- shared memory within a block (not perfectly synchronized like in a warp, have to call `__syncthreads();`)

Banked memory -> even faster than classic shared memory :
- shared memory is in form of banks, and when 2 threads access to the same bank at the same time -> sequential

# CM 05/12/2024
Lecture 4 : Sierpinski fractal on GPU

CUDA Streams : Stream allows us to reserve only a part of the GPU

Il y a aussi les CUDA graphs -> creation de graphes de kernels
- cudaGraphs
- assigne automatiquement les dépendances (plus besoin de `cudaDeviceSynchronize()`)
- Un seul appel, comme un seul kernel

Test :
- questions sur le cours
- questions sur tp jeu ( + copiés-collés)
# CM 09/12/2024

Deadline for project submission
Friday 20/12 - 23:59
Email -> CFile, CUDAFile, any comments in the email

#### Project
first, improve and optimize code in sequential

```c
bits_t
{
	size_t nbytes;
	short ignore;
	unsigned char *bytes;
}
```

On GPU, only sending `bytes` ?

Structure on CPU :
`bits_t**` 
- `bits_t*`
	- `nbytes`
	- `ignore`
	- `bytes (unsigned char*)`
		- `unsigned char`

Structure on GPU : 
allocate with `CudaMalloc` ?
`bits_t **gpu_side`
Solution  :
- use `CudaMalloc` to init `gpu_side`.
- use `CudaMalloc` to generate all `bits_t` instances in `gpu_side`.
- use `CudaMalloc`  to generate all `unsigned char *` in `bits_t` instances.
- allows us to use `bits_t` struct in kernels

Copying back bytes arrays when we only send bytes array on GPU (with constant nbytes and ignore for every bits_t) :

`for(size_t k = 0; k < n; k ++) cudaMemcpy(cpu_side[k]->byte, &gpu_side[k*nbytes], nbytes*sizeof(unsigned char), cudaMemcpyDeviceToHost);`

# Projet 
  
Conversions en unsigned long : il y a deux méthodes à développer.

- La première doit construire une nouvelle instance de bits_t en utilisant les premières n bits moins significatifs d’un nombre entier stocké dans une variable de type “unsigned long”. 
    
- La deuxième doit transformer, si possible, une chaîne de bits en une variable de type “unsigned long”.
    
bits_t* bits_from_long(size_t n,unsigned long value);

unsigned long bits_to_long(bits_t *bits);

Sur GPU : utilisez la première méthode pour générer des chaînes de bits à partir des identifiants de chaque thread. Renvoyer ensuite l’information au CPU, qui s’occupera de reconvertir en unsigned long, et de vérifier qu’il n’y pas d’identifiants manquants.
