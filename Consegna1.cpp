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

// Funzione per mantenere la proprietà dell'heap
void heapify(int *A, int n, int i) {
    int largest = i;
    int left = 2 * i + 1;
    int right = 2 * i + 2;
    
    ct_cmp++;
    if (left < n) {
        ct_read++;
        if (A[left] > A[largest]) {
            largest = left;
        }
    }
    
    ct_cmp++;
    if (right < n) {
        ct_read++;
        if (A[right] > A[largest]) {
            largest = right;
        }
    }
    
    if (largest != i) {
        ct_read++;
        ct_read++;
        swap(A[i], A[largest]);
        heapify(A, n, largest);
    }
}

// Heap Sort Algorithm
void heap_sort(int *A, int n) {
    // Costruzione dell'heap (riorganizza l'array)
    for (int i = n / 2 - 1; i >= 0; i--) {
        heapify(A, n, i);
    }
    
    // Estrazione elementi dall'heap uno per uno
    for (int i = n - 1; i > 0; i--) {
        ct_read++;
        ct_read++;
        swap(A[0], A[i]);  // Sposta la radice corrente alla fine
        heapify(A, i, 0);  // Chiamata heapify sull'heap ridotto
    }
}

// Struttura per i Leonardo numbers necessari per Smooth Sort
int leonardo(int k) {
    if (k < 2) return 1;
    return leonardo(k - 1) + leonardo(k - 2) + 1;
}

// Funzione per il merging dei subtrees in Smooth Sort
void sift(int *A, int start, int end) {
    int root = start;
    int child;
    
    while (root <= end) {
        child = root * 2 + 1;
        if (child > end) break;
        
        ct_cmp++;
        if (child + 1 <= end) {
            ct_read++;
            ct_read++;
            if (A[child] < A[child + 1]) {
                child++;
            }
        }
        
        ct_cmp++;
        ct_read++;
        if (A[root] < A[child]) {
            ct_read++;
            ct_read++;
            swap(A[root], A[child]);
            root = child;
        } else {
            break;
        }
    }
}

// Funzione per ripristinare la proprietà di heap in Smooth Sort
void trinkle(int *A, int p, int r, int lp, int rp) {
    while (lp > 0) {
        // Trova la posizione appropriata
        int q = p - leonardo(lp);
        int r1 = rp - 1;
        
        if (r1 >= 0 && q >= 0) {
            ct_cmp++;
            ct_read++;
            if (A[q] > A[p - 1]) {
                ct_read++;
                ct_read++;
                swap(A[q], A[p - 1]);
                p = q + 1;
                rp = r1;
                lp--;
                continue;
            }
        }
        
        if (r1 >= 0) {
            ct_cmp++;
            ct_read++;
            if (A[p - 1] < A[p + leonardo(lp - 1) - 1]) {
                // Caso speciale
                int t = lp;
                while (t > 0) {
                    int idx = p + leonardo(t - 1) - 1;
                    ct_cmp++;
                    ct_read++;
                    if (A[p - 1] < A[idx]) {
                        ct_read++;
                        ct_read++;
                        swap(A[p - 1], A[idx]);
                        break;
                    }
                    t--;
                }
                break;
            }
        }
        
        // Rotazione
        if (r1 >= 0) {
            ct_read++;
            ct_read++;
            swap(A[p - 1], A[p + leonardo(r1) - 1]);
            p = p + leonardo(r1);
            rp = r1 - 1;
        } else {
            break;
        }
    }
}

// Funzione principale Smooth Sort
void smooth_sort(int *A, int n) {
    int p = 1;  // Indice corrente
    int lp = 1; // Indice Leonardo corrente
    int rp = 0; // Indice rimanente
    
    // Fase di costruzione
    while (p < n) {
        if ((lp & 1) == 1) {  // lp è dispari
            // Caso di fusione
            if (p + leonardo(lp + 1) <= n) {
                rp = lp;
                lp++;
                continue;
            }
        }
        
        // Inserimento di un nuovo elemento
        if (lp > 1) {
            int idx = p + leonardo(lp - 2) - 1;
            ct_cmp++;
            ct_read++;
            if (idx < n && A[p - 1] < A[idx]) {
                ct_read++;
                ct_read++;
                swap(A[p - 1], A[idx]);
            }
        }
        
        p++;
        
        // Aggiornamento dei Leonardo numbers
        if (lp > 1) {
            int temp = lp - 2;
            while (temp > 0 && rp >= 0) {
                lp = temp;
                temp--;
            }
        }
        
        if (lp == 1) {
            lp = 2;
            rp = 1;
        } else {
            lp--;
            rp = 0;
        }
    }
    
    // Fase di estrazione
    while (p > 0) {
        p--;
        
        if (lp == 1) {
            // Caso semplice: rimuovi elemento
            lp = 2;
            rp = 1;
        } else if (lp == 2) {
            // Caso di fusione
            lp = 1;
            rp = 0;
        } else {
            // Caso complesso: decomposizione
            int t = lp - 2;
            if (rp >= 0) {
                int idx1 = p + leonardo(t) - 1;
                int idx2 = p + leonardo(t - 1) - 1;
                
                if (idx1 < n && idx2 < n) {
                    ct_cmp++;
                    ct_read++;
                    if (A[idx1] < A[idx2]) {
                        ct_read++;
                        ct_read++;
                        swap(A[idx1], A[idx2]);
                    }
                }
            }
            
            rp = t;
            lp = t + 1;
            
            // Ripristina la proprietà
            if (rp >= 0) {
                int idx = p + leonardo(rp) - 1;
                if (idx < n) {
                    sift(A, p, idx);
                }
            }
        }
        
        // Trinkle per mantenere l'ordine
        if (p > 0) {
            trinkle(A, p, n - 1, lp, rp);
        }
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

int main(int argc, char **argv) {
    int i, test;
    int *A;
    int *B; /// buffer per visualizzazione algoritmo

    if (parse_cmd(argc, argv))
        return 1;

    /// allocazione array
    A = new int[max_dim];

    n = max_dim;

    ifstream input_data;
    input_data.open("data.csv");

    int read_min = -1;
    int read_max = -1;
    long read_avg = 0;
    
    // Scegli quale algoritmo utilizzare
    // 0: quick_sort, 1: heap_sort, 2: smooth_sort
    int algorithm = 2; // Modifica questo valore per testare diversi algoritmi

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
        switch(algorithm) {
            case 0:
                smooth_sort(A, n);
                break;
            case 1:
                heap_sort(A, n);
                break;
        }

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
