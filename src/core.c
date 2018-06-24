#include "core.h"
#include "gui.h"

//Node* core_load_list_from_file()
//{
//
//    FILE* fp = NULL;
//    Node* current = NULL;
//    Node* previous = NULL;
//    Node* startPtr = NULL;
//    Node tmp;
//
//    fp = fopen( "menu.dat", "rb" );
//
//    if ( fp == NULL ) {
//        fp = fopen( "menu.dat", "wb" );
//        fclose(fp);
//        return NULL;
//    }
//
//    while ( fread ( &tmp, sizeof(Node), 1, fp ) == 1 ) {
//        current = (Node*) malloc(sizeof(Node));
//        if ( current == NULL ) {
//            fclose(fp);
//            exit(-1);
//        }
//        *current = tmp;
//        if ( previous == NULL ) {
//            startPtr = current;
//        }
//        else
//            previous->next = current;
//
//        previous = current;
//    }
//
//    if (current != NULL) {
//        current->next = NULL;
//    }
//
//    fclose(fp);
//
//    return startPtr;
//}

/* Questa funzione riceve in ingresso un puntatore a puntatore a una variabile di tipo Piatto.
 * Vengono letti gli elementi presenti nel file che contiene il menu e viene allocato un array
 * in cui memorizzare temporaneamente il menu.
 * La funzione restituisce il numero di elementi letti dal file. */
int core_load_list_from_file(Piatto** piattoPtr ) {

    FILE* fp = NULL;
    long int size = 0; // dimensione del file. E' di tipo long int poiché ftell restituisce un valore di tipo long int
    int records = 0; // numero dei record scritti sul file


    fp = fopen( "menu.dat", "rb" ); // apro il file in modalità di lettura

    if ( fp == NULL ) {
        fp = fopen( "menu.dat", "wb" ); // se il file non può essere aperto viene creato
        if ( fp == NULL ) {
            // se è impossibile creare il file viene visualizzato un messaggio di errore
            // e l'esecuzione viene interrotta.
            gui_error( NULL, "Errore nella gestione dei files.");
            exit(-1);
        }
        else return 0; // se il file è stato creato è vuoto, dunque contiene 0 elementi
    }

    // sposto il puntatore di posizione del file alla fine del file
    fseek( fp, 0, SEEK_END );
    // la posizione del puntatore interno al file rappresenta il numero di byte
    // presenti nel file
    size = ftell(fp);

    if ( size == 0 ) { // file vuoto
        fclose(fp);
        return 0;
    }
    else if ( size == -1 ) { // se ftell restituisce -1 si è verificato un errore
        fclose(fp);
        gui_error( NULL, "Errore nella gestione dei files");
        exit(-1);
    }

    // alloco sullo heap una quantità di memoria pari alla dimensione del file
    *piattoPtr = (Piatto*) malloc( size );

    if ( piattoPtr == NULL ) { // se l'allocazione non andata a buon fine l'esecuzione viene interrotta
        fclose(fp);
        gui_error(NULL, "Memoria non disponibile");
        exit(-1);
    }

    // il numero di record scritti sul file è uguale al numero di byte
    // scritti sul file diviso per la dimensione di ogni record.
    records = size / sizeof(Piatto);

    // il puntatore di posizione del file viene riportato all'inizio del file
    rewind(fp);

    /* leggo tutti gli elementi presenti nel file
     * se fread restituisce un numero diverso dal numero di elementi presenti
     * nel file l'esecuzione viene interrotta
     */
    if ( fread( *piattoPtr, sizeof(Piatto), records, fp ) != records ) {
        fclose(fp);
        gui_error( NULL, "Errore nella lettura dei dati");
        exit(-1);
    }

    fclose(fp);

    // la funzione restituisce il numero di elementi letti ( la dimensione del vettore allocato ).
    return records;

}

/* La seguente funzione prende in ingresso due stringhe e verifica se sono uguali, ignorando la differenza tra
 * caratteri maiuscoli e minuscoli. Viene restituito 0 in caso di stringhe diverse e 1 nel caso in cui le stringhe siano uguali*/
