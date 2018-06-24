#ifndef CORE_H
#define CORE_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define DIM_NOME 100
#define DIM_INGR 50
#define N_INGR 3
#define TABLES 15

// codici di errore
#define NO_ERRORS 1
#define INPUT_ERROR -1
#define INGREDIENT_LEN_ERROR -2
#define PIATTO_ESISTENTE -3
#define FILE_ERROR -4

#define DEBUG_MODE 0

typedef enum {
    antipasto, primo, secondo, contorno, dessert
} TipoPiatto;

typedef struct {
    int id;
    char nome[DIM_NOME + 1];
    TipoPiatto tipo;
    char ingredienti[N_INGR][DIM_INGR + 1];
    float prezzo;
} Piatto;

typedef struct {
    char ingrediente[DIM_INGR + 1];
    int contatore;
} Scorta;
// una varibile di tipo scorta contiene una stringa corrispondente a
// un ingrediente e un contatore che indica quante volte è stato utilizzato l'ingrediente


// struttura autoreferenziale per la memorizzazione delle ordinazioni
typedef struct listNode {
    Piatto ordered_dish;
    int qnt;
    struct listNode* next;
} ListNode;

typedef struct {
    int id;
    ListNode* order;
    float totalPrice;
} Bill; // scontrino


// prototipi delle funzioni
int core_load_list_from_file( Piatto** );
int core_is_valid(char*);
//int core_add_to_file(Piatto*);
//int core_add_to_list(Node**, Piatto*);
int core_strcmp_case_insensitive( char*, char* );
//Node* core_load_list_from_file();
int core_aggiungi_alle_scorte( Piatto* );
//int core_write_list_to_file(Node*);
Bill* core_add_to_bill( Piatto, int, int );
int core_print_bill( Bill* );
int core_fnprintf( FILE*, char*, int );
int core_refresh_stock( char ingredienti[][DIM_INGR + 1], int qnt );

#endif
