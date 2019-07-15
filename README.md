# Finding approximation of PI number with parallel computing on AWS cloud
### Programmazione Concorrente e Parallela su Cloud
### Università degli Studi di Salerno - Anno Accademico 2018/2019
### Prof. Vittorio Scarano
### Dott. Carmine Spagnuolo
#### Studente: Antonio Giulio 0522500732
#
### Problem statement
Calcolare il valore approssimato del π sfruttando il calcolo parallelo, sviluppando un programma in C utilizzando MPI. Lo scopo principale è quello di comparare due soluzione del calcolo del π fornite tramite la regola del Trapezio e tramite il metodo di Monte Carlo.  Verranno analizzati singolarmente i due metodi per poi confrontare le soluzioni. 
#
### Strumenti utilizzati

  1. 8 istanze Amazon EC2 m4.large (2 core) con ami-f4cc1de2
  2. 1 nodo MASTER
  3. 7 nodi SLAVE
#
### Soluzione proposta
Per entrambi gli algoritmi, lo scopo è quello di parallelizzare il processo di iterazioni di un ciclo. E' stato utilizzato pressoché lo stesso approccio per entrambe le soluzioni.
Ho cercato di parallelizzare il più possibile, infatti non è il processo MASTER a preoccuparsi di distribuire le porzioni di input agli altri processi, bensì ognuno ha i mezzi necessari per calcolare autonomamente la porzione di input da processare.
#
### Regola del trapezio
Analizziamo nel dettaglio la soluzione proposta per la regola del Trapezio.

Per prima cosa si inizializza MPI
```c
 int np; //numero totale di processori
 int my_rank; //rank del processore corrente
 
 MPI_Init(&argc, &argv);
 MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
 MPI_Comm_size(MPI_COMM_WORLD, &np);
```
Ogni processo deve applicare la regola del trapezio su un intervallo specifico compreso tra 0 e 1E7, quindi deve calcolare gli estremi del proprio intervallo in funzione di my_rank e reminder:
```c
 int quotient, reminder;
 int helper[2];
 
 quotient = N / np;
 reminder = (int) N % np;
 
 if(my_rank < reminder){
    helper[0] = (quotient * my_rank) + my_rank;
    helper[1] = helper[0] + quotient + 1;
 }else{
    helper[0] = (quotient * my_rank) + reminder;
    helper[1] = helper[0] + quotient;
 }
 ```
Successivamente ogni processo invoca la funzione trapezio passando come parametri gli estremi dell'intervallo precedentemente calcolati e salva il risultato nella variabile local_result:
 
 ```c
 double trapezio(int start, int finish){
    double x2, result = 0.0;
    for(; start < finish; start++){
        x2 = d2 * start * start;
        result += 1.0 / (1.0 + x2);
    }
    return result;
 }
 ```
 ```c
   local_result = trapezio(helper[0], helper[1];
 ```
Dopo aver ottenuto il local result ogni processo lo invia al MASTER tramite la funzione MPI_Reduce fornita da MPI. 
Ho scelto di utilizzare questa funzione perchè specificando un parametro (in questo caso MPI_SUM) ci consente di sommare automaticamente tutte le variabili local_result evitando ulteriore calcolo al processo MASTER.
```c
  MPI_Reduce(&local_result, &global_result, 1, MPI_Double, MPI_SUM, MASTER, MPI_COMM_WORLD);
```
Infine il processo MASTER si occupa di ultimare l'approssimazione del π e stampa i risultati.
```c
  if(my_rank == MASTER){
      pi = 4 * d * global_result;
      printf("il valore di pi: %lf\n", pi);
  }
```
Per quanto riguarda la gestione del tempo in entrabe le soluzioni ogni processo calcola quanto tempo intercorre tra l'inizio della computazione e il calcolo del local_resul. Infine tutti inviano il proprio time_elapsed al processo MASTER, utilizzando MPI_Reduce, che stamperà il tempo di esecuzione peggiore.
```c
MPI_Reduce(&time_elapsed, &worst_time, 1, MPI_DOUBLE, MPI_MAX, MASTER, MPI_COMM_WORLD);
```
#
### Metodo Monte Carlo
Analizziamo nel dettaglio la soluzione proposta per il metodo di Monte Carlo.

Per prima cosa si inizializza MPI
```c
 int np; //numero totale di processori
 int my_rank; //rank del processore corrente
 
 MPI_Init(&argc, &argv);
 MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
 MPI_Comm_size(MPI_COMM_WORLD, &np);
```
Ogni processo invoca la funzione get_input che tramite la collective communication di MPI, in particolare tramite la funzione MPI_Bcast, invia a tutti il valore n_iter passato da riga di comando che rappresenta il numero di iterazioni totali per l'approssimazione del π.
```c
  void get_input(int argc, char** argv, int my_rank, long* n_iter){
      ...
      MPI_Bcast(n_iter, 1, MPI_LONG, MASTER, MPI_COMM_WORLD);
      ...
  }
```
```c
  get_input(argc, argv, my_rank, &n_iter);
```
Successivamente ogni processo calcola il numero di iterazioni che deve effettuare gestendo il resto della divisione tra (n_iter / np).
```c
  quotient = n_iter / np;
  reminder = n_iter % np;
  n_proc_iter = (my_rank < reminder) ? (quotient + 1) : quotient;
```
Non ci resta che ottenere il numero dei punti utili all'approssimazione del π invocando la funzione monte_carlo che implementa l'algoritmo.
```c
long monte_carlo(long local_iter){
    long i, count = 0;
    double x, y;
    srand((unsigned int) time(0));
    for(i = 0; i < local_iter; i++){
        x = (double) rand() / RAND_MAX;
        y = (double) rand() / RAND_MAX;
        if((x * x + y * y) <= 1)
            count ++
    }
    return count;
}
```
```c
  proc_count = monte_carlo(n_proc_iter);
```
In seguito tramite la funzione MPI_Reduce, specificando il parametro MPI_SUM, ogni processo invia al MASTER il risultato ottenuto (proc_count) che verrà sommato nella variabile global_count.
```c
  MPI_Reduce(&proc_count, &global_count, 1, MPI_LONG, MPI_SUM, MASTER, MPI_COMM_WORLD);
```
Infine il processo MASTER completa l'approssimazione e stampa i risultati:
```c
  if(my_rank == MASTER){
    pi = (double) global_count / n_iter * 4;
    ...
    printf("Number of point = %ld, pi = %g\n", n_iter, pi);
  }
```
La gestione del tempo di esecuzione è stata effettuata come nella precedente soluzione.
