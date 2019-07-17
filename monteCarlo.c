#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

#define MASTER 0

/*
Specifica sintattica
	get_input(int, char**, int, long*) -> void
Specifica semantica
	get_input(argc, argv, my_rank, n_iter) -> void
		argc: numero degli argomenti passati da riga di comando
		argv: vettore degli argomenti passati da riga di comando
		my_rank: numero del processo corrente
		n_iter: numero di iterazioni totali

	Questa funzion viene invocata all'inizio del programma da ogni processo ed utilizza la Collective
	communication. In particolare il processo MASTER invia in un messaggio broadcast (tramite il metodo
	MPI_Bcast) il numero di iterazioni totali da eseguire a tutti gli altri processi.
 */
void get_input(int argc, char** argv, int my_rank, long* n_iter);
/*
Specifica sintattica
	monte_carlo(long) -> long
Specifica semantica
	monte_carlo(local_iter) -> l
	local_iter: numero di iterazioni da effettuare per ogni processore
	l: numero dei punti genrati randomicamente che ricadono nell'aria del quarto di cerchio7

	Questa funzione applica l'algoritmo di Monte Carlo generando n_iter punti casuali e controlla se 
	ricadono o meno all'interno dell'aria del quarto di cerchio. 
*/
long monte_carlo(long local_iter);

int main (int argc, char** argv){
	int np, my_rank;
	long n_iter, quotient, reminder, n_proc_iter, proc_count, global_count;
	double t_start, t_finish, time_elapsed, worst_time;
	double pi;
	
	/*
	 * Istruzioni per l'inizializzazione di MPI
	 */
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &np);

	t_start = MPI_Wtime(); //facciamo partire il timer prima dell'effettiva computazione

	get_input(argc, argv, my_rank, &n_iter); //propaghiamo il valore di n_iter a tutti i processi
	
	/*
	 * ogni processo calcola il numero di iterazioni che deve effettuare
	 * gestendo e spalmando il resto della divisione (n_iter / np).
	 */
	quotient = n_iter / np;
	reminder = n_iter % np;
	n_proc_iter = (my_rank < reminder) ? (quotient + 1) : quotient; 
	
	proc_count = monte_carlo(n_proc_iter); //ogni processo ottiene il numero di punti utili all'approssimazione del PI applicando Monte Carlo
	
	/*
	 * Ogni processo invia proc_count al processo MASTER che accumula i risultati nella varibile global_count
	 */
	MPI_Reduce(&proc_count, &global_count, 1, MPI_LONG, MPI_SUM, MASTER, MPI_COMM_WORLD);
												
	/*
	 * Ogni processo ferma il timer, calcola il proprio tempo di esecuzione e invia il valore al processo MASTER
	 * che salva il valore maggiore  nella variabile worst_time
	 */
	t_finish = MPI_Wtime(); 
	time_elapsed = t_finish - t_start; 
	MPI_Reduce(&time_elapsed, &worst_time, 1, MPI_DOUBLE, MPI_MAX, MASTER, MPI_COMM_WORLD); 
	
	/*Il processo MASTER con i dati ottenuti fa l'approssimazione del pigreco*/
	if(my_rank == MASTER){
		pi = (double) global_count / n_iter * 4;
		printf("Worst process time = %lf seconds\n", worst_time);
		printf("Number of point = %ld, pi = %g\n", n_iter, pi);
	}

	MPI_Finalize();
	return 0;
}

void get_input(int argc, char** argv, int my_rank, long* n_iter){
	if(my_rank == MASTER){
		if(argc != 2){
			fprintf(stderr, "per l'esecuzione di %s bisogna fornire il numero di iterazioni\n", argv[0]);
			fflush(stderr);
			*n_iter = 0;
		}else{
			*n_iter = atoi(argv[1]);
		}
	}
	MPI_Bcast(n_iter, 1, MPI_LONG, MASTER, MPI_COMM_WORLD);

	if(*n_iter == 0){
		MPI_Finalize();
		exit(-1);
	}
}

long monte_carlo(long local_iter){
	long i, count = 0;
	double x, y;
	srand((unsigned int) time(0));
	for(i = 0; i < local_iter; i++){
		x = (double) rand() / RAND_MAX;
		y = (double) rand() / RAND_MAX;
		if((x *x + y * y) <= 1)
			count++;
	}
	return count;
}