int core_strcmp_case_insensitive( char* s1, char* s2 ) {
    int i = 0;  // indice del ciclo for
    int len_s1, len_s2; // lunghezza delle stringhe

    // se almeno uno dei parametri è NULL, viene restituito 0
    if ( s1 == NULL || s2 == NULL ) return 0;

    // calcola la lunghezza delle stringhe
    len_s1 = strlen(s1);
    len_s2 = strlen(s2);

    // se le due stringhe hanno lunghezza differente sono diverse
    if ( len_s1 != len_s2 ) return 0;

    for ( i = 0; i < len_s1; i++ ) {
        if ( s1[i] != s2[i] ) { // se l'i-esimo carattere di s1 è diverso dal corrispondente carattere di s2
            // considera il caso in cui una delle due stringhe abbia un carattere maiuscolo.
            if ('A' <= s1[i] && s1[i] <= 'Z' ) {
                if ( s1[i] + 32 != s2[i] ) return 0; // se s1 ha un carattere maiuscolo e il corrispondente carattere minuscolo
            }                                        // è ancora diverso dall'i-esimo carattere di s2 la funzione restituisce 0
            // è necessario considerare anche il caso in cui s2[i] sia un carattere maiuscolo
            else if ('A' <= s2[i] && s2[i] <= 'Z' ) {
                if ( s2[i] + 32 != s1[i] ) return 0;
            }
            else return 0; // se s1[i] è diverso da s2[i] e nessuno di tali caratteri è maiuscolo viene restituito 0
        }
    }

    return 1;
}

/* Questa funzione prende in ingresso una stringa corrispondente a un prezzo e restituisce 1 se la tale stringa corrisponde
 * a un numero espresso con due cifre decimali separate dalla parte intera tramite una virgola. In caso contrario viene restituito 0. */
int core_is_valid(char* prezzo) {
    int i = 0; // indice del ciclo
    int flag_point = 0; // variabile che determina se nel numero nella stringa è presente la virgola
    int len = 0; // lunghezza della stringa

    if ( prezzo == NULL ) return 0;

    len = strlen(prezzo);

    // per rappresentare un numero espresso con 2 cifre decimali occorre una stringa
    // di lunghezza non inferiore a 4 caratteri
    if (len < 4) return 0;

    for ( i = 0; i < len; i++ ) {
        if  ( !('0' <= prezzo[i] && prezzo[i] <= '9') ) { // se il carattere corrente non è un numero è ammesso solo il caso in cui
            if ( prezzo[i] == ',' ) {                     // tale carattere sia una virgola
                flag_point++;   // incrementa flag_point per indicare che è stata trovata una virgola
                if ( i != len - 3 ) return 0; // se la virgola non compare nella posizione corretta viene restituito 0.
                if ( flag_point > 1 ) return 0; // se il carattere virgola compare più di una volta viene restituito 0.
            } else
                return 0;
        }
    }

    if (flag_point == 0) // se nella stringa non compare la virgola viene restituito 0
        return 0;
    else
        return 1;
}

