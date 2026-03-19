#include <fstream>
#include <iostream>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
using namespace std;

// compilazione: g++ consegna1-loader.c
//

// Il programma carica il file data.csv contenente 100 righe con dati da ordinare in modo crescente
// ./a.out
// In output viene mostrato il numero di accessi in read alla memoria per eseguire il sorting di ciascuna riga

// Obiettivo:
// Creare un algoritmo di sorting che minimizzi la somma del numero di accessi per ogni sorting di ciascuna riga del file

int ct_swap = 0;
int ct_cmp = 0;
int ct_read = 0;

int max_dim = 0;
int ntests = 100;
int ndiv = 1;
int details = 0;
int graph = 0;

int n = 0; /// dimensione dell'array

void print_array(int *A, int dim) {
    for (int j = 0; j < dim; j++) {
        printf("%d ", A[j]);
    }
    printf("\n");
}

void swap(int &a, int &b) {
    int tmp = a;
    a = b;
    b = tmp;
    /// aggiorno contatore globale di swap
    ct_swap++;
}

int partition(int *A, int p, int r) {

    /// copia valori delle due meta p..q e q+1..r
    ct_read++;
    int x = A[r];
    int i = p - 1;

    for (int j = p; j < r; j++) {
        ct_cmp++;
        ct_read++;
        if (A[j] <= x) {
            i++;
            ct_read++;
            ct_read++;
            swap(A[i], A[j]);
        }
    }
    ct_read++;
    ct_read++;
    swap(A[i + 1], A[r]);

    return i + 1;
}

void quick_sort(int *A, int p, int r) {
    /// gli array L e R sono utilizzati come appoggio per copiare i valori: evita le allocazioni nella fase di merge
    if (p < r) {
        int q = partition(A, p, r);
        quick_sort(A, p, q - 1);
        quick_sort(A, q + 1, r);
    }
}

int parse_cmd(int argc, char **argv) {

    /// parsing argomento
    max_dim = 1000;

    for (int i = 1; i < argc; i++) {
        if (argv[i][1] == 'd')
            ndiv = atoi(argv[i] + 3);
        if (argv[i][1] == 't')
            ntests = atoi(argv[i] + 3);
        if (argv[i][1] == 'v')
            details = 1;
        if (argv[i][1] == 'g') {
            graph = 1;
            ndiv = 1;
            ntests = 1;
        }
    }

    return 0;
}

int min_element(int *A, int n) {
    int min = A[0];
    for (int i = 1; i < n; i++) {
        ct_cmp++;
        ct_read++;
        if (A[i] < min) {
            min = A[i];
        }
    }
    return min;
}

int max_element(int *A, int n) {
    int max = A[0];
    for (int i = 1; i < n; i++) {
        ct_cmp++;
        ct_read++;
        if (A[i] > max) {
            max = A[i];
        }
    }
    return max;
}


int occorrences(int *A, int n, int value) {
    int count = 0;
    for (int i = 0; i < n; i++) {
        ct_cmp++;
        ct_read++;
        if (A[i] == value) {
            count++;
        }
    }
    return count;
}

void pigeonholesort(int *A, int n) {
    /// algoritmo di sorting da implementare
    // n = dimensione dell'array da ordinare
    int min = min_element(A, n);
    int max = max_element(A, n); 
    int range = max - min + 1;  
    int *holes = new int[range]();
    for(int i = 0; i < range; i++) {
        ct_read++;
        holes[i] = 0;
    }
    for(int i = 0; i < n; i++) {
        ct_read++;
        holes[A[i] - min]++;

        for(int i = 0; i < n; i++) {
            ct_read++;
            holes[A[i] - min]++;
        }
    }

    
}

int main(int argc, char **argv) {
    int i, test;
    int *A;
    int *B; /// buffer per visualizzazione algoritmo

    if (parse_cmd(argc, argv))
        return 1;

    /// allocazione array
    A = new int[max_dim];

    n = max_dim;

    // //creazione file input: NON USARE PIU' --> il file data.csv ufficiale è stato allegato, per permettere confronti equivalenti
    // srand((unsigned)time(NULL));
    //  FILE *f = fopen("data.csv", "w+");
    //  int size=100;
    //  for (int j = 0; j < size; j++) {
    //      for (int i = 0; i < n; i++) {
    //          int v = 0;             
    //         //  v=(int)(100000*exp(-(0.0+i-n/2)*(0.0+i-n/2)/n/n*64));
    //         //  v=(int)(5000*(1+sin(3.1415*(i/(n/2.0)))));
    //          v=(int)(10000*sin(3.1415*(((int)(i/30))*30/(n+0.0))));
    //          v+=rand()%(3+(int)(2*n*pow(((i+0.0)/n),2)));
    //          fprintf(f, "%d,", v);
    //      }
    //      fprintf(f, "\n");
    //  }
    //  fclose(f);

    ifstream input_data;
    input_data.open("data.csv");

    int read_min = -1;
    int read_max = -1;
    long read_avg = 0;

    //// lancio ntests volte per coprire diversi casi di input random
    for (test = 0; test < ntests; test++) {

        /// inizializzazione array: numeri random con range dimensione array
        for (i = 0; i < n; i++) {
            char comma;
            input_data >> A[i];
            input_data >> comma;
        }

        if (details) {
            printf("caricato array di dimensione %d\n", n);
            print_array(A, n);
        }

        ct_swap = 0;
        ct_cmp = 0;
        ct_read = 0;

        /// algoritmo di sorting
        quick_sort(A, 0, n - 1);

        if (details) {
            printf("Output:\n");
            print_array(A, n);
        }

        /// statistiche
        read_avg += ct_read;
        if (read_min < 0 || read_min > ct_read)
            read_min = ct_read;
        if (read_max < 0 || read_max < ct_read)
            read_max = ct_read;
        printf("Test %d %d\n", test, ct_read);
    }

    printf("N test: %d, Min: %d, Med: %.1f, Max: %d\n",
           ntests,
           read_min, (0.0 + read_avg) / ntests, read_max);

    delete[] A;

    return 0;
}
