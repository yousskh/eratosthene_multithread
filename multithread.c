#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <math.h>
#include <time.h>

#define TIME_LOOPS 100

void* eratosthene(void* arg);
void multithread(int n, int num_threads);

void save_execution_times(double* times, int n, int num_threads, int n_value);

double timespec_diff(struct timespec start, struct timespec end);

typedef struct {
    int start;
    int end;
    int n;
    bool* tab;
    int* premiers;
    int nb_premiers;
} thread_data_t;

int main() {
    // declaration des variables
    int n, num_threads;

    double execution_times[100];

    printf("Entrez un nombre n : ");
    scanf("%d", &n);

    printf("Entrez le nombre de threads : ");
    scanf("%d", &num_threads);

    // instructions spécifiques mesure d'execution (trouvé sur stackoverflow)
    double total_execution_time = 0;
    struct timespec start, end;

    for (int i = 0; i < TIME_LOOPS; i++) {
        clock_gettime(CLOCK_MONOTONIC, &start);

        // appel de la fonction principale (répétée TIME_LOOPS fois pour la mesure de temps)
        multithread(n, num_threads);
        
        clock_gettime(CLOCK_MONOTONIC, &end);
        execution_times[i] = timespec_diff(start, end);
        total_execution_time += timespec_diff(start, end);
    }

    double execution_time = total_execution_time / TIME_LOOPS;
    printf("\nTemps d'exécution moyen : %f secondes\n", execution_time);
    
    save_execution_times(execution_times, TIME_LOOPS, num_threads, n);
    return 0;   
}

void multithread(int n, int num_threads) {
    // initialisation du tableau de bool à true
    bool* tab = malloc(sizeof(bool) * (n + 1));
    for (int i = 2; i < n; i++)
        tab[i] = true;

    int sqrt_n = (int)sqrt(n);

    // on parcourt sequentiellement jusqu'a sqrt(n) pour trouver les premiers
    int* premiers = malloc(sizeof(int) * (sqrt_n + 1));
    int nb_premiers = 0;
    for (int i = 2; i <= sqrt_n; i++) {
        if (tab[i]) {
            premiers[nb_premiers++] = i;
            for (int j = i * i; j <= sqrt_n; j += i)
                tab[j] = false;
        }
    }

    // on parcourt en parallele le reste du tableau pour barrer les multiples de chaque premier trouvé
    if (num_threads < 1) num_threads = 1;
    pthread_t* threads = malloc(sizeof(pthread_t) * num_threads);

    int restant = n - (sqrt_n + 1);
    int section = restant / num_threads;
    if (section < 1) section = 1;

    // creation des threads avec leurs sections de travail attribuées
    for (int i = 0; i < num_threads; i++) {
        thread_data_t* data = malloc(sizeof(thread_data_t));
        data->start = sqrt_n + 1 + i * section;
        data->end = (i == num_threads - 1) ? n : data->start + section;
        data->n = n;
        data->tab = tab;
        data->premiers = premiers;
        data->nb_premiers = nb_premiers;

        pthread_create(&threads[i], NULL, eratosthene, data);
    }

    // attente la fin de tous les threads et free
    for (int i = 0; i < num_threads; i++)
        pthread_join(threads[i], NULL);

    free(tab);
    free(threads);
    free(premiers);
}

void* eratosthene(void* arg) {
    // on recupere les données passées au thread
    thread_data_t* data = (thread_data_t*)arg;
    int start = data->start;
    int end = data->end;
    bool* tab = data->tab;
    int* premiers = data->premiers;
    int nb_premiers = data->nb_premiers;

    // on barre les multiples de chaque premier trouvé
    for (int k = 0; k < nb_premiers; k++) {
        int p = premiers[k];
        int debut = ((start + p - 1) / p) * p;
        if (debut < p * p) debut = p * p;
        for (int j = debut; j < end; j += p)
            tab[j] = false;
    }

    free(data);
    return NULL;
}

// fonction specifique mesure temps
double timespec_diff(struct timespec start, struct timespec end) {
    return (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
}

void save_execution_times(double* times, int n, int num_threads, int n_value) {
    char filename[64];
    snprintf(filename, sizeof(filename), "multithread_times_%d_%d", n_value, num_threads);

    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        perror("Erreur lors de l'ouverture du fichier");
        return;
    }

    for (int i = 0; i < n; i++) {
        fprintf(file, "%f", times[i]);
        if (i < n - 1) fprintf(file, ",");
    }

    fclose(file);
    printf("Temps sauvegardés dans %s\n", filename);
}