//int core_add_to_list( Node** startPtr, Piatto* nuovoPiatto )
//{
//    Node* current = NULL;
//    Node* previous =NULL;
//    Node* newPtr = NULL;
//    Node *hold_previous = NULL, *hold_current = NULL;
//    int flag = 0;
//    int current_id = 0, max = -1;
//
//    newPtr = (Node*) malloc( sizeof(Node) );
//
//    if ( newPtr == NULL ) {
//        exit(-1);
//    }
//
//    newPtr->piatto = *nuovoPiatto;
//    current = *startPtr;
//
//    while ( current != NULL ) {
//        if ( flag == 0 && nuovoPiatto->tipo < current->piatto.tipo ) {
//            flag = 1;
//            hold_previous = previous;
//            hold_current = current;
//        }
//        if ( core_strcmp_case_insensitive(current->piatto.nome, nuovoPiatto->nome) == 1 )
//            return PIATTO_ESISTENTE;
//
//        current_id = current->piatto.id;
//        if ( current_id > max )
//            max = current_id;
//
//        previous = current;
//        current = current->next;
//    }
//
//    newPtr->piatto.id = max + 1;
//
////    if ( core_aggiungi_alle_scorte( newPtr ) == FILE_ERROR ) return FILE_ERROR;
//
//    if ( flag == 1 ) {
//        if ( hold_previous == NULL ) {
//            newPtr->next = *startPtr;
//            *startPtr = newPtr;
//        }
//        else {
//            hold_previous->next = newPtr;
//            newPtr->next = hold_current;
//        }
//    }
//    else {
//        if ( previous == NULL ) {
//            newPtr->next = *startPtr;
//            *startPtr = newPtr;
//        }
//        else {
//            previous->next = newPtr;
//            newPtr->next = current;
//        }
//    }
//
//
//    #if DEBUG_MODE
//    current = *startPtr;
//    previous = NULL;
//
//    while ( current != NULL ) {
//        printf("%s\n", current->piatto.nome );
//        current = current->next;
//    }
//    #endif // DEBUG_MODE
//
//    return NO_ERRORS;
//}


/* La seguente funzione accetta in ingresso un puntatore a una variabile di tipo piatto e scrive su un file binario
 * i suoi ingredienti. Restituisce il valore FILE_ERROR in caso di errore. */
int core_aggiungi_alle_scorte( Piatto* piattoPtr ) {

    FILE* fp = NULL; // puntatore al file
    Scorta tmp; // variabile in cui memorizzare temporaneamente i dati letti dal file
    int i = 0; // indice del ciclo for
    int flag[N_INGR] = {0}; // variabile che stabilisce se un ingrediente è già presente nel file

    fp = fopen( "scorte.dat", "rb+" ); // apre il file per l'aggiornamento dei dati

    if ( fp == NULL ) { // se il file non esiste viene creato
        fp = fopen( "scorte.dat", "wb+" );
        if ( fp == NULL ) {
            return FILE_ERROR;
        }
    }

    while ( fread( &tmp, sizeof(Scorta), 1, fp ) == 1 ) { // legge un elemento dal file e lo memorizza nella variabile tmp
        for ( i = 0; i < N_INGR; i++ ) // confronta l'ingrediente letto dal file con gli ingredienti del piatto aggiunto al menu
            if ( core_strcmp_case_insensitive( tmp.ingrediente, piattoPtr->ingredienti[i] ) == 1 ) {
                flag[i] = 1; // se l'i-esimo ingrediente è già presente nel file flag[i] assume il valore 1
                break;
            }
    }

    for ( i = 0; i < N_INGR; i++ )
        if ( flag[i] == 0 ) { // al termine del ciclo while, vengono scritti sul file gli ingredienti non presenti
            tmp.contatore = 0;
            strcpy( tmp.ingrediente, piattoPtr->ingredienti[i] );
            if ( fwrite ( &tmp, sizeof(Scorta), 1, fp ) != 1 ) { // controlla se fwrite è andata a buon fine
                fclose(fp);
                return FILE_ERROR;
            }
        }


    fclose( fp );

    #if DEBUG_MODE
    printf("\n***** Contenuto del File scorte.dat *****\n\n");
    fp = fopen("scorte.dat", "rb");
    while ( !feof(fp) ) {
        if ( fread( &tmp, sizeof(Scorta), 1, fp ) != 0 ) {
            //printf("%03d", tmp.id );
            printf("%20s\t", tmp.ingrediente );
            printf("%03d", tmp.contatore );
            printf("\n");
        }
    }
    printf("*****************************************\n\n");
    fclose(fp);

    #endif // DEBUG_MODE

    return NO_ERRORS;

}


