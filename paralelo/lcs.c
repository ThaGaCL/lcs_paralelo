#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>

#ifndef max
#define max( a, b ) ( ((a) > (b)) ? (a) : (b) )
#endif


#define NUM_THREADS 4
int nthreads = 1;

typedef unsigned short mtype;

/* Read sequence from a file to a char vector.
 Filename is passed as parameter */

char* read_seq(char *fname) {
	//file pointer
	FILE *fseq = NULL;
	//sequence size
	long size = 0;
	//sequence pointer
	char *seq = NULL;
	//sequence index
	int i = 0;

	//open file
	fseq = fopen(fname, "rt");
	if (fseq == NULL ) {
		printf("Error reading file %s\n", fname);
		exit(1);
	}

	//find out sequence size to allocate memory afterwards
	fseek(fseq, 0L, SEEK_END);
	size = ftell(fseq);
	rewind(fseq);

	//allocate memory (sequence)
	seq = (char *) calloc(size + 1, sizeof(char));
	if (seq == NULL ) {
		printf("Erro allocating memory for sequence %s.\n", fname);
		exit(1);
	}

	//read sequence from file
	while (!feof(fseq)) {
		seq[i] = fgetc(fseq);
		if ((seq[i] != '\n') && (seq[i] != EOF))
			i++;
	}
	//insert string terminator
	seq[i] = '\0';

	//close file
	fclose(fseq);

	//return sequence pointer
	return seq;
}

mtype ** allocateScoreMatrix(int sizeA, int sizeB) {
	int i;
	//Allocate memory for LCS score matrix
	mtype ** scoreMatrix = (mtype **) malloc((sizeB + 1) * sizeof(mtype *));
	for (i = 0; i < (sizeB + 1); i++)
		scoreMatrix[i] = (mtype *) malloc((sizeA + 1) * sizeof(mtype));
	return scoreMatrix;
}

void initScoreMatrix(mtype ** scoreMatrix, int sizeA, int sizeB) {
	int i, j;

	// Inicialização sequencial
	// avg_time = 0,0015 total 
	//Fill first line of LCS score matrix with zeroes
	for (j = 0; j < (sizeA + 1); j++)
		scoreMatrix[0][j] = 0;

	//Do the same for the first collumn
	for (i = 1; i < (sizeB + 1); i++)
		scoreMatrix[i][0] = 0;

	// Inicialização em paralelo 
	// avg_time = 0,0025 total
	// Aparentemente não há ganho de performance, pelo contrário, há perda (testar no pc do dinf pois é nele que será baseado o relatório)
	// #pragma omp parallel for private(j)
	// for (j = 0; j < (sizeA + 1); j++)
	// 	scoreMatrix[0][j] = 0;

	// #pragma omp parallel for
	// for (i = 1; i < (sizeB + 1); i++)
	// 	scoreMatrix[i][0] = 0;
}


void printMatrix(char * seqA, char * seqB, mtype ** scoreMatrix, int sizeA,
	int sizeB) {
int i, j;
//print header
printf("Score Matrix:\n");
printf("========================================\n");

//print LCS score matrix allong with sequences

printf("    ");
printf("%5c   ", ' ');

for (j = 0; j < sizeA; j++)
	printf("%5c   ", seqA[j]);
printf("\n");
for (i = 0; i < sizeB + 1; i++) {
	if (i == 0)
		printf("    ");
	else
		printf("%c   ", seqB[i - 1]);
	for (j = 0; j < sizeA + 1; j++) {
		printf("%5d   ", scoreMatrix[i][j]);
	}
	printf("\n");
}
printf("========================================\n");
}

int LCS(mtype **scoreMatrix, int sizeA, int sizeB, char *seqA, char *seqB) {
    int max_k = sizeA + sizeB; // Calcula o indice maximo da diagonal
    #pragma omp parallel num_threads(nthreads)// Inicia a paralelização
    {
		// Começa com 2 pq a primeira diagonal valida é a 2 (i=1,j=1) já que a primeira linha e coluna são inicializadas com 0
        for (int k = 2; k <= max_k; k++) {
			// Encontra o inicio e fim da diagonal valida
            int start_i = (k > sizeA) ? k - sizeA : 1;
            int end_i = (k - 1 < sizeB) ? k - 1 : sizeB;
            #pragma omp for schedule(auto)
            for (int i = start_i; i <= end_i; i++) {
                int j = k - i; // Calcula a coluna correspondente pra diagonal k
                if (seqA[j-1] == seqB[i-1]) { // Ve se deu o match 
                    scoreMatrix[i][j] = scoreMatrix[i-1][j-1] + 1; // Se sim, soma 1 ao score da diagonal anterior
                } else {
                    scoreMatrix[i][j] = max(scoreMatrix[i-1][j], scoreMatrix[i][j-1]); // Se não, pega o maximo entre a diagonal anterior e a linha anterior
                }
				// printMatrix(seqA, seqB, scoreMatrix, sizeA, sizeB);
			}
        }
    }
    return scoreMatrix[sizeB][sizeA];
}

void freeScoreMatrix(mtype **scoreMatrix, int sizeB) {
	int i;
	for (i = 0; i < (sizeB + 1); i++)
		free(scoreMatrix[i]);
	free(scoreMatrix);
}

int main(int argc, char ** argv) {
	// sequence pointers for both sequences
	char *seqA, *seqB;

	// sizes of both sequences
	int sizeA, sizeB;

	if(!argv[1]){
		printf("Uso correto: %s <num_threads>\n", argv[0]);
		exit(1);
	}
	nthreads = atoi(argv[1]);

	if (nthreads < 1) {
		printf("Number of threads must be greater than 0.\n");
		exit(1);
	}

	//read both sequences
	seqA = read_seq("fileA.in");
	seqB = read_seq("fileB.in");

	//find out sizes
	sizeA = strlen(seqA);
	sizeB = strlen(seqB);

	// allocate LCS score matrix
	mtype ** scoreMatrix = allocateScoreMatrix(sizeA, sizeB);

	//initialize LCS score matrix
	initScoreMatrix(scoreMatrix, sizeA, sizeB);

	//fill up the rest of the matrix and return final score (element locate at the last line and collumn)
	mtype score = LCS(scoreMatrix, sizeA, sizeB, seqA, seqB);

	/* if you wish to see the entire score matrix,
	 for debug purposes, define DEBUGMATRIX. */
#ifdef DEBUGMATRIX
	printMatrix(seqA, seqB, scoreMatrix, sizeA, sizeB);
#endif

	//print score
	printf("\nScore: %d\n", score);

	//free score matrix
	freeScoreMatrix(scoreMatrix, sizeB);

	return EXIT_SUCCESS;
}
