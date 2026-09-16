/**
 * @file main.c
 * @brief Implementácia projektu IOS FIT VUT
 * @author Andrej Chalány (xchalaa00)
 * @date 25.04.2024
*/

#include "main.h"

FILE *file;

sem_t *mutex;     // Semafor pre zápis do súboru
sem_t *final;     // Semafor pre konečnú zastávku
sem_t *boarded;   // Semafor pre nástup a výstup lyžiarov
sem_t *stop[10];  // Semafory pre zastávky
mem_t *memory = NULL;

int semaphore_init() {
    // Otvorenie súboru pre zápis
    file = fopen("proj2.out", "w");
    setbuf(file,NULL);
    if (file == NULL) {
        fprintf(stderr, "Chyba pri otváraní proj2.out\n");
        return 1;
    }

    // Inicializácia semaforov
    mutex = sem_open("/xchalaa00_sem_mutex", O_CREAT , 0666, 1);
    if (mutex == SEM_FAILED) {
        fprintf(stderr, "Chyba pri vytvorení semaforu\n");
        return 1;
    }

    final = sem_open("/xchalaa00_sem_final", O_CREAT , 0666, 0);
    if (final == SEM_FAILED) {
        fprintf(stderr, "Chyba pri vytvorení semaforu\n");
        return 1;
    }

    boarded = sem_open("/xchalaa00_sem_boarded", O_CREAT , 0666, 0);
    if (boarded == SEM_FAILED) {
        fprintf(stderr, "Chyba pri vytvorení semaforu\n");
        return 1;
    }

    stop[0] = sem_open("/xchalaa00_sem_stop0", O_CREAT , 0666, 0);
    stop[1] = sem_open("/xchalaa00_sem_stop1", O_CREAT , 0666, 0);
    stop[2] = sem_open("/xchalaa00_sem_stop2", O_CREAT , 0666, 0);
    stop[3] = sem_open("/xchalaa00_sem_stop3", O_CREAT , 0666, 0);
    stop[4] = sem_open("/xchalaa00_sem_stop4", O_CREAT , 0666, 0);
    stop[5] = sem_open("/xchalaa00_sem_stop5", O_CREAT , 0666, 0);
    stop[6] = sem_open("/xchalaa00_sem_stop6", O_CREAT , 0666, 0);
    stop[7] = sem_open("/xchalaa00_sem_stop7", O_CREAT , 0666, 0);
    stop[8] = sem_open("/xchalaa00_sem_stop8", O_CREAT , 0666, 0);
    stop[9] = sem_open("/xchalaa00_sem_stop9", O_CREAT , 0666, 0);
    for (int i = 0; i < 10; i++) {
        if(stop[i] == SEM_FAILED) {
            fprintf(stderr, "Chyba pri vytvorení semaforu\n");
            return 1;
        }
    }

    return 0;
}

