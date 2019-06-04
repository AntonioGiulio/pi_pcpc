/*il nostro obiettivo questa volta per il metodo di monte carlo con la 
 * collective communication è di gestire anche il resto, perchè ci siamo riusciti
 * con il metodo del trapezio quindi dobbiamo farlo anche qui per non perdere di
 * correttezza ma ahimè di efficienza*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

#define MASTER 0

void get_input(int argc, char** argv, int my_rank, long* n_iter);
long monte_carlo(long local_iter);

int main (int argc, char** argv){
	int np, my_rank;
	long n_iter, quotient, reminder, n_proc_iter, proc_count, global_count;
	double t_start, t_finish, time_elapsed, worst_time;
	double pi;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &np);

	t_start = MPI_Wtime();

	get_input(argc, argv, my_rank, &n_iter);

	quotient = n_iter / np;
	reminder = n_iter % np;

	n_proc_iter = (my_rank < reminder) ? (quotient + 1) : quotient;
	
	proc_count = monte_carlo(n_proc_iter);

	MPI_Reduce(&proc_count, &global_count, 1, MPI_LONG, MPI_SUM, MASTER, MPI_COMM_WORLD);

	t_finish = MPI_Wtime();
	time_elapsed = t_finish - t_start;
	MPI_Reduce(&time_elapsed, &worst_time, 1, MPI_DOUBLE, MPI_MAX, MASTER, MPI_COMM_WORLD); 
	

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
