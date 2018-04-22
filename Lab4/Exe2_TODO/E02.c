#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

void doubleMatrixPrint(double **mat, int m, int n);

int main(int argc, char **argv) {
/* Check if params from command line are the exact ones*/
	if(argc < 2) {
        printf ("usage: %s <k>\n", argv[0]);
        exit(EXIT_FAILURE);
    	}
int k;
double *v1,*v2;
double **mat;


 k = atoi(argv[1])

	if(k==0){
	printf("Provide a non-zero parameter!\n");
	exit(EXIT_FAILURE);
	}
 
	//allocate memory for v1,v2,mat
	v1= malloc(k* sizeof(double));
	v2= malloc(k* sizeof(double));
	mat= malloc(k* sizeof(double*));

	// assign values to arrays and to the matrix
	for (i = 0; i < k; i++) {
		v1[i] = ((double)rand()) / INT_MAX - 0.5;
		v2[i] = ((double)rand()) / INT_MAX - 0.5;
		mat[i] = malloc(k * sizeof(double));
		for (j = 0; j < k; j++) {
			mat[i][j] = ((double)rand()) / INT_MAX - 0.5;
		}
	}

	

	
  
    return 0;
}


double **doubleVettToVerticalMatrix(double *vett, int dim) {
	int i;
	double **mat = malloc(dim * sizeof(double *));
	for (i = 0; i < dim; i++) {
		mat[i] = malloc(dim * sizeof(double));
		mat[i][0] = vett[i];
	}
	return mat;
}


void doubleMatrixPrint(double ** mat, int m, int n) {
	int i, j;
	for (i = 0; i < m; i++) {
		for (j = 0; j < n; j++) {
			printf("%f\t", mat[i][j]);
		}
		printf("\n");
	}


