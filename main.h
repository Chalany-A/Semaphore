/**
 * @file main.h
 * @brief Hlavickovy subor pre main.c
 * @author Andrej Chalány (xchalaa00)
 * @date 25.04.2024
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h> 
#include <ctype.h> 
#include <semaphore.h>
#include <sys/mman.h> 
#include <sys/wait.h> 
#include <sys/types.h> 
#include <unistd.h> 
#include <fcntl.h> 


typedef struct{
    int n_skiers;   // počet lyžiarov L<20000
    int n_stops;    // počet zastávok 0<Z<=10
    int capacity;   // kapacita skibusu 10<=K<=100
    int skier_wait; // max čakanie lyžiara 0<=TL<=10000 milisekund
    int bus_travel; // max cesta medzi zástavkami 0<=TB<=1000 
}arg_t;

typedef struct Memory {
    int A_counter;    // počítadlo výpisov
    int skiers_left;  // počet lyžiarov čakajúcich na skibus
    int empty_seats;  // počet voľných miest v skibuse
    int skier_id;     // id lyžiara
    int stop[10];     // zastavky
}mem_t;

/**
 * @brief Funkcia na inicializáciu semaforov
 * 
 * @return 0 pri úspešnej inicializácii, 1 pri chybe 
*/
int semaphore_init();

/**
 * @brief Funkcia na vytvorenie zdielanej pamate
 * 
 * Alokuje pamäť pre štruktúru mem_t pomocou mmap
*/
void shared_memory();

/**
 * @brief Funkcia na zničenie semaforov vytvorených v semaphore_init
*/
void semaphore_destroy();

/**
 * @brief Funkcia na uvoľnenie pamäte alokovanej v shared_memory
*/
void mem_clean();

/**
 * @brief Funkcia pre priebeh procesu lyžiara
 * 
 * @param args štruktúra arg_t obsahujúca argumenty z príkazového riadku
*/
void skier_process(arg_t args);

/**
 * @brief Funkcia na vytvorenie lyžiarov
 * 
 * @param args štruktúra arg_t obsahujúca argumenty z príkazového riadku
*/
void skier_wakeup(arg_t args);

/**
 * @brief Funkcia pre priebeh procesu skibusu
 * 
 * @param args štruktúra arg_t obsahujúca argumenty z príkazového riadku
*/
void skibus_process(arg_t args);

/**
 * @brief Funkcia na vytvorenie skibusu
 * 
 * @param args štruktúra arg_t obsahujúca argumenty z príkazového riadku
*/
void skibus_wakeup(arg_t args);

/**
 * @brief Funkcia na kontrolu argumentov z príkazového riadku
 * 
 * @param argc počet argumentov
 * @param argv pole argumentov
 * @return 0 pri úspešnej kontrole, 1 pri chybe
*/
int arg_check(int argc, char *argv[]);


