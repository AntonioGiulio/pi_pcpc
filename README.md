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

### Regola del trapezio

### Metodo Monte Carlo