//int core_write_list_to_file(Node* start)
//{
//    FILE* fp = NULL;
//    Node* current = NULL;
//    Node* hold = NULL;
//
//    fp = fopen( "menu.dat", "wb" );
//    if ( fp == NULL) return FILE_ERROR;
//
//    current = start;
//    while( current != NULL ) {
//        hold = current->next;
//        current->next = NULL;
//        if ( fwrite( current, sizeof(Node), 1, fp ) != 1 ){
//            fclose ( fp );
//            return FILE_ERROR;
//        }
//        current = hold;
//    }
//    fclose (fp);
//
//    #if DEBUG_MODE
//    printf("\n****** Contenuto del File menu.dat ******\n\n");
//    fp = fopen("menu.dat", "rb");
//    Node tmp;
//    int i = 0;
//
//    while ( !feof(fp) ) {
//        if ( fread( &tmp, sizeof(Node), 1, fp ) != 0 ) {
//            printf("%03d", tmp.piatto.id );
//            printf("  %s\n", tmp.piatto.nome );
//            for ( i = 0; i < N_INGR; i++ ) {
//                printf("     %s\n", tmp.piatto.ingredienti[i] );
//            }
//            printf("     %.2f", tmp.piatto.prezzo );
//            printf("\n\n");
//        }
//    }
//    fclose(fp);
//    #endif // DEBUG_MODE
//    return NO_ERRORS;
//
//
//}

//int core_add_to_file(Piatto* nuovoPiatto)
//{
//
//    FILE* fp_menu = NULL;
//    FILE* fp_scorte = NULL;
//    Piatto piatto_tmp;
//    Scorta scorta_tmp;
//
//    int i = 0, n = 0, max = 0;
//    int flag[N_INGR] = {0};
//    int res = 0;
//
//
//    fp_scorte = fopen("scorte.dat", "rb+");
//    fp_menu = fopen("menu.dat", "rb+");
//
//    if ( fp_scorte == NULL && fp_menu == NULL ) {
//        fp_scorte = fopen("scorte.dat", "wb+");
//        fp_menu = fopen("menu.dat", "wb+");
//    }
//
//
//    if ( fp_menu == NULL || fp_scorte == NULL ) {
//        return FILE_ERROR;
//    }
//
//    piatto_tmp.id = -1;
//    while ( !feof(fp_menu) ) {
//        if ( fread( &piatto_tmp, sizeof(Piatto), 1, fp_menu ) == 1 ) {
//            if ( core_strcmp_case_insensitive( nuovoPiatto->nome, piatto_tmp.nome ) == 1 ) {
//                fclose(fp_menu);
//                return PIATTO_ESISTENTE;
//            }
//        }
//    }
//
//    nuovoPiatto->id = piatto_tmp.id + 1;
//    res = fwrite( nuovoPiatto, sizeof(Piatto), 1, fp_menu );
//    fclose(fp_menu);
//    if ( res != 1 ) return FILE_ERROR;
//
//
//    for ( i = 0; i < N_INGR; i++ ) {
//        n = 0;
//        rewind( fp_scorte );
//        while ( !feof(fp_scorte) )
//            if ( fread( &scorta_tmp, sizeof(Scorta), 1, fp_scorte ) != 0 ) {
//                n++;
//                if ( core_strcmp_case_insensitive( nuovoPiatto->ingredienti[i], scorta_tmp.ingrediente ) == 1 ) {
//                    flag[i] = 1;
//                    break;
//                }
//            }
//
//        if (n > max) max = n;
//    }
//
//    for ( i = 0; i < N_INGR; i++ )
//        if ( flag[i] == 0 ) {
////                scorta_tmp.id = max++;
//                strcpy( scorta_tmp.ingrediente, nuovoPiatto->ingredienti[i] );
//                scorta_tmp.contatore = 0;
//                if ( fwrite(&scorta_tmp, sizeof(Scorta), 1, fp_scorte ) != 1 ) {
//                    fclose( fp_scorte );
//                    return FILE_ERROR;
//                }
//        }
//
//    fclose(fp_scorte);
//
//    #if DEBUG_MODE
//    printf("\n****** Contenuto del File menu.dat ******\n\n");
//    fp_menu = fopen("menu.dat", "rb");
//    while ( !feof(fp_menu) ) {
//        if ( fread( &piatto_tmp, sizeof(Piatto), 1, fp_menu ) != 0 ) {
//            printf("%03d", piatto_tmp.id );
//            printf("  %s\n", piatto_tmp.nome );
//            for ( i = 0; i < N_INGR; i++ ) {
//                printf("     %s\n", piatto_tmp.ingredienti[i] );
//            }
//            printf("     %.2f", piatto_tmp.prezzo );
//            printf("\n\n");
//        }
//    }
//    printf("*****************************************\n\n");
//    fclose(fp_menu);
//
//    printf("\n***** Contenuto del File scorte.dat *****\n\n");
//    fp_scorte = fopen("scorte.dat", "rb");
//    while ( !feof(fp_scorte) ) {
//        if ( fread( &scorta_tmp, sizeof(Scorta), 1, fp_scorte ) != 0 ) {
////            printf("%03d", scorta_tmp.id );
//            printf("     %20s\t", scorta_tmp.ingrediente );
//            printf("%03d", scorta_tmp.contatore );
//            printf("\n");
//        }
//    }
//    printf("*****************************************\n\n");
//    fclose(fp_scorte);
//    #endif // DEBUG_MODE
//
//    return NO_ERRORS;
//}

