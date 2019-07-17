#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define MASTER 0

#define N 1E7
#define d 1E-7
#define d2 1E-14

/*
 Specifica sintattica
	trapezio(int, int) -> double
 Specifica semantica
 	trapezio(start, finish) -> d
	start: rappresenta il limite inferiore del range di input che ogni processo deve eseguire
	finish: rappresenta il limite superiore
	d: risultato della regola del trapezio applicata al range di input
 */
double trapezio(int start, int finish);

int main(int argc, char** argv){
	int np, my_rank;
	int quotient, reminder;
	int helper[2], i;
	double pi, global_result = 0, local_result;
	double time_elapsed, t_start, t_finish, worst_time;
	/*
	 * Istruzioni per l'inizializzazione di MPI
	 */
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &np);

	/* Parte il timer */
	t_start = MPI_Wtime();
	
	/*generiamo il range di input per ogni processo*/
	quotient = N / np;
	reminder = (int) N % np;

	if(my_rank < reminder){
		helper[0] = (quotient * my_rank) + my_rank;
		helper[1] = helper[0] + quotient + 1;
	}else{
		helper[0] = (quotient * my_rank) + reminder;
		helper[1] = helper[0] + quotient;
	}
	
	/* applichiamo la regola del trapezio e salviamo i risultati per ogni thread nella variabile local_result */
	local_result = trapezio(helper[0], helper[1]);
	
	/* ogni processo invia il suo local_result al processo MASTER che somma tutto nella variabile global_result */
	MPI_Reduce(&local_result, &global_result, 1, MPI_DOUBLE, MPI_SUM, MASTER, MPI_COMM_WORLD);
	
	/*
	 * fermiamo il tempo per ogni processo e calcoliamo il tempo di secuzione
	 * poi ogni processo invia questo valore al MASTER che salva in worst_time il tempo peggiore
	 */
	t_finish = MPI_Wtime();
	time_elapsed = t_finish - t_start;
	MPI_Reduce(&time_elapsed, &worst_time, 1, MPI_DOUBLE, MPI_MAX, MASTER, MPI_COMM_WORLD);
	
	/* I MASTER effettua l'approssimazione del pigreco e stampa il risultato */
	if (my_rank == MASTER){
		pi = 4 * d * global_result;
		printf("time elapsed %lf\n", worst_time);
		printf("il valore di pi: %lf\n", pi);
	}

	MPI_Finalize();
	return 0;
}

double trapezio(int start, int finish){
	double x2, result = 0.0;

	for(; start < finish; start++){
		x2 = d2 * start * start;
		result += 1.0 / (1.0 + x2);
 	}
	return result;
}