void shared_memory() {
    // Alokácia pamäte
    memory = mmap(NULL, sizeof(mem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (memory == MAP_FAILED) {
        fprintf(stderr, "Chyba pri alokácii pamäte\n");
        exit(1);
    }
    // Inicializácia hodnôt
    memory->A_counter = 0;
    memory->skiers_left = 0;
    memory->empty_seats = 0;
    memory->skier_id = 0;
    memory->stop[0] = 0;
    memory->stop[1] = 0;
    memory->stop[2] = 0;
    memory->stop[3] = 0;
    memory->stop[4] = 0;
    memory->stop[5] = 0;
    memory->stop[6] = 0;
    memory->stop[7] = 0;
    memory->stop[8] = 0;
    memory->stop[9] = 0;
}

void semaphore_destroy() {
    sem_close(mutex);
    sem_unlink("/xchalaa00_sem_mutex");
    sem_close(final);
    sem_unlink("/xchalaa00_sem_final");
    sem_close(boarded);
    sem_unlink("/xchalaa00_sem_boarded");
    sem_close(stop[0]);
    sem_unlink("/xchalaa00_sem_stop0");
    sem_close(stop[1]);
    sem_unlink("/xchalaa00_sem_stop1");
    sem_close(stop[2]);
    sem_unlink("/xchalaa00_sem_stop2");
    sem_close(stop[3]);
    sem_unlink("/xchalaa00_sem_stop3");
    sem_close(stop[4]);
    sem_unlink("/xchalaa00_sem_stop4");
    sem_close(stop[5]);
    sem_unlink("/xchalaa00_sem_stop5");
    sem_close(stop[6]);
    sem_unlink("/xchalaa00_sem_stop6");
    sem_close(stop[7]);
    sem_unlink("/xchalaa00_sem_stop7");
    sem_close(stop[8]);
    sem_unlink("/xchalaa00_sem_stop8");
    sem_close(stop[9]);
    sem_unlink("/xchalaa00_sem_stop9");
}

void mem_clean() {
    munmap(memory, sizeof(mem_t));
}

void skier_process(arg_t args) {
    int skier_id = 0;

    // Zobudí sa a ohlási že začal
    sem_wait(mutex);
    ++memory->skier_id;
    ++memory->A_counter;
    skier_id = memory->skier_id;
    fprintf(file, "%d: L %d: started\n", memory->A_counter, skier_id);
    sem_post(mutex);

    // Vyberie si zastávku
    int bus_choice = ((rand()+getpid()) % args.n_stops);
    
    // Raňajkuje (čaká v intervale <0,args.skier_wait>)
    if(args.skier_wait != 0) {
        usleep((rand()+getpid()) % args.skier_wait);
    }
    else {
        usleep(1);
    }

    // Dorazí na danú zastávku a ohlási to
    sem_wait(mutex);
    ++memory->A_counter;
    ++memory->stop[bus_choice];
    fprintf(file, "%d: L %d: arrived to %d\n", memory->A_counter, skier_id, bus_choice+1);
    sem_post(mutex);


    // Čaká na skibus, keď dostane signál nastúpi, ohlási že nastúpil a signalizuje skibusu
    sem_wait(stop[bus_choice]);
    ++memory->A_counter;
    fprintf(file, "%d: L %d: boarding\n", memory->A_counter, skier_id);
    sem_post(boarded);

    // Čaká na konečnú zastávku, keď dostane signál vystúpi, ohlási že vystúpil a signalizuje skibusu
    sem_wait(final);
    ++memory->A_counter;
    fprintf(file, "%d: L %d: going to ski\n", memory->A_counter, skier_id);
    sem_post(boarded);    

    // Ukončí sa
    exit(0);
}

void skier_wakeup(arg_t args) {
    for (int i = 0; i < args.n_skiers; i++) {
        pid_t skier_pid = fork();
        if (skier_pid == -1) {
            fprintf(stderr, "Chyba pri vytváraní procesu lyžiara\n");
        }
        else if (skier_pid == 0) {
            skier_process(args);
        }
    }
}

void skibus_process(arg_t args) {    
    // Ohlási že začal jazdu, nastaví počet lyžiarov a kapacitu
    sem_wait(mutex);
    ++memory->A_counter;
    fprintf(file, "%d: BUS: started\n", memory->A_counter);
    memory->skiers_left = args.n_skiers;
    memory->empty_seats = args.capacity;
    sem_post(mutex);

    // Cyklus sa opakuje kým neprevezie všetkých lyžiarov
    while (memory->skiers_left > 0) {
        int travel_time = 0;

        // Cyklus prejde všetky zástavky
        for (int i = 0; i < args.n_stops; i++) {
            // Vygeneruje čas cesty a čaká
            travel_time = ((rand()+getpid()) % args.bus_travel);
            usleep(travel_time);
            
            // Ohlási že dorazil na zástavku i
            sem_wait(mutex);
            ++memory->A_counter;
            fprintf(file, "%d: BUS: arrived to %d\n", memory->A_counter, i+1);

            // Určí koľko lyžiarov nastúpi
            int n = memory->stop[i] < memory->empty_seats ? memory->stop[i] : memory->empty_seats;
            // Postupne púšťa lyžiarov do skibusu a čaká na signál nástupu
            for (int j = 0; j < n; j++) {
                sem_post(stop[i]);
                sem_wait(boarded);
            }
            // Aktualizuje počet lyžiarov čo zostali na zástavke a počet voľných miest
            memory->stop[i] -= n;
            memory->empty_seats -= n;
            // Ohlási že odchádza zo zástavky i
            ++memory->A_counter;
            fprintf(file, "%d: BUS: leaving %d\n", memory->A_counter, i+1);
            sem_post(mutex);
        }

        // Vygeneruje čas cesty na konečnú zastávku a čaká
        travel_time = ((rand()+getpid()) % args.bus_travel);
        usleep(travel_time);

        // Ohlási že dorazil na konečnú zastávku
        sem_wait(mutex);
        ++memory->A_counter;
        fprintf(file, "%d: BUS: arrived to final\n", memory->A_counter);

        // Postupne púšťa lyžiarov zo skibusu a čaká na signál výstupu
        int riders = args.capacity - memory->empty_seats;
        for(int i = 0; i < riders; i++) {
            sem_post(final);
            sem_wait(boarded);
        }
        // Aktualizuje počet neprevezených lyžiarov a počet voľných miest
        memory->skiers_left -= riders;
        memory->empty_seats = args.capacity;
        // Ohlási že odchádza z konečnej zastávky
        ++memory->A_counter;
        fprintf(file, "%d: BUS: leaving final\n", memory->A_counter);
        sem_post(mutex);
    }

    // Po prevezení všetkých lyžiarov ohlási že skončil
    sem_wait(mutex);
    ++memory->A_counter;
    fprintf(file, "%d: BUS: finish\n", memory->A_counter);
    sem_post(mutex);

    exit(0);
}

void skibus_wakeup(arg_t args) {
    pid_t skibus_pid = fork();
    if(skibus_pid == -1) {
        fprintf(stderr, "Chyba pri vytváraní procesu skibusu\n");
    }
    else if(skibus_pid == 0) {
        skibus_process(args);
    }
}

int arg_check(int argc, char *argv[]) {
    if (argc != 6) {
        fprintf(stderr, "Nesprávny počet argumentov\n");
        return 1;
    }

    int L = atoi(argv[1]);
    int Z = atoi(argv[2]);
    int K = atoi(argv[3]);
    int TL = atoi(argv[4]);
    int TB = atoi(argv[5]);

    for (int i = 1; i < argc; i++) {
        for (size_t j = 0; j < strlen(argv[i]); j++) {
            if (!isdigit(argv[i][j])) {
                fprintf(stderr, "Chyba: Vstupné argumenty musia byť čísla\n");
                return 1;
            }
        }
    }

    if(L >= 20000) {
        fprintf(stderr, "Chyba: Počet lyžiarov musí byť menší ako 20000\n");
        return 1;
    }

    if((Z <= 0) || (Z > 10)) {
        fprintf(stderr, "Chyba: Počet zastávok musí byť v intervale <1,10>\n");
        return 1;
    }

    if((K < 10) || (K > 100)) {
        fprintf(stderr, "Chyba: Kapacita skibusu musí byť v intervale <10,100>\n");
        return 1;
    }

    if((TL < 0) || (TL > 10000)) {
        fprintf(stderr, "Chyba: Maximálny čas čakania lyžiara musí byť v intervale <0,10000>\n");
        return 1;
    }

    if((TB < 0) || (TB > 1000)) {
        fprintf(stderr, "Chyba: Maximálny čas cesty medzi zastávkami musí byť v intervale <0,1000>\n");
        return 1;
    }

    return 0;
}

int main(int argc, char *argv[]) {
    arg_t args;
    // Kontrola vstupných argumentov
    if(arg_check(argc, argv) == 1) {
        exit(1);
    }

    // Zápis argumentov do pamäte
    args.n_skiers = atoi(argv[1]);
    args.n_stops = atoi(argv[2]);
    args.capacity = atoi(argv[3]);
    args.skier_wait = atoi(argv[4]);
    args.bus_travel = atoi(argv[5]);

    // Inicializácia semaforov
    if(semaphore_init() == 1) {
        exit(1);
    }

    // Inicializácia zdieľanej pamäte a spustenie procesov
    shared_memory();
    skibus_wakeup(args);
    skier_wakeup(args);

    // Čakanie na ukončenie všetkých procesov
    while(wait(NULL) > 0);

    // Uvoľnenie zdrojov, zatvorenie súboru a ukončenie programu
    mem_clean();
    semaphore_destroy();
    fclose(file);
    exit(0);
}