/* La seguente funzione prende in ingresso una variabile di tipo Piatto, che rappresenta il piatto ordinato, il numero delle porzioni e il numero del tavolo.
 * Tali dati vengono memorizzati in uno scontrino all'interno di un array di scontrini */
Bill* core_add_to_bill( Piatto ordered_dish, int qnt, int table ) {

    static Bill cash_register[TABLES] = { { 0, NULL, .0 } }; // dichiarazione dell'array di scontrini
    int index = 0; // indice corrispondente al tavolo
    ListNode* newPtr = NULL; // nuovo elemento da inserire alla lista dei piatti ordinati
    ListNode* current = NULL, *previous = NULL; // puntatori al nodo corrente e al nodo precedente nella lista.

    index = table - 1; // ottiene l'indice corrispondente al tavolo
    cash_register[index].id = table; // memorizza il numero del tavolo nello scontrino

    // alloca la memoria necessaria per inserire il nuovo ordine
    newPtr = (ListNode*) malloc(sizeof(ListNode));
    if ( newPtr == NULL ) {
        gui_error( NULL, "Memoria non disponibile");
        exit(-1);
    }

    newPtr->ordered_dish = ordered_dish;
    newPtr->qnt = qnt;
    newPtr->next = NULL; // il nuovo elemento della lista non è collegato ad altri elementi

    current = cash_register[index].order; // il puntatore corrente rappresenta il primo elemento della lista

    // I piatti vengono ordinati secondo il tipo. Il ciclo scorre la lista fino a quando non viene trovato il punto
    // in cui inserire il nuovo piatto
    while ( current != NULL && current->ordered_dish.tipo <= newPtr->ordered_dish.tipo ) {
        if ( newPtr->ordered_dish.id == current->ordered_dish.id ) {
            current->qnt += qnt; // se il nuovo piatto ordinato è già presente nella lista viene aggiornato il numero di porzioni
            return cash_register;
        }
        previous = current;
        current = current->next; // vai all'elemento successivo
    }

    if ( previous == NULL ) { // Se il ciclo non è mai stato eseguito la lista è vuota, oppure il tipo del nuovo piatto è minore del tipo del primo piatto nella lista
        newPtr->next = cash_register[index].order; // il nuovo elemento viene inserito all'inizio della lista
        cash_register[index].order = newPtr;
    }
    else {
        previous->next = newPtr; // il nuovo elemento viene inserito tra previous e current
        newPtr->next = current;
    }

    #if DEBUG_MODE
    printf("***********************\n");
    current = cash_register[index].order;
    printf("Tavolo %d\n", cash_register[index].id );
    while( current != NULL ) {
        printf("%d x %s\n", current->qnt, current->ordered_dish.nome );
        current = current->next;
    }
    printf("***********************\n");
    #endif // DEBUG_MODE

    return cash_register;
}

/* Funzione per stampare lo scontrino. Riceve in ingresso un puntatore allo scontrino da stampare. Restituisce 0 in caso di errore e 1 se non si sono verificati errori */
int core_print_bill( Bill*  billPtr ) {

    FILE* fpBill = NULL; // file relativo allo scontrino
    FILE* fpTotal = NULL; // file in cui viene stampato il totale
    ListNode* current = NULL; // puntatore al nodo corrente
    ListNode* previous = NULL; // puntatore al nodo precedente
    char* type_str[] = { "Antipasti", "Primi piatti", "Secondi piatti", "Contorni", "Dessert" }; // array per stampare il tipo del piatto
    float discount = .0; // sconto
    float currentPrice = .0; // prezzo per l'ordine corrente
    float cash = .0; // incasso totale
    int res = 0;


    fpBill = fopen( "bill.txt", "w" ); // apre il file in modalità di scrittura
    if ( fpBill == NULL ) {
        return 0;
    }

    fprintf( fpBill, "Tavolo: %2d\n", billPtr->id ); // stampa sul file il numero del tavolo
    current = billPtr->order; // current punta al primo elemento della lista dei piatti ordinati
    billPtr->totalPrice = 0.0; // il prezzo totale viene posto a 0.0


    if ( current != NULL ) {
        fprintf( fpBill, "\n%s\n", type_str[ current->ordered_dish.tipo ] ); // stampa sul file il tipo del primo elemento
    }

    while ( current != NULL ) {
        if ( previous != NULL )
            if ( previous->ordered_dish.tipo != current->ordered_dish.tipo ) // se il tipo del piatto è cambiato
                fprintf( fpBill, "\n%s\n", type_str[ current->ordered_dish.tipo ] ); // stampa sul file il tipo del piatto

        currentPrice = current->qnt * current->ordered_dish.prezzo;
        fprintf( fpBill, "   - %3d x ", current->qnt ); // stampa il numero di porzioni
        res = core_fnprintf( fpBill, current->ordered_dish.nome, 50 ); // stampa i primi 50 caratteri del nome del piatto
        fprintf( fpBill, "   %7.2f\n", currentPrice ); // lascia uno spazio e stampa il prezzo sulla stessa riga del nome del piatto
        if ( !res ) { // se il nome del piatto non è stato stampato per intero, continua a stampare nella riga successiva a partire dal 51-esimo carattere
            fprintf( fpBill, "           %s\n", &(current->ordered_dish.nome[50]) ); // lascia uno spazio per allinearsi alla riga precedente
        }
        billPtr->totalPrice += currentPrice; // aggiorna il prezzo totale

        core_refresh_stock( current->ordered_dish.ingredienti, current->qnt ); // aggiorna il file delle scorte

        previous = current;
        current = current->next; // vai all'elemento successivo
    }

    fprintf( fpBill, "\n\n%-61s   %7.2f\n", "Importo parziale", billPtr->totalPrice ); // stampa l'importo parziale

    if ( billPtr->totalPrice > 40.0 ) { // se il prezzo è maggiore di 40 calcola lo sconto
        discount = billPtr->totalPrice / 10;
        fprintf( fpBill, "\n%-61s   %7.2f\n", "Valore sconto ( 10% )",  discount ); // stampa sul file lo sconto
    }

    billPtr->totalPrice -= discount; // sottrai dal prezzo totale lo sconto
    fprintf( fpBill, "\n%-61s   %7.2f", "Importo totale",  billPtr->totalPrice ); // stampa il prezzo scontato
    fclose(fpBill); // chiude il file dello scontrino

    current = billPtr->order;
    // libera la memoria
    while ( current != NULL ) {
        previous = current;
        current = current->next;
        free(previous);
    }

    billPtr->order = NULL;


    fpTotal = fopen( "total.txt", "r+" ); // apre il file in cui memorizzare il totale di tutti gli scontrini

    if ( fpTotal == NULL ) {
        fpTotal = fopen( "total.txt", "w" ); // se il file non esiste viene creato
        if ( fpTotal == NULL )
            return 0;
        fprintf( fpTotal, "%.2f", billPtr->totalPrice ); // se il file è appena stato creato viene scritto sul file il prezzo totale relativo
        fclose(fpTotal);                                 // allo scontrino appena stampato
        return 1;
    }

    fscanf( fpTotal, "%f", &cash ); // legge il valore presente nel file
    cash += billPtr->totalPrice; // aggiunge il prezzo relativo all'ultimo scontrino stampato
    rewind(fpTotal);
    fprintf( fpTotal, "%.2f", cash ); // aggiorna il contenuto del file
    fclose(fpTotal);



    return 1;
}

