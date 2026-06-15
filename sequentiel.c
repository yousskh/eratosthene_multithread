#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#define TIME_LOOPS 100

void eratosthene(int n, bool* tab);
void sequentiel(int n);

double timespec_diff(struct timespec start, struct timespec end);

int main() {
    int n;
    printf("Entrez un nombre : ");
    scanf("%d", &n);


    // instructions spécifiques mesure d'execution (trouvé sur stackoverflow)
    double total_execution_time = 0;
    struct timespec start, end;

    for (int i = 0; i < TIME_LOOPS; i++) {
        clock_gettime(CLOCK_MONOTONIC, &start);

        sequentiel(n);
        
        clock_gettime(CLOCK_MONOTONIC, &end);
        total_execution_time += timespec_diff(start, end);
    }

    double execution_time = total_execution_time / TIME_LOOPS;
    printf("\nTemps d'exécution moyen : %f secondes\n", execution_time);

    return 0;
}

void sequentiel(int n) {
    bool* tab = malloc(sizeof(bool) * (n + 1));

    for (int i = 2; i < n; i++) {
        tab[i] = true;
    }

    eratosthene(n, tab);

    //printf("Nombres premiers inférieurs à %d : ", n);
    //for (int i = 2; i < n; i++) {
    //    if (tab[i]) {
    //        printf("%d ", i);
    //    }
    //}

    free(tab);
}

void eratosthene(int n, bool* tab) {

    for (int i = 2; i * i < n; i++) {
        if (tab[i]) {
            for (int j = i * i; j < n; j += i) {
                tab[j] = false;
            }
        }
    }
}

// fonction specifique mesure temps
double timespec_diff(struct timespec start, struct timespec end) {
    return (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
}