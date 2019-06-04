/*secondo paradigma di comunicazione per la tecnica del trapezio. Il processo
 * master effettua il partizionamento, lo trasferisce a tutti i processi e fa
 * anche lui la sua parte di computazione. */
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define MASTER 0

#define N 1E7
#define d 1E-7
#define d2 1E-14

double trapezio(int start, int finish);

int main(int argc, char** argv){
	int np, my_rank;
	int quotient, reminder, proc_iter;
	int helper[2], i;
	double pi, global_result = 0, local_result;
	double time_elapsed, t_start, t_finish;

	MPI_Status status;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &np);

	if (my_rank == MASTER){
		t_start = MPI_Wtime();

		helper[0] = 0;
		helper[1] = 0;
		quotient = N / np;
		reminder = (int) N % np;
		printf("quotient: %d\n reminder: %d\n", quotient, reminder);
		for(i = 1; i < np; i++){
			proc_iter = (i <= reminder) ? quotient+1 : quotient;
			helper[1] += proc_iter;
			MPI_Send(&helper, 2, MPI_INT, i, 0, MPI_COMM_WORLD);
			helper[0] = helper[1];
		}
		global_result = trapezio(helper[0], N);

		for(i = 1; i < np; i++){
			MPI_Recv(&local_result, 1, MPI_DOUBLE, i, MPI_ANY_SOURCE, MPI_COMM_WORLD, &status);
			global_result += local_result;
		}

		pi = 4 * d * global_result;

		t_finish = MPI_Wtime();
		time_elapsed = t_finish - t_start;
		printf("time elapsed: %lf\n", time_elapsed);

		printf("Global_result: %lf\n", global_result);
		printf("il valore di pi è: %lf\n", pi);
	}else{
		MPI_Recv(&helper, 2, MPI_INT, MASTER, MPI_ANY_SOURCE, MPI_COMM_WORLD, &status);
		local_result = trapezio(helper[0], helper[1]);

		MPI_Send(&local_result, 1, MPI_DOUBLE, MASTER, 0, MPI_COMM_WORLD);
	}

	MPI_Finalize();
	return 0;
}

double trapezio(int start, int finish){
	double x2, result = 0.0;

	for(;start < finish; start++){
		x2 = d2 * start * start;
		result += 1.0 / (1.0 + x2);
	}
	return result;
}