/* Funzione per aggiornare il contenuto del file delle scorte durante la stampa dello scontrino.
 * Riceve in ingresso l'array degli ingredienti e il numero di porzioni ordinate. Restituisce -1 in caso di errore e 1 nel caso in cui
 * non si siano verificati errori. */
int core_refresh_stock( char ingredienti[][DIM_INGR + 1], int qnt ) {

    FILE* fp = NULL;    // puntatore al file
    long int last_pos = 0; // identifica la posizione del puntatore di posizione del file
    Scorta tmp; // variabile in cui memorizzare temporaneamente gli elementi letti
    int n = 0, i = 0; // numero degli ingredienti trovati e indice del ciclo for

    fp = fopen( "scorte.dat", "rb+" ); // apre il file per l'aggiornamento

    if ( fp == NULL ) return -1;

    while ( n < N_INGR && fread( &tmp, sizeof(Scorta), 1, fp ) == 1 ) { // legge un elemento dal file
        for ( i = 0; i < N_INGR; i++ ) {
            if ( core_strcmp_case_insensitive( tmp.ingrediente, ingredienti[i] ) == 1 ) { // se l'elemento letto è uno degli ingredienti cercarti
                n++; // incrementa il numero degli ingredienti trovati
                tmp.contatore += qnt; // aggiungi al contatore relativo all'elemento letto dal file la quantità di porzioni ordinate
                fseek( fp, last_pos, SEEK_SET ); // posiziona il puntatore di posizione del file per sovrascrivere i dati
                if ( fwrite( &tmp, sizeof( Scorta ), 1, fp ) != 1 ) { // aggiorna i dati
                    fclose(fp);
                    return -1;
                }
                fseek( fp, last_pos + sizeof( Scorta ), SEEK_SET ); // riposiziona il puntatore di posizione del file
                break;
            }
        }
        last_pos = ftell(fp); // aggiorna la posizione
    }

    fclose(fp);

    #if DEBUG_MODE
    printf("\n***** Contenuto del File scorte.dat *****\n\n");
    fp = fopen("scorte.dat", "rb");
    while ( !feof(fp) ) {
        if ( fread( &tmp, sizeof(Scorta), 1, fp ) != 0 ) {
            printf("%20s\t", tmp.ingrediente );
            printf("%03d", tmp.contatore );
            printf("\n");
        }
    }
    printf("*****************************************\n\n");
    fclose(fp);
    #endif // DEBUG_MODE
    return 1;
}

/* Questa funzione permette di stampare solo i primi n caratteri della stringa str sul file fp
 * Restituisce 1 se è stata stampata tutta la stringa, 0 altrimenti.*/
int core_fnprintf( FILE* fp, char* str, int n ) {

    int i = 0;

    if ( str == NULL ) return 0;

    if ( strlen(str) <= n ) { // se la stringa contiene meno di n caratteri stampa l'intera stringa
        fprintf( fp, "%*s", -n, str );
        return 1;
    }
    else {
        for ( i = 0; i < n; i++ ) { // stampa i primi n caratteri
            fprintf( fp, "%c", str[i] );
        }
        return 0;
    }


}

