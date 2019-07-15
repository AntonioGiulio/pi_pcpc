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
Analizziamo nel dettaglio la soluzione proposta per il metodo del Trapezio.

Per prima cosa si inizializza MPI
'''c
 int np; //numero totale di processori
 int my_rank; //rank del processore corrente
 
 MPI_Init(&argc, &argv);
 MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
 MPI_Comm_size(MPI_COMM_WORLD, &np);
'''
Ogni processo deve applicare il metodo del trapezio su un intervallo specifico compreso tra 0 e 1E7, quindi deve calcolare gli estremi del proprio intervallo in funzione di my_rank e reminder:
'''c
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
'''


### Metodo Monte Carlo
