#include "core.h"
#include "gui.h"

int main (int argc, char *argv[])
{
    #if DEBUG_MODE
    puts("*** DEBUG MODE ***\n");
    #endif // DEBUG_MODE

    Gui* guiPtr = NULL;
    GtkWidget* list = NULL; // widget per la visualizzazione del menu del ristorante
    gboolean flag_filter = FALSE;   // valore che stabilisce se viene visualizzato il menu completo.
                                    // vale TRUE, se l'utente decide di cercare i piatti del menu che contengono un determinato ingrediente

    /* La variabile guiPtr conterrà gli elementi principali
     * necesssari per la visualizzazione dell'interfaccia grafica. */
    guiPtr = (Gui*) malloc( sizeof(Gui) );
    if ( guiPtr == NULL ) {
        return -1;
    }

    gtk_init(&argc, &argv);    // Inizializzazione delle librerie GTK

    gtk_rc_parse("gtkrc"); // imposta il colore dei testi

    gui_load_list(&list); // legge il menu dal file e consente di visualizzare i dati tramite la variabile list

    /* La procedura g_object_set_data stabilisce un'associazione tra l'oggetto list e la variabile flag_filter
     * Attraverso la stringa "flag filter" e l'oggetto list sarà possibile risalire
     * all'indirizzo della variabile flag_filter.*/

    g_object_set_data(G_OBJECT(list), "flag filter", &flag_filter);

    gui_init( guiPtr, list ); // crea la finestra principale

    gtk_main();    // Loop di Rendering

    free(guiPtr); // libera la memoria allocata

    return 0;
}
