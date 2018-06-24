#include "gui.h"
#include "core.h"

/* La funzione gui_init crea una finestra contenente tutti gli elementi essenziali
 * presenti in ogni schermata */
void gui_init( Gui* guiPtr, GtkWidget* treeView ) {

    GtkWidget* window = NULL;
    GtkWidget* layout = NULL;               // permette la sovrapposizione dei widget
    GtkWidget* treeView_box = NULL;         // box contenente il widget per la visualizzazione del menu
    GtkWidget* bg = NULL;                   // background
    GtkWidget* bar = NULL;                  // barra posizionata in basso in ogni finestra
    GtkWidget* menu_image = NULL, *back_image = NULL; // pulsanti presenti nella barra
    GtkWidget* menu_eventBox = NULL, *back_eventBox = NULL;
    GtkListStore* listStore = NULL;
    GtkWidget* scrolled_window = NULL;      // finestra scorrevole per la visualizzazione dell'intero menu
    GtkWidget* scrollBox = NULL;


    // creazione della finestra
    window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title (GTK_WINDOW (window), "Gestione Ristorante");
    gtk_window_set_position (GTK_WINDOW (window), GTK_WIN_POS_CENTER);
    gtk_widget_set_size_request( window, WIN_WIDTH, WIN_HEIGHT );
    gtk_window_set_resizable(GTK_WINDOW(window), FALSE);

    // creazione del layout
    layout = gtk_layout_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER (window), layout);

    // impostazione del background
    bg = gtk_image_new_from_file("images/MainWinBG.jpg");
    gtk_layout_put(GTK_LAYOUT(layout), bg, 0, 0);

    // realizzazione della barra
    bar = gtk_image_new_from_file("images/bar.png");
    gtk_layout_put(GTK_LAYOUT(layout), bar, 0, BAR_POS);

    // creazione di un pulsante per la visualizzazione delle opzioni
    menu_eventBox = gtk_event_box_new();
    menu_image = gtk_image_new_from_file("images/menu_button.png");
    gtk_container_add( GTK_CONTAINER(menu_eventBox), menu_image );
    gtk_event_box_set_visible_window( GTK_EVENT_BOX(menu_eventBox), FALSE);
    gtk_layout_put(GTK_LAYOUT(layout), menu_eventBox, BAR_RIGHT, BAR_POS);

    // creazione di un pulsante per muoversi tra le finestre
    back_eventBox = gtk_event_box_new();
    back_image = gtk_image_new_from_file("images/back.png");
    gtk_container_add( GTK_CONTAINER(back_eventBox), back_image );
    gtk_event_box_set_visible_window( GTK_EVENT_BOX(back_eventBox), FALSE);
    gtk_layout_put(GTK_LAYOUT(layout), back_eventBox, BAR_LEFT, BAR_POS);

    // box in cui inserire il menu
    treeView_box = gtk_vbox_new(FALSE, 16);

    // creazione di una finestra scorrevole
    scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    scrollBox = gtk_vbox_new(TRUE, 0);
    gtk_box_pack_start( GTK_BOX(scrollBox), treeView, TRUE, TRUE, 0);
    gtk_scrolled_window_add_with_viewport( GTK_SCROLLED_WINDOW( scrolled_window ), scrollBox );
    gtk_box_pack_start(GTK_BOX(treeView_box), GTK_WIDGET(scrolled_window), TRUE, TRUE, 0);
    g_object_set_data( G_OBJECT(treeView_box), "scrollBox", scrollBox );

    gtk_container_add( GTK_CONTAINER(layout), treeView_box );
    gtk_widget_set_size_request( treeView_box, WIN_WIDTH, BAR_POS );
    gtk_container_set_border_width( GTK_CONTAINER(treeView_box), 16 );

    // Memorizzazione dei dati nell'area di memoria puntata da guiPtr,
    // dichiarato nel main.
    guiPtr->window = window;
    guiPtr->layout = layout;
    guiPtr->box = NULL;
    guiPtr->bg = bg;
    guiPtr->bar = bar;
    guiPtr->back_button = back_eventBox;
    guiPtr->menu_button = menu_eventBox;
    guiPtr->ptr = NULL;
    guiPtr->treeView = GTK_TREE_VIEW(treeView);
    guiPtr->treeView_box = treeView_box;
    guiPtr->handler_id = g_signal_connect( guiPtr->back_button, "button_press_event", G_CALLBACK(save_list_to_file), guiPtr);

    listStore = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(treeView)));
    guiPtr->listStore = listStore;

    g_signal_connect (window, "delete-event", G_CALLBACK(save_list_to_file), guiPtr);

    // La subroutine gui_main_window crea la schermata iniziale
    gui_main_window(NULL, NULL, guiPtr);
}

/* La funzione gui_main_window crea la schermata principale. Riceve in ingresso un puntatore a una variabile
 * di tipo Gui, e i parametri necessari affinché sia possibile chiamare la funzione anche tramite g_signal_connect
 * quando viene premuto il pulsante per tornare indietro*/
void gui_main_window( GtkWidget* widget, GdkEventButton* eventButton, Gui* guiPtr ) {

    gui_box_new( HORIZONTAL, guiPtr ); // crea una hbox

    // viene chiamata la funzione save_list_to_file se viene premuto il pulsante indietro.
    // se il pulsante indietro era stato precedentemente associato ad altre callback tali associazioni vengono rimosse
    g_signal_handler_disconnect( guiPtr->back_button, guiPtr->handler_id);
    guiPtr->handler_id = g_signal_connect( guiPtr->back_button, "button_press_event", G_CALLBACK(save_list_to_file), guiPtr );

    // imposta il background
    gtk_image_set_from_file( GTK_IMAGE(guiPtr->bg), "images/MainWinBG.jpg" );

    // vengono creati 3 pulsanti collegati a tre diverse subroutine
    gui_image_button_new( "images/menu.png", guiPtr->box, G_CALLBACK (gui_print_menu), guiPtr );
    gui_image_button_new( "images/orders.png", guiPtr->box, G_CALLBACK (gui_new_bill), guiPtr );
    gui_image_button_new( "images/bill.png", guiPtr->box, G_CALLBACK (gui_print_button_clicked), guiPtr );

    gtk_widget_show_all(guiPtr->window);
    gtk_widget_hide(GTK_WIDGET(guiPtr->treeView_box));
    gtk_widget_hide(guiPtr->menu_button);
}

/* La seguente funzione distrugge la box presente nella schermata e ne crea una nuova del tipo
 * specificato come parametro */
void gui_box_new( char box_type, Gui* guiPtr ) {

    if ( guiPtr->box != NULL ) {
        gtk_widget_destroy(guiPtr->box);
    }

    if ( box_type == VERTICAL ) {
        guiPtr->box = gtk_vbox_new( FALSE, 16 ); // crea una nuova vbox
    }
    else {
        guiPtr->box = gtk_hbox_new( FALSE, 16 ); // crea una hbox
    }

    // la box viene aggiunta al layout
    gtk_container_add( GTK_CONTAINER(guiPtr->layout), guiPtr->box );
    gtk_widget_set_size_request( guiPtr->box, WIN_WIDTH, BAR_POS );
    gtk_container_set_border_width( GTK_CONTAINER(guiPtr->box), 16 );
}

/* Inserisce l'immagine indicata dalla stringa str nella box passata come secondo parametro.
 * Al click del mouse sull'immagine viene invocata la subroutine indicata come terzo parametro. */
void gui_image_button_new(  char* str, GtkWidget* box, GCallback subroutine, gpointer ptr ) {

    GtkWidget *image = NULL;
    GtkWidget *eventBox = NULL;

    image = gtk_image_new_from_file(str); // crea l'immagine
    eventBox = gtk_event_box_new();       // crea il contenitore in cui inserire l'immagine

    gtk_container_add( GTK_CONTAINER(eventBox), image ); // inserisce image in eventBox

    // Quando viene emesso il segnale "button_press_event" viene chiamata la subroutine passata come terzo parametro
    // A tale subroutine viene passato ptr
    g_signal_connect(G_OBJECT (eventBox), "button_press_event", G_CALLBACK(subroutine), ptr );
    gtk_event_box_set_visible_window( GTK_EVENT_BOX(eventBox), FALSE); // eventBox non è visibile
    // la stringa associata all'immagine viene associata all'eventbox tramite la keyword "image"
    g_object_set_data( G_OBJECT (eventBox), "image", str );
    gtk_box_pack_start( GTK_BOX(box), eventBox, TRUE, FALSE, 0 );

}

/* La funzione gui_print_menu permette di visualizzare il menu del ristorante
 * Tale funzione, può essere invocata anche dopo le subroutine per la modifica del menu. */
void gui_print_menu( GtkWidget* image_button, GdkEventButton* eventButton, Gui* guiPtr ) {

    gboolean* flag_filter = NULL;
    gboolean* boolPtr = NULL;
    GtkCellRenderer* renderer = NULL;
    GtkTreeViewColumn* col =  NULL;
    GtkWidget* filteredView = NULL;
    GList* list = NULL;

    // se la subroutine viene invocata in seguito alla scelta dell'utente di tornare indietro
    // dopo la ricerca dei piatti contenenti un determinato ingrediente, è necessario distruggere i widget creati dalla subroutine precedentemente eseguita
    flag_filter = g_object_get_data(G_OBJECT(guiPtr->treeView), "flag filter"); // recupera flag_filter dichiarato nel main
    if ( flag_filter != NULL )
    if ( *flag_filter == TRUE ) { // se la schermata precedente era relativa alla ricerca dei piatti vengono distrutti i widget creati in tale finestra
        filteredView = g_object_get_data(G_OBJECT(guiPtr->layout), "filtered_view");
        if ( filteredView != NULL ) {
            gtk_widget_destroy(filteredView);
        }
    }
    *flag_filter = FALSE;

    gtk_tree_view_set_model( guiPtr->treeView, GTK_TREE_MODEL(guiPtr->listStore));

    // cancella gli elementi presenti precedentemente nella finestra e crea una vbox
    gui_box_new(VERTICAL, guiPtr);
    gtk_image_set_from_file( GTK_IMAGE(guiPtr->bg), "images/bg.jpg" ); // imposta il bg

    /* L'istruzione seguente recupera la settima colonna del widget GtkTreeView.
     * Tale colonna contiene un intero il cui valore può essere interpretato in due modi.
     * Se l'utente si trova nella sezione dedicata alla gestione del menu l'intero presente nell'ultima
     * colonna, interpretato come booleano, rappresenta un CheckButton che indica se la colonna è selezionata.
     * Se l'utente si trova nella sezione per la gestione delle ordinazioni, l'intero nell'ultima colonna rappresenta il numero di
     * porzioni ordinate. */
    col = gtk_tree_view_get_column( guiPtr->treeView, 7 );

    if ( col != NULL )
        gtk_tree_view_remove_column(guiPtr->treeView, col);


    renderer = gtk_cell_renderer_toggle_new(); // imposta il renderer della colonna: verrà creata una checkBox
    col = gtk_tree_view_column_new_with_attributes( "", renderer, "active", 7, NULL);
    // al click del mouse sul pulsante nella settima colonna viene chiamata la funizone gui_on_toggle
    g_signal_connect(G_OBJECT(renderer), "toggled", G_CALLBACK(gui_on_toggle), guiPtr->treeView);
    gtk_tree_view_append_column(GTK_TREE_VIEW(guiPtr->treeView), col);

    // il valor di tutti gli interi dell'ultima colonna vien impstato a 0
    list = gui_menu_list_selected_items( GTK_TREE_MODEL(guiPtr->listStore) );
    g_list_free_full( list, free );

    // impostazione del bottone indietro
    g_signal_handler_disconnect( guiPtr->back_button, guiPtr->handler_id);
    guiPtr->handler_id = g_signal_connect( guiPtr->back_button, "button_press_event", G_CALLBACK(gui_main_window), guiPtr );

    // Se i processi di aggiunta o modifica di un piatto sono stati interrotti bruscamente tornano indietro alla visualizzazione
    // del menu, viene liberata la memoria allocata per tali processi
    free(guiPtr->ptr);
    guiPtr->ptr = NULL;

    boolPtr = g_object_get_data(G_OBJECT(guiPtr->layout), "flag" );
    free(boolPtr);
    g_object_set_data( G_OBJECT(guiPtr->layout), "flag", NULL );

    gtk_widget_show_all(guiPtr->layout);
    g_signal_connect(guiPtr->menu_button, "button_press_event", G_CALLBACK(gui_menu_button_clicked), guiPtr );
}

/* La subroutine gui_menu_button_clicked
 * crea una hbox contente dei pulsanti associati alle azioni disponibili per la gestione del menu
 * */
void gui_menu_button_clicked( GtkWidget* image_button, GdkEventButton* eventButton, Gui* guiPtr ) {
//{
//    static GtkWidget* treeView = NULL;
//    static GtkTreeSelection* selection = NULL;
//    char* headers[] = { "", "" };
//
//    #if DEBUG_MODE
//    printf("Creo il Menu'\n");
//    #endif // DEBUG_MODE
//    if ( treeView == NULL ) {
//        treeView = gui_list_new( headers, TRUE, FALSE );
//        gui_add_to_list_with_icon( treeView, "images/add.png", "Aggiungi un nuovo piatto" );
//        gui_add_to_list_with_icon( treeView, "images/edit.png", "Modifica un piatto esistente" );
//        gui_add_to_list_with_icon( treeView, "images/delete.png", "Elimina piatti" );
//        gui_add_to_list_with_icon( treeView, "images/search.png", "Cerca per ingrediente" );
//
//        g_object_set_data( G_OBJECT (guiPtr->layout), "menu_view", treeView );
//
//        selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeView));
//        g_signal_connect(selection, "changed", G_CALLBACK(gui_list_selection), guiPtr);
//
//    }
//
//    gtk_tree_selection_unselect_all( selection );
//    gtk_layout_put( GTK_LAYOUT(guiPtr->layout), treeView, 800, 340 );
//    gtk_widget_show(treeView);
//    g_signal_handlers_disconnect_by_func(guiPtr->menu_button, G_CALLBACK(gui_menu_button_clicked), guiPtr );
//}

    GtkWidget* vbox = NULL, *hbox = NULL;
    GtkWidget* button = NULL;
    // vbox rappresenta la box che contiene la lista dei piatti

    vbox = guiPtr->treeView_box;
    hbox = gtk_hbox_new(TRUE, 16);


    // creazione dei bottoni per la gestione del menu
    button = gtk_button_new_with_label("Aggiungi un nuovo piatto");
    g_signal_connect( button, "clicked", G_CALLBACK( gui_scegli_tipo_piatto ), guiPtr );
    gtk_box_pack_start( GTK_BOX(hbox), button, TRUE, TRUE, 0 );

    button = gtk_button_new_with_label("Modifica un piatto esistente");
    g_signal_connect( button, "clicked", G_CALLBACK( gui_switch_fields_to_mod ), guiPtr );
    gtk_box_pack_start( GTK_BOX(hbox), button, TRUE, TRUE, 0 );

    button = gtk_button_new_with_label("Elimina i piatti selezionati");
    g_signal_connect( button, "clicked", G_CALLBACK( gui_del_from_list ), guiPtr );
    gtk_box_pack_start( GTK_BOX(hbox), button, TRUE, TRUE, 0 );

    button = gtk_button_new_with_label("Cerca per ingrediente");
    g_signal_connect( button, "clicked", G_CALLBACK( gui_search_by_ingredient ), guiPtr );
    gtk_box_pack_start( GTK_BOX(hbox), button, TRUE, TRUE, 0 );

    gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0 );
    gtk_widget_show_all(vbox);

    gtk_widget_set_size_request( hbox, WIN_WIDTH, 40 );

    g_signal_handlers_disconnect_by_func(guiPtr->menu_button, G_CALLBACK(gui_menu_button_clicked), guiPtr );

    // hbox viene distrutta insieme alla box principale guiPtr->box
    g_signal_connect( G_OBJECT(guiPtr->box), "destroy", G_CALLBACK(gui_destroy), hbox );
    g_object_set_data( G_OBJECT(guiPtr->layout), "hbox", hbox );

}

// la seguente funzione crea la lista. Ogni colonna ha come intestazione una stringa dell'array headers
GtkWidget* gui_list_new( char** headers ) {

    GtkWidget* treeView = NULL;
    GtkListStore* listStore = NULL;
    GtkTreeViewColumn* col = NULL;
    int i = 0;

    // crea un nuovo TreeView per la visualizzazione dei dati
    treeView = gtk_tree_view_new();

    // crea un nuovo listStore con 8 colonne dei tipi specificati
    // il contenuto dell'ultima colonna verrà visualizzato come intero o come CheckButton ( booleano )
    listStore = gtk_list_store_new( 8, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_INT );

    gtk_tree_view_set_model( GTK_TREE_VIEW(treeView), GTK_TREE_MODEL(listStore));

    /* Dopo la creazione, il contatore per le references di listStore vale 1.
     * Tale contatore è stato incrementato quando listStore è stato assegnato come modello a
     * treeView. Quando il contatore per le references scende a 0 la memoria riservata per listStore viene liberata.
     * E' necessario dunque decrementare il contatore in maniera tale che in seguito alla distruzione di treeView
     * il suo valore scenda a 0 e venga liberata anche la memoria riservata a listStore*/
    g_object_unref(listStore);

    // creazione delle colonne per il widget TreeView
    for( i = 0; i < 7; i++ ) {
        col = gtk_tree_view_column_new_with_attributes( headers[i], gtk_cell_renderer_text_new(), "text", i, NULL);
        gtk_tree_view_append_column(GTK_TREE_VIEW(treeView), col);
    }

    return treeView;

}

//void gui_add_to_list_with_icon(GtkWidget* treeView, const char* str1, char* str2 )
//{
//
//    GtkListStore* listStore = NULL;
//    GtkTreeIter iter;
//    GtkWidget* icon = NULL;
//    GdkPixbuf* pixbuf = NULL;
//
//    listStore = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(treeView)));
//    gtk_list_store_append(listStore, &iter);
//
//    icon = gtk_image_new_from_file(str1);
//    pixbuf = gtk_image_get_pixbuf(GTK_IMAGE(icon));
//    gtk_list_store_set(listStore, &iter, 0, pixbuf, 1, str2, -1);
//
//    g_object_unref(pixbuf);
//}

/* La subroutine seguente permette di inserire i dati relativi a un piatto letto dal file
 * in un oggetto GtkListStore, utilizzabile insieme al widget TreeView. Nel GtkListStore viene memorizzata una
 * copia dei dati relativi ai piatti. */

void gui_add_to_list_dish(GtkWidget* treeView, Piatto* piattoPtr ) {

    GtkTreeIter iter; // memorizza la posizione della riga in cui scrivere i dati
    GtkListStore* listStore = NULL; // oggetto che permette la memorizzazione dei dati
    char prezzo_str[15]; // stringa in cui scrivere il prezzo del piatto
    char id_str[15];    // stringa in cui scrivere l'id del piatto
    char* tipi_piatto[] = {"Antipasto", "Primo", "Secondo", "Contorno", "Dessert"}; // array che consente la visualizzazione del tipo di piatto

    // ricava il modello del widget TreeView passato come parametro
    listStore = GTK_LIST_STORE(gtk_tree_view_get_model( GTK_TREE_VIEW(treeView)));

    // aggiunge una riga al GtkListStore
    gtk_list_store_append(listStore, &iter);

    // stampa id e prezzo su una stringa
    sprintf( id_str, "%03d", piattoPtr->id );
    sprintf( prezzo_str, "%.2f Euro", piattoPtr->prezzo );

    // aggiungi al listStore i dati relativi al piatto passato come parametro
    gtk_list_store_set(listStore, &iter, 0, id_str, 1, tipi_piatto[piattoPtr->tipo], 2, piattoPtr->nome, 3, piattoPtr->ingredienti[0],
                       4, piattoPtr->ingredienti[1], 5, piattoPtr->ingredienti[2], 6, prezzo_str, 7, FALSE, -1 );


}

//void gui_list_selection( GtkWidget* selection, Gui* guiPtr )
//{
//    GtkTreeIter iter;
//    GtkTreeModel* model;
//    GtkWidget* treeView = NULL;
//    char* action;
//
//    if ( gtk_tree_selection_get_selected(GTK_TREE_SELECTION(selection), &model, &iter) ) {
//        gtk_tree_model_get(model, &iter, 1, &action,  -1);
//    }
//
//    switch ( action[0] ) {
//        case 'A':   gui_scegli_tipo_piatto( NULL, guiPtr);
//                    gtk_widget_hide(guiPtr->treeView_box);
//                    break;
//        case 'M':   gui_mod_dish( NULL, guiPtr); break;
//        case 'E':   gui_del_from_list( NULL, guiPtr); break;
//        case 'C':   gui_search_by_ingredient( NULL, guiPtr); break;
//    }
//
//
//    g_free(action);
//    treeView = g_object_get_data( G_OBJECT(guiPtr->layout), "menu_view");
//    g_object_unref(treeView);
//    gtk_widget_destroy( treeView );
//    //gtk_container_remove(GTK_CONTAINER(guiPtr->layout), treeView );
//}

/* La seguente procedura permette di selezionare o deselezionare una riga.
 * Il parametro path identifica la posizione della riga selezionata*/
void gui_on_toggle( GtkCellRenderer* toggle, char* path, GtkTreeView* treeView ) {

    GtkTreeModel* model = NULL;
    GtkTreeIter iter;
    gboolean state = FALSE; // state indica se la riga è selezionata

    //seleziona o deseleziona la riga premuta
    model = gtk_tree_view_get_model(GTK_TREE_VIEW(treeView));
    if ( gtk_tree_model_get_iter_from_string( model , &iter , path ) ) { // la stringa path viene convertita in un iter
        gtk_tree_model_get( model , &iter , 7 , &state , -1);
        gtk_list_store_set( GTK_LIST_STORE( model ), &iter , 7 , !state , -1 ); // cambia il valore presente nell'ultima colonna
    }

}

/* La procedura gui_scegli_tipo_piatto avvia la creazione di un nuovo piatto*/
void gui_scegli_tipo_piatto(GtkWidget* widget, Gui* guiPtr) {

    GtkWidget* image = NULL; // immagine inserita nella finestra
    GtkWidget* bar = NULL; // barra che permette di scegliere il tipo del piatto
    static Piatto* nuovoPiatto = NULL; // piatto da aggiungere al menu

    if ( guiPtr->ptr == NULL ) {
        #if DEBUG_MODE
        printf("************ Alloco la memoria  *********\n\n\n");
        #endif // DEBUG
        nuovoPiatto = (Piatto*) malloc(sizeof(Piatto)); // Alloca la memoria per il nuovo piatto
        guiPtr->ptr = nuovoPiatto;                      // copia il puntatore nella variabile guiPtr in modo tale che possa essere
                                                        // facilmente passato come parametro insieme alla variabile guiPtr.
    }
    else {
        #if DEBUG_MODE
        printf("*********** Memoria già allocata *********\n\n\n");
        #endif // DEBUG_MODE

        // se la memoria è già disponibile non è necessario allocare nuovamente
        nuovoPiatto = guiPtr->ptr;
    }

    if ( nuovoPiatto == NULL ) { // errore in allocazione
        gui_error( GTK_WINDOW(guiPtr->window), "Memoria non disponibile");
        return;
    }

    nuovoPiatto->id = 0;

    // gui_box_new elimina il precedente contenuto della finestra
    gui_box_new( HORIZONTAL, guiPtr );

    // impostazioni per tornare alla schermata precedente
    g_signal_handler_disconnect( guiPtr->back_button, guiPtr->handler_id); // rimuove l'associazione identificata da handler_id per back_button
    guiPtr->handler_id = g_signal_connect( guiPtr->back_button, "button_press_event", G_CALLBACK(gui_print_menu), guiPtr );

    gtk_image_set_from_file( GTK_IMAGE(guiPtr->bg), "images/floor.jpg"); // cambia il bg

    // inserisce un'immagine nel layout.
    image = gtk_image_new_from_file("images/piatti.png");
    gtk_layout_put(GTK_LAYOUT(guiPtr->layout), image, 0, 0);
    g_object_set_data( G_OBJECT (guiPtr->layout), "image", image ); // permette di distruggere l'immagine
                                                                    // in un'altra funzione che non la riceve come parametro

    // creazione di una barra per la selezione del tipo del piatto
    // I bottoni vengono creati tramite la subroutine gui_image_button_new
    bar = gtk_hbox_new(TRUE, 0);
    gui_image_button_new( "images/antipasto.jpg", bar, G_CALLBACK (gui_memorizza_tipo_piatto), guiPtr );
    gui_image_button_new( "images/primo.jpg", bar, G_CALLBACK (gui_memorizza_tipo_piatto), guiPtr );
    gui_image_button_new( "images/secondo.jpg", bar, G_CALLBACK (gui_memorizza_tipo_piatto), guiPtr );
    gui_image_button_new( "images/contorno.jpg", bar, G_CALLBACK (gui_memorizza_tipo_piatto), guiPtr );
    gui_image_button_new( "images/dessert.jpg", bar, G_CALLBACK (gui_memorizza_tipo_piatto), guiPtr );
    gtk_layout_put(GTK_LAYOUT(guiPtr->layout), bar, 0, BAR_POS);
    g_object_set_data( G_OBJECT (guiPtr->layout), "bar", bar );

    gtk_widget_show_all(guiPtr->layout);
    gtk_widget_hide(GTK_WIDGET(guiPtr->treeView_box));
}

/* La seguente procedura permette di individuare la stringa corrispondente al tipo di piatto scelto dall'utente*/
void gui_memorizza_tipo_piatto(GtkWidget* image_button, GdkEventButton* eventButton, Gui* guiPtr) {

    Piatto* nuovoPiatto = NULL;
    char* str = NULL;

    nuovoPiatto = guiPtr->ptr;

    /* La procedura gui_image_button_new utilizza la funzione g_object_set_data
     * per associare la stringa corrispondente all'immagine alla eventBox in cui l'immagine viene inserita.
     * Dunque, è possibile recuperare la stringa relativa all'immagine corrispondente al tipo di piatto
     * scelto dall'utente. */
    str = g_object_get_data( G_OBJECT(image_button), "image");

    #if DEBUG_MODE
        printf("%s\t", str);
    #endif // DEBUG_MODE

    // assegno a nuovoPiatto->tipo il valore corretto
    if ( strcmp( str, "images/antipasto.jpg") == 0 ) {
        nuovoPiatto->tipo = antipasto;
    } else if ( strcmp( str, "images/primo.jpg") == 0 ) {
            nuovoPiatto->tipo = primo;
        } else if ( strcmp( str, "images/secondo.jpg") == 0 ) {
            nuovoPiatto->tipo = secondo;
            } else if ( strcmp( str, "images/contorno.jpg") == 0 ) {
                nuovoPiatto->tipo = contorno;
                }  else if ( strcmp( str, "images/dessert.jpg") == 0 ) {
                        nuovoPiatto->tipo = dessert;
                    }



    #if DEBUG_MODE
    printf("%d\n", nuovoPiatto->tipo);
    #endif // DEBUG_MODE

    // La funzione gui_nuovo_piatto_info permette all'utente di inserire le informazioni relative al nuovo
    // piatto. La stessa funzione viene chiamata per modificare un piatto. Il secondo parametro indica se si
    // intende modificare un piatto già esistente. FALSE, in questo caso indica che non si deve modificare un piatto
    // ma che si desidera creare un nuovo piatto.
    gui_nuovo_piatto_info(guiPtr, FALSE );
}

/* Questa funzione visualizza una schermata per l'inserimento dei dati relativi al nuovo piatto*/
void gui_nuovo_piatto_info( Gui* guiPtr, gboolean flag_modifica ) {

    GtkWidget* bar = NULL;
    GtkWidget* image = NULL;
    GtkWidget* apply = NULL;
    //Piatto* piattoPtr = NULL;
    gboolean* flagPtr = NULL;

    if ( !flag_modifica ) {
        // recupera e distrugge i widget creati nella finestra precedente
        bar = g_object_get_data( G_OBJECT(guiPtr->layout), "bar");
        image = g_object_get_data( G_OBJECT(guiPtr->layout), "image");
        gtk_widget_destroy(bar);
        gtk_widget_destroy(image);
    }
    gtk_image_set_from_file( GTK_IMAGE(guiPtr->bg), "images/bg.jpg");

    gui_box_new(VERTICAL, guiPtr); // svuota la finestra e predispone una nuova box verticale

    // creazione di 5 label e 5 entryBox
    gui_entry_new( guiPtr, "Nome del Piatto" );
    gui_entry_new( guiPtr, "Primo Ingrediente" );
    gui_entry_new( guiPtr, "Secondo Ingrediente" );
    gui_entry_new( guiPtr, "Terzo Ingrediente");
    gui_entry_new( guiPtr, "Prezzo" );

    // creazione di un pulsante "applica"
    apply = gui_apply_button(guiPtr);
    g_signal_connect( G_OBJECT(apply), "button_press_event", G_CALLBACK(gui_dialog_creazione_piatto), guiPtr );

    // impostazioni per tornare alla schermata precedente
    g_signal_handler_disconnect( guiPtr->back_button, guiPtr->handler_id);
    guiPtr->handler_id = g_signal_connect( guiPtr->back_button, "button_press_event", G_CALLBACK(gui_print_menu), guiPtr );

    // alloco sullo heap una copia di flag_modifica, in maniera tale che il suo valore possa
    // essere recuperato dalla funzione guj_dialog_creazione_piatto
    flagPtr = (gboolean*) malloc(sizeof(gboolean));
    if ( flagPtr == NULL ) {
        gui_error( GTK_WINDOW(guiPtr->window), "Memoria non disponibile.");
        gui_print_menu(NULL, NULL, guiPtr);
        return;
    }
    *flagPtr = flag_modifica;
    g_object_set_data( G_OBJECT(guiPtr->layout), "flag", flagPtr );

    gtk_widget_show_all(guiPtr->box);
}

/* Questa procedura crea una label e un entryBox */
void gui_entry_new( Gui* guiPtr, char* label_str ) {

    GtkWidget* entry = NULL;
    GtkWidget* label = NULL;
    GtkWidget* hbox = NULL;

    // creazione dell'entrybox
    entry = gtk_entry_new();
    gtk_entry_set_max_length( GTK_ENTRY (entry), DIM_NOME ); // lunghezza massima del testo inserito

    // creazione della label
    label = gtk_label_new(label_str);
    gtk_widget_set_size_request(label, 200, 40);

    // entry e label vengono inseriti dentro hbox, che a sua volta viene inserita in guiPtr->box
    hbox = gtk_hbox_new(FALSE, 16);
    gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(hbox), entry, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(guiPtr->box), hbox, TRUE, TRUE, 0);

    //permette di recuperare l'entryBox associato alla label
    g_object_set_data( G_OBJECT (guiPtr->box), label_str, entry );

}

/* Questa procedura crea un widget per la selezione del tipo del piatto.
 * Riceve come parametri guiPtr e il tipo da impostare come attivo nella comboBox */
void gui_combo_new( Gui* guiPtr, TipoPiatto active ) {
    GtkWidget* combo = NULL;
    GtkWidget* label = NULL;
    GtkWidget *hbox = NULL;

    // creazione della comboBox
    combo = gtk_combo_box_new_text();
    gtk_combo_box_append_text(GTK_COMBO_BOX(combo), "Antipasto");
    gtk_combo_box_append_text(GTK_COMBO_BOX(combo), "Primo");
    gtk_combo_box_append_text(GTK_COMBO_BOX(combo), "Secondo");
    gtk_combo_box_append_text(GTK_COMBO_BOX(combo), "Contorno");
    gtk_combo_box_append_text(GTK_COMBO_BOX(combo), "Dessert");

    // imposta il tipo attivo
    gtk_combo_box_set_active(GTK_COMBO_BOX(combo), active );

    label = gtk_label_new("Tipo del Piatto");
    gtk_widget_set_size_request(label, 200, 40);

    // inserisce i widget in una box
    hbox = gtk_hbox_new(FALSE, 16);
    gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(hbox), combo, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(guiPtr->box), hbox, FALSE, FALSE, 0);

    g_object_set_data(G_OBJECT(guiPtr->box), "combo", combo );

}

 /* Questa funzione crea un pulsante posizionato in basso a destra nella barra
  * la funzione restituisce il widget creato */
GtkWidget* gui_apply_button( Gui* guiPtr ) {

    GtkWidget *apply_eventBox = NULL, *apply_image = NULL;

    gtk_widget_hide(guiPtr->menu_button); // nasconde il pulsante che potrebbe trovarsi nella stessa posizione del nuovo pulsante


    // crea il nuovo pulsante e lo posiziona nel punto corretto.
    apply_eventBox = gtk_event_box_new();
    apply_image = gtk_image_new_from_file("images/apply.png");
    gtk_container_add( GTK_CONTAINER(apply_eventBox), apply_image );
    gtk_event_box_set_visible_window( GTK_EVENT_BOX(apply_eventBox), FALSE);
    gtk_layout_put(GTK_LAYOUT(guiPtr->layout), apply_eventBox, BAR_RIGHT, BAR_POS);
    // il pulsante viene distrutto insieme alla box principale
    g_signal_connect( G_OBJECT(guiPtr->box), "destroy", G_CALLBACK(gui_destroy), apply_eventBox );
    gtk_widget_show_all( apply_eventBox );
    g_object_set_data( G_OBJECT(guiPtr->box), "apply", apply_eventBox);

    return apply_eventBox;
}

/* Questa funzione permette di memorizzare i dati dei piatti aggiunti al menu.
 * viene adoperata anche nel caso di modifica di un piatto esistente. In caso di errore restituisce un codice di errore in base
 * al quale viene creata una specifica finestra di dialogo */
int gui_memorizza_info_piatto( Gui* guiPtr, gboolean flag_modifica ) {

    GtkEntry* entry = NULL; // punta agli entrybox creati nella schermata precedente
    GtkComboBox* combo = NULL; // punta al widget per la selezione del tipo ( solo se flag_modifica vale TRUE )
    Piatto* nuovoPiatto = NULL;
    int i = 0, j = 0, len = 0;

    nuovoPiatto = guiPtr->ptr;

    // La funzione per la modifica del piatto adotta la stessa interfaccia utilizzata per la creazione di un nuovo piatto,
    // ma aggiunge un widget per cambiare il tipo. Se è stato scelto precedentemente scelto di modificare un piatto
    // bisogna recuperare il valore scelto per il tipo del piatto.
    if ( flag_modifica ) {
        combo = g_object_get_data(G_OBJECT(guiPtr->box), "combo");
        nuovoPiatto->tipo = gtk_combo_box_get_active( combo );
    }


    // dopo l'istruzione seguente entry punta all'entryBox relativo al nome del piatto
    entry = g_object_get_data(G_OBJECT(guiPtr->box), "Nome del Piatto");
    if ( strcmp( entry->text, "") == 0 ) {
        return INPUT_ERROR; // se il campo è vuoto si restituisce un codice di errore
    }
    strcpy( nuovoPiatto->nome, entry->text ); // altrimenti il testo inserito dall'utente viene copiato nella variabile nuovoPiatto

    // recupera gli entryBox relativi agli ingredienti
    for ( i = 0; i < 3; i++ ) {
        switch (i) {
            case 0: entry = g_object_get_data(G_OBJECT(guiPtr->box), "Primo Ingrediente"); break;
            case 1: entry = g_object_get_data(G_OBJECT(guiPtr->box), "Secondo Ingrediente"); break;
            case 2: entry = g_object_get_data(G_OBJECT(guiPtr->box), "Terzo Ingrediente"); break;
        }

        len =  gtk_entry_get_text_length(entry); // calcola la lunghezza del testo inserito
        if ( len == 0 ) return INPUT_ERROR; // campo vuoto: viene restituito un codice di errore
        else if ( len > DIM_INGR ) return INGREDIENT_LEN_ERROR; // testo inserito troppo lungo: errore

        strcpy( nuovoPiatto->ingredienti[i], entry->text );

        // verifico se alcuni ingredienti sono uguali
        // per il primo ingrediente i vale 0 quindi il ciclo non viene eseguito
        for ( j = 0; j < i; j++ ) {
            if ( core_strcmp_case_insensitive( nuovoPiatto->ingredienti[i], nuovoPiatto->ingredienti[j] ) == 1 ) {
                return INPUT_ERROR;
            }
        }
    }

    // ricavo il prezzo
    entry = g_object_get_data(G_OBJECT(guiPtr->box), "Prezzo");
    if ( core_is_valid(entry->text) ) { // verifico che il prezzo sia valido
        sscanf(entry->text, "%f", &(nuovoPiatto->prezzo)); // memorizzo il prezzo come float
    }
    else {
        #if DEBUG_MODE
            puts("\nPrezzo non valido");
        #endif // DEBUG_MODE
        return INPUT_ERROR;
    }

    #if DEBUG_MODE
        printf("\n");
        printf("id:\t%d\n", nuovoPiatto->id);
        printf("Nome:\t%s\n", nuovoPiatto->nome);
        for ( i = 0; i < 3; i++ )
            printf( "Ingrediente %d:\t%s\n", i+1, nuovoPiatto->ingredienti[i]);
        printf("Prezzo:\t%.2f Euro\n", nuovoPiatto->prezzo);
        printf("\n\n");
    #endif // DEBUG_MODE
    if ( !flag_modifica )
        return gui_add_new_dish( guiPtr->treeView, nuovoPiatto ); // permette di aggiungere al menu il nuovo piatto
    else
        return gui_mod_dish( guiPtr, nuovoPiatto ); // permette di modificare un piatto esistente
}

/* La seguente procedura invoca la funzione per la memorizzazione dei dati relativi al nuovo piatto
 * e se si verifica un errore visualizza una finestra di dialogo corrispondente all'errore.*/
void gui_dialog_creazione_piatto(GtkWidget* widget, GdkEventButton* eventButton, Gui* guiPtr) {
    int creazione = 0; // variabile che stabilisce se la creazione/modifica del piatto è andata a buon fine
    gboolean flag_modifica = FALSE, *boolPtr = NULL; // variabili che stabiliscono se l'utente intende modificare o aggiungere un piatto
    GtkWidget* dialog = NULL; // finestra di dialogo
    gint response;  // permette di capire quale finestra di dialogo è stata visualizzata e di interpretare le decisioni dell'utente

    /* Recupero il valore di flag_modifica e lo copio in una variabile memorizzata sullo stack */
    boolPtr = g_object_get_data(G_OBJECT(guiPtr->layout), "flag" );

    if ( boolPtr != NULL ) {
        flag_modifica = *boolPtr;
    }

    // memorizza i dati relativi al nuovo piatto. se si verificano errori creazione assume un valore corrispondente all'errore.
    creazione = gui_memorizza_info_piatto(guiPtr, flag_modifica);


    switch ( creazione ) {

        case INPUT_ERROR: // errore nell'input
            dialog = gtk_message_dialog_new(GTK_WINDOW(guiPtr->window) , GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_ERROR  , GTK_BUTTONS_OK, "Errore");
            gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(dialog) , "Per creare un nuovo piatto e' necessario:\n"
                                                                                  "1. Riempire tutti i campi\n"
                                                                                  "2. Assegnare nomi diversi a tutti gli ingredienti\n"
                                                                                  "3. Esprimere il prezzo in euro con due cifre decimali.\n"
                                                                                  "   Tali cifre devono essere separate dalla parte intera\n"
                                                                                  "   tramite una virgola.");

            break;

        case INGREDIENT_LEN_ERROR: // lunghezza degli ingredienti superiore ai 50 caratteri
            dialog = gtk_message_dialog_new(GTK_WINDOW(guiPtr->window) , GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_ERROR  , GTK_BUTTONS_OK, "Errore");
            gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(dialog) , "La massima lunghezza consentita\nper gli ingredienti e' di:\n%d caratteri.", DIM_INGR);
            break;

        case FILE_ERROR: // errore nella gestione dei file
            dialog = gtk_message_dialog_new(GTK_WINDOW(guiPtr->window) , GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_ERROR  , GTK_BUTTONS_OK, "Errore");
            gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(dialog) , "Errore nella gestione dei files.");
            break;

        case PIATTO_ESISTENTE: // piatto già esistente
            dialog = gtk_message_dialog_new(GTK_WINDOW(guiPtr->window) , GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_WARNING, GTK_BUTTONS_OK, "Attenzione!");
            gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(dialog) , "Esiste gia' un piatto che ha lo stesso\nnome del piatto che si vuole creare.");
            break;

        case NO_ERRORS: // nessun errore rilevato
            if ( !flag_modifica ) { // se è stato aggiunto un nuovo piatto, viene data all'utente la possibilità di aggiungerne uno nuovo
                dialog = gtk_message_dialog_new(GTK_WINDOW(guiPtr->window) , GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_INFO , GTK_BUTTONS_NONE, "Creazione del piatto completata!");
                gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(dialog) , "Il Nuovo Piatto e' stato aggiunto al menu' con successo!\nDesideri aggiungere un altro Piatto?");
                gtk_dialog_add_buttons( GTK_DIALOG(dialog), "Si'", GTK_RESPONSE_ACCEPT, "No", GTK_RESPONSE_CANCEL, NULL );
                break;
            }
            else { // modifica del piatto completata.
                dialog = gtk_message_dialog_new(GTK_WINDOW(guiPtr->window) , GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_INFO , GTK_BUTTONS_NONE, "Modifica del piatto completata!");
                gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(dialog) , "Il piatto selezionto e' stato modificato con successo!");
                gtk_dialog_add_buttons( GTK_DIALOG(dialog), "OK", GTK_RESPONSE_CANCEL, NULL );
                break;
            }
    }

    response = gtk_dialog_run( GTK_DIALOG(dialog)); // response identifica il bottone premuto dall'utente e indirettamente dà informazioni sulla finestra visualizzata
    gtk_widget_destroy(dialog);

    switch (response) {
        case GTK_RESPONSE_OK:       break; // l'utente ha visualizzato un messaggio di errore: viene concessa la possibilità di correggere i dati inseriti
        case GTK_RESPONSE_ACCEPT:   gtk_widget_destroy(widget); // distrugge il bottone "applica"
                                    // l'utente desidera creare un nuovo piatto: la memoria riservata per il piatto appena creato non viene liberata.
                                    // se la creazione del nuovo piatto non dovesse avvenire, premendo il pulsante indietro l'utente torna alla visualizzazione del
                                    // menu e la memoria viene liberata dalla funzione gui_print_menu
                                    gui_scegli_tipo_piatto(NULL, guiPtr);
                                    free(boolPtr);
                                    g_object_set_data( G_OBJECT(guiPtr->layout), "flag", NULL );
                                    break;

        case GTK_RESPONSE_CANCEL:   free(guiPtr->ptr); // operazioni terminate: liberazione della memoria
                                    guiPtr->ptr = NULL;
                                    gtk_widget_destroy(widget);
                                    gtk_widget_show(guiPtr->menu_button);
                                    free(boolPtr);
                                    g_object_set_data( G_OBJECT(guiPtr->layout), "flag", NULL );
                                    gui_print_menu(NULL, NULL, guiPtr); // torna alla visualizzazione del menu
                                    break;
    }
}

/* Questa subroutine permette all'utente di visualizzare un'interfaccia per la modifica di un piatto.
 * Viene invocata la subroutine gui_nuovo_piatto_info, e l'interfaccia creata da questa subroutine viene leggermente modificata */
void gui_switch_fields_to_mod( GtkWidget* widget, Gui* guiPtr ) {

    GtkTreeView* treeView = NULL;
    GtkTreeModel* model = NULL;
    GList* list = NULL;     // lista dei piatti selezionati
    GtkWidget* dialog = NULL;
    int len; // numero degli elementi nella lista
    Piatto* piattoPtr = NULL;
    char prezzo_str[15];    // stringa per memorizzare il prezzo
    GtkWidget* entry = NULL;

    treeView = guiPtr->treeView;
    model = gtk_tree_view_get_model(treeView);

    list = gui_menu_list_selected_items(model); // stabilisce quali elementi del menu sono selezionati crea una lita di tali elementi

    len = g_list_length( list ); // memorizza in len il numero di elementi presenti nella lista

    if ( len > 1 ) {
        // Più di un elemento selezionato: impossibile modificare più di un piatto contemporaneamente
        dialog = gtk_message_dialog_new( NULL, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_WARNING, GTK_BUTTONS_OK, "Attenzione!");
        gtk_window_set_position (GTK_WINDOW(dialog), GTK_WIN_POS_CENTER);
        gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(dialog) , "Puoi modificare solo un piatto alla volta:\nSeleziona solo un piatto!\n");
        gtk_dialog_run( GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        g_list_free_full(list, free); // chiama la funzione free su ogni elemento della lista e libera la memoria occupata dalla lista
        g_signal_connect(guiPtr->menu_button, "button_press_event", G_CALLBACK(gui_menu_button_clicked), guiPtr );
        gui_box_new( VERTICAL, guiPtr );
        return;
    }
    else if ( len == 0 ) {
        // errore: nessun elemento selezionato
        dialog = gtk_message_dialog_new( NULL, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_WARNING, GTK_BUTTONS_OK, "Attenzione!");
        gtk_window_set_position (GTK_WINDOW(dialog), GTK_WIN_POS_CENTER);
        gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(dialog) , "Seleziona il piatto che vuoi modificare!");
        gtk_dialog_run( GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        g_list_free_full(list, free);
        g_signal_connect(guiPtr->menu_button, "button_press_event", G_CALLBACK(gui_menu_button_clicked), guiPtr );
        gui_box_new( VERTICAL, guiPtr );
        return;
    }

    // se la lista contiene un solo elemento vengono memorizzati i dati relativi a tale elemento
    // in una variabile di tipo Piatto
    piattoPtr = gui_get_dish_from_list( model, list->data );

    guiPtr->ptr = piattoPtr;

    gui_nuovo_piatto_info( guiPtr, TRUE ); // la funzione gui_nuovo_piatto_info predispone l'interfaccia

    // inserisce negli entryBox i dati relativi al piatto per favorire la modifica
    entry = g_object_get_data(G_OBJECT(guiPtr->box), "Nome del Piatto");
    gtk_entry_set_text( GTK_ENTRY(entry), piattoPtr->nome );

    entry = g_object_get_data(G_OBJECT(guiPtr->box), "Primo Ingrediente");
    gtk_entry_set_text( GTK_ENTRY(entry), piattoPtr->ingredienti[0] );

    entry = g_object_get_data(G_OBJECT(guiPtr->box), "Secondo Ingrediente");
    gtk_entry_set_text( GTK_ENTRY(entry), piattoPtr->ingredienti[1] );

    entry = g_object_get_data(G_OBJECT(guiPtr->box), "Terzo Ingrediente");
    gtk_entry_set_text( GTK_ENTRY(entry), piattoPtr->ingredienti[2] );

    sprintf( prezzo_str, "%.2f", piattoPtr->prezzo );
    entry = g_object_get_data(G_OBJECT(guiPtr->box), "Prezzo");
    gtk_entry_set_text( GTK_ENTRY(entry), prezzo_str );

    // crea un widget per la selezione del tipo del piatto
    gui_combo_new( guiPtr, piattoPtr->tipo );

    gtk_widget_show_all(guiPtr->box);
    gtk_widget_hide(guiPtr->treeView_box);

    g_object_set_data( G_OBJECT(guiPtr->box), "list", list ); // permette alle altre subroutine di accedere contenuto della lista e di liberare la memoria

    return;
}

/* La seguente funzione modifica i dati di un piatto visualizzato nel widget TreeView
 * Restituisce un intero corrispondente al codice di errore */
int gui_mod_dish( Gui* guiPtr, Piatto* piattoPtr ) {

    GList* list = NULL; // lista dei patti selezionati. Di fatto, in questo caso, contiene un solo elemento.
    Piatto* old_dish = NULL;    // rappresenta il piatto prima della modifica
    Piatto* current_dish = NULL; // piatto corrente
    gboolean cond = FALSE;  // condizione del ciclo while
    GtkTreeIter current_iter, old_iter; // iter che identifica il piatto corrente e iter che identifica la posizione del piatto da modificare
    GtkTreeIter* iterPtr = NULL; // puntatore all'iter che identifica la posizione del piatto da modificare
    GtkTreeModel* model = NULL; // modello del widget per la visualizzazione dei piatti

    list = g_object_get_data( G_OBJECT(guiPtr->box), "list" ); // recupera la lista dei piatti selezionati

    iterPtr = list->data;   // poiché non è concesso modificare più piatti contemporaneamente
                            // l'unico elemento contenuto nella lista è un puntatore alla posizione del piatto da modificare
    old_iter = *iterPtr;

    model = GTK_TREE_MODEL(guiPtr->listStore);

    old_dish = gui_get_dish_from_list( model, iterPtr ); // recupera i dati relativi al piatto da modificare
    g_list_free_full( list, free ); // libera la memoria allocata per memorizzare la lista e i suoi elementi

    if ( core_strcmp_case_insensitive( old_dish->nome, piattoPtr->nome ) != 1 ) {   // se il nome del piatto è cambiato occorre controllare
                                                                                    // che il nuovo nome non sia utilizzato per altri piatti
        free(old_dish); // libera la memoria allocata per old_dish
        old_dish = NULL;

        cond = gtk_tree_model_get_iter_first( model, &current_iter );   // current_iter si riferisce al primo elemento della lista.
                                                                        // cond vale TRUE se il menu non è vuoto e non si verificano errori

        while ( cond == TRUE ) {
            current_dish = gui_get_dish_from_list( model, &current_iter ); // recupera il piatto corrispondente a current_iter
            if ( core_strcmp_case_insensitive( current_dish->nome, piattoPtr->nome ) == 1  ) {
                return PIATTO_ESISTENTE; // se il nome del inserito esiste già viene restituito un messaggio di errore
            }
            free( current_dish ); // libera la memoria per il piatto corrente
            cond = gtk_tree_model_iter_next( model, &current_iter ); // vai alla posizione successiva. Se current_iter si riferisce già
                                                                     // all'ultimo elemento, cond = FALSE e si esce dal ciclo.
        }
    }

    // se i dati inseriti per la modifica del piatto sono corretti
    // il piatto precedente viene rimosso e viene avviato il processo di aggiunta di un nuovo piatto.
    // il piatto verrà inserito nella lista nella posizione corrispondente alla sua tipologia e gli ingredienti verranno inseriti
    // tra le scorte
    gtk_list_store_remove( guiPtr->listStore, &old_iter );
    free(old_dish);
    return gui_add_new_dish( guiPtr->treeView, piattoPtr );
}

/* La seguente procedura consente l'eliminazione di uno o più piatti dal modello del widget TreeView */
void gui_del_from_list( GtkWidget* widget, Gui* guiPtr ) {

    GtkTreeView* treeView = NULL;
    GtkTreeModel* model = NULL;
    GList* list = NULL;
    GtkTreeIter* iterPtr = NULL;

    gui_box_new(VERTICAL, guiPtr); // reimposta il contenuto della finestra

    treeView = guiPtr->treeView;
    model = gtk_tree_view_get_model( GTK_TREE_VIEW(treeView));

    list = gui_menu_list_selected_items( model ); // memorizza in list una lista degli iter associati agli elementi selezionati

    /* Il ciclo while scorre gli elementi della lista creata precedentemente e rimuove dal GtkListStore
     * le righe corrispondenti a tali elementi */
    while ( list != NULL ) {
        iterPtr = list->data;
        gtk_list_store_remove( GTK_LIST_STORE(model), iterPtr );
        list = list->next; // vai all'elemento successivo
    }

    g_list_free_full( list, free ); // chiama la funzione free su ogni elemento della lista e libera tutta la memoria usata dalla lista

    g_signal_connect(guiPtr->menu_button, "button_press_event", G_CALLBACK(gui_menu_button_clicked), guiPtr );
}

/* La seguente procedura crea l'interfaccia che permette di cercare tutti i piatti che contengono un certo ingrediente */
void gui_search_by_ingredient( GtkWidget* widget, Gui* guiPtr ) {

    GtkWidget* hbox = NULL; // contiene entry e button
    GtkWidget* entry = NULL; // widget che permette di inserire l'ingrediente da cercare
    GtkWidget* button = NULL; // bottone "Cerca"
    GtkWidget* vbox = NULL;
    GList* list = NULL; // lista degli elementi selezionati

    // se alcune rughe sono selezionate, esse vengono deselezionate
    list = gui_menu_list_selected_items(GTK_TREE_MODEL(guiPtr->listStore));
    g_list_free_full( list, free );

    // vbox è il contenitore in cui è inserito il widget per visualizzare la lista dei piatti
    vbox = guiPtr->treeView_box;

    gui_box_new( VERTICAL, guiPtr ); // reimposta il contenuto della finestra

    hbox = gtk_hbox_new( FALSE, 16 ); // crea una nuova hbox

    entry = gtk_entry_new();    // crea il widget per l'inserimento del testo
    g_object_set_data( G_OBJECT(guiPtr->box), "entry", entry );

    button = gtk_button_new_with_label("Cerca"); // crea il bottone "Cerca"
    g_signal_connect( button, "clicked", G_CALLBACK(gui_search_button_clicked), guiPtr );

    gtk_box_pack_start(GTK_BOX(hbox), entry, TRUE, TRUE, 0 );
    gtk_box_pack_start(GTK_BOX(hbox), button, FALSE, FALSE, 0 );
    gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0 );
    g_signal_connect( G_OBJECT(guiPtr->box), "destroy", G_CALLBACK(gui_destroy), hbox );

    gtk_widget_show_all( hbox );
    gtk_widget_hide( guiPtr->menu_button);

    // imposta il pulsante indietro
    g_signal_handler_disconnect( guiPtr->back_button, guiPtr->handler_id);
    guiPtr->handler_id = g_signal_connect( guiPtr->back_button, "button_press_event", G_CALLBACK(gui_print_menu), guiPtr );

}

/* Questa subroutine viene invocata quando l'utente preme il bottone per la ricerca.
 * I piatti contenenti l'ingrediente cercato dall'utente vengono visualizzati in un nuovo widget.
 * Alternativamente sarebbe stato possibile utilizzare un GtkTreeModelFilter per nascondere gli elementi
 * che non contengono l'ingrediente cercato dall'utente, ma il filtro creato avrebbe reso meno efficienti le
 * operazioni di modifica del menu */
void gui_search_button_clicked( GtkWidget* button, Gui* guiPtr ) {

    GtkWidget* treeView = NULL; // widget in cui inserire i piatti che contengono l'ingrediente cercato dall'utente
    GtkEntry* entry = NULL;
    // intestazioni per le colonne del widget treeView
    char* headers[] = {" Id ", " Tipo ", " Nome del Piatto ", " Ingrediente 1 ", " Ingrediente 2 ", " Ingrediente 3 ", " Prezzo ", ""};
    GtkTreeIter iter; // iter corrente
    Piatto* tmp = NULL; // puntatore per la memorizzazione temporanea dei dati
    gboolean cond = FALSE; // condizione del ciclo
    GtkWidget* scrollBox = NULL; // box in cui inserire il widget
    static gulong handler_id = 0; // handler_id e old_id permettono di rimuovere le connessioni create da g_signal_connect
    static gulong old_id = 0;     // all'interno di questa subroutine. Utile nel caso in cui la subroutine venga invocata ripetutamente
    gboolean* flag_filter = NULL;
    int i = 0;

    flag_filter = g_object_get_data(G_OBJECT(guiPtr->treeView), "flag filter");

    // in seguito a ripetute invocazioni di questa subroutine, l'istruzione seguente permette di
    // rimuovere le connessioni create da g_signal_connect, affinché il programma non tenti di distruggere
    // widget che in realtà sono già stati distrutti precedentemente
    if ( *flag_filter == TRUE ) {
        if ( old_id != 0 )
            g_signal_handler_disconnect( button, old_id );
    }
    else {
        *flag_filter = TRUE;
        handler_id = 0;
    }

     // entry assume lo stesso valore della variabile entry dichiarata nella subroutine gui_search_by_ingredient
    entry = g_object_get_data(G_OBJECT(guiPtr->box), "entry" );

    cond = gtk_tree_model_get_iter_first( GTK_TREE_MODEL(guiPtr->listStore), &iter ); // iter si riferisce al primo elemento della lista
                                                                                      // completa dei piatti
    treeView = gui_list_new(headers); // crea un nuovo widget per la visualizzazione dei soli piatti cercati
    while ( cond ) {
        tmp = gui_get_dish_from_list( GTK_TREE_MODEL(guiPtr->listStore), &iter); // recupera il piatto relativo alla riga corrente
        for ( i = 0; i < N_INGR; i++ ) {
            if ( core_strcmp_case_insensitive( entry->text, tmp->ingredienti[i]) == 1 ) {
                gui_add_to_list_dish( treeView, tmp ); // se il piatto contiene l'ingrediente cercato viene aggiunto alla lista
                break;                                 // dei piatti che l'utente desidera visualizzare
            }
        }
        free(tmp); // libera la memoria
        cond = gtk_tree_model_iter_next(GTK_TREE_MODEL(guiPtr->listStore), &iter ); // vai alla riga successiva
    }

    scrollBox = g_object_get_data(G_OBJECT(guiPtr->treeView_box), "scrollBox" );
    gtk_box_pack_start( GTK_BOX(scrollBox), treeView, TRUE, TRUE, 0); // inserisce il widget nella scrolled window

    old_id = handler_id;
    handler_id = g_signal_connect( button, "clicked", G_CALLBACK(gui_destroy), treeView ); // associa il pulsante "cerca" alla distruzione del widget appena creato

    gtk_widget_show(treeView);
    g_object_set_data( G_OBJECT(guiPtr->layout), "filtered_view", treeView);
    gtk_widget_hide( GTK_WIDGET(guiPtr->treeView));
}

/* La seguente funzione crea una lista degli elementi selezionati nel menu e li deseleziona
 * restituisce un puntatore al primo elemento della lista. */
GList* gui_menu_list_selected_items( GtkTreeModel* model ) {

    GtkTreeIter iter, *iterPtr = NULL;  // identificano la riga corrente
    gboolean cond = FALSE;  // condizione di esecuzione del ciclo
    int n = 0;      // valore dell'elemento nell'ultima colonna
    GList* list = NULL; // puntatore al primo elemento della lista degli elementi selezionati

    cond = gtk_tree_model_get_iter_first( model, &iter ); // iter punta alla prima riga

    while ( cond == TRUE ) {
        gtk_tree_model_get( model, &iter, 7, &n, -1 );  // memorizza in n il valore dell'intero presente nella colonna con indice 7 alla riga identificata da iter
        // se l'elemento è selezionato viene aggiunto alla lista
        if ( n != 0 ) {
            iterPtr = (GtkTreeIter*) malloc(sizeof(GtkTreeIter));
            if ( iterPtr == NULL ) {
                gui_error( NULL, "Memoria non disponibile");
                return NULL;
            }
            *iterPtr = iter;
            list = g_list_append(list, iterPtr); // aggiunge l'elemento in coda alla lista
            n = 0;
            gtk_list_store_set( GTK_LIST_STORE( model ), &iter , 7 , n, -1 ); // l'elemento viene deselezionato
        }
        cond = gtk_tree_model_iter_next( model, &iter ); // vai all'elemento successivo
    }

    return list;
}

/* La seguente funzione recupera i dati relativi al piatto che si trova nella posizione specificata da iter
 * e restituisce un puntatore al piatto allocato. */
Piatto* gui_get_dish_from_list( GtkTreeModel* model, GtkTreeIter* iterPtr ) {

    Piatto* piattoPtr = NULL;   // puntatore al piatto da allocare
    // stringhe in cui saranno memorizzati temporaneamente i dati relativi al piatto
    char* nome = NULL;
    char* ingrediente1 = NULL;
    char* ingrediente2 = NULL;
    char* ingrediente3 = NULL;
    char* prezzo_str = NULL;
    char* id_str = NULL;
    char* tipo_str = NULL;

    // memorizza le informazioni relative al piatto in stringhe allocate dinamicamente
    gtk_tree_model_get(model, iterPtr, 0, &id_str, 1, &tipo_str, 2, &nome, 3, &ingrediente1, 4, &ingrediente2, 5, &ingrediente3, 6, &prezzo_str, -1);

    piattoPtr = (Piatto*) malloc( sizeof(Piatto) ); // alloca una variabile di tipo piatto

    if ( piattoPtr != NULL ) {
        // se l'allocazione è andata a buon fine memorizza le informazioni nella
        // zona di memoria allocata
        strcpy( piattoPtr->nome, nome );
        strcpy( piattoPtr->ingredienti[0], ingrediente1 );
        strcpy( piattoPtr->ingredienti[1], ingrediente2 );
        strcpy( piattoPtr->ingredienti[2], ingrediente3 );
        sscanf( prezzo_str, "%f%*s", &piattoPtr->prezzo );
        sscanf( id_str, "%d", &piattoPtr->id );
        // a seconda del carattere iniziale della stringa relativa al tipo del piatto
        // memorizza un valore diverso in piattoPtr->tipo
        switch ( tipo_str[0] ) {
            case 'A': piattoPtr->tipo = antipasto; break;
            case 'P': piattoPtr->tipo = primo; break;
            case 'S': piattoPtr->tipo = secondo; break;
            case 'C': piattoPtr->tipo = contorno; break;
            case 'D': piattoPtr->tipo = dessert; break;
        }
    }

    // libera la memoria precedentemente allocata
    g_free(nome);
    g_free(ingrediente1);
    g_free(ingrediente2);
    g_free(ingrediente3);
    g_free(prezzo_str);
    g_free(tipo_str);
    g_free(id_str);

    if ( piattoPtr == NULL ) {
        gui_error( NULL, "Memoria non disponibile");
        exit(-1);
    }

    return piattoPtr;

}

/* La funzione seguente permette di salvare su un file tutti i dati memorizzati nel GtkListStore. Viene invocata
 * alla chiusura della finestra principale. */

int save_list_to_file( GtkWidget* window, GtkWidget* widget, Gui* guiPtr ) {

    FILE* fp = NULL; // puntatore al file
    Piatto* piattoPtr = NULL; // puntatore al piatto corrente
    GtkTreeIter iter;
    GtkTreeModel* model = NULL;
    gboolean cond = FALSE; // condizione del ciclo
    Bill* cash_register = NULL; // array di scontrini
    ListNode* previous = NULL, *current = NULL; // variabili per scorrere la lista al fine di liberare la memoria
    int i = 0; // indice del ciclo for


    gui_box_new( VERTICAL, guiPtr );

    fp = fopen( "menu.dat", "wb" ); // apre il file in modalità di scrittura

    if ( fp == NULL ){
        gui_error( GTK_WINDOW(window), "Impossibile salvare i dati");
        exit(-1);
    }

    // se alla chiusura del programma sono ancora memorizzati alcuni scontrini, la memoria viene liberata
    cash_register = g_object_get_data ( G_OBJECT(window), "cash_register");
    if ( cash_register != NULL ) {
        for ( i = 0; i < TABLES; i++ ) { // scorre l'array cash_register
            current = cash_register[i].order;
            while ( current != NULL ) { // se è presente qualche scontrino per cui la memoria non è stata liberata
                previous = current;
                current = current->next;
                free( previous ); // libera la memoria per l'elemento corrente
            }
            cash_register[i].order = NULL;
        }
    }


    model = GTK_TREE_MODEL(guiPtr->listStore);

    cond = gtk_tree_model_get_iter_first( model, &iter );   // iter si riferisce al primo elemento del menu
                                                            // se il menu on è vuoto e non si sono verificati errori
                                                            // cond vale TRUE

    while ( cond == TRUE ) {
        piattoPtr = gui_get_dish_from_list( model, &iter ); // ricava il piatto corrispondente all'iter corrente
        if ( fwrite( piattoPtr, sizeof(Piatto), 1, fp ) != 1 ) { // verifica che la scrittura sul file sia avvenuta
            free(piattoPtr);
            fclose(fp);
            exit(-1);
        }
        free(piattoPtr); // libera la memoria allocata per il piatto
        cond = gtk_tree_model_iter_next( model, &iter ); // aggiorna la condizione del ciclo e vai all'elemento successivo
    }

    fclose(fp);
    g_signal_connect( window, "destroy", gtk_main_quit, NULL);
    gtk_widget_destroy(window); // chiude la finestra causando l'uscita dal loop di rendering

    #if DEBUG_MODE
    Piatto piatto_tmp;

    printf("\n****** Contenuto del File menu.dat ******\n\n");
    fp = fopen("menu.dat", "rb");
    while ( !feof(fp) ) {
        if ( fread( &piatto_tmp, sizeof(Piatto), 1, fp ) != 0 ) {
            printf("%03d", piatto_tmp.id );
            printf("  %s\n", piatto_tmp.nome );
            for ( i = 0; i < N_INGR; i++ ) {
                printf("     %s\n", piatto_tmp.ingredienti[i] );
            }
            printf("     %.2f", piatto_tmp.prezzo );
            printf("\n\n");
        }
    }
    printf("*****************************************\n\n");
    fclose(fp);

    #endif // DEBUG_MODE
    return 1;
}

/* La procedura gui_load_list invoca la funzione core_load_list_from_file per leggere i dati presenti
 * nel file che contiene il menu. In seguito, tutti i piatti del menu vengono copiati in una lista
 * per poter essere visualizzati. A questo punto, la memoria allocata dalla funzione core_load_list_from_file viene liberata. */
void gui_load_list( GtkWidget** treeView ) {

    Piatto* piattoPtr = NULL;
    int i = 0, records = 0;
    char* headers[] = {" Id ", " Tipo ", " Nome del Piatto ", " Ingrediente 1 ", " Ingrediente 2 ", " Ingrediente 3 ", " Prezzo ", ""};

    // la funzione gui_list_new predispone la lista dei piatti
    // come intestazione delle colonne vengono usate le stringhe dell'array headers
    *treeView = gui_list_new( headers );

    // la funzione core_load_list_from_file legge i dati salvati nel file
    // e li memorizza in un array. Restituisce il numero degli elementi letti
    records = core_load_list_from_file(&piattoPtr);

    // Ogni elemento dell'array di piatti viene aggiunto alla lista
    for( i = 0; i < records; i++ )
        gui_add_to_list_dish( *treeView, &piattoPtr[i] );

    free(piattoPtr); // libera la memoria precedentemente allocata.

    return;

}

/* La funzione gui_add_new_dish aggiunge un nuovo piatto al menu nella posizione corretta. Riceve in
 * ingresso il widget treeView per la visualizzazione del menu e un puntatore al nuovo piatto*/
int gui_add_new_dish( GtkTreeView* treeView, Piatto* piattoPtr ) {

    GtkTreeIter iter, hold; // iter corrispondente alla riga corrente e iter corrispondente alla posizione in cui aggiungere il nuovo piatto
    GtkTreeModel* model = NULL;
    Piatto* current = NULL; // puntatore al piatto corrente
    gboolean cond = FALSE; // condizione del ciclo
    GtkListStore* listStore = NULL;
    char prezzo_str[15]; // stringa per il prezzo
    char id_str[15]; // stringa per l'id
    char* tipi_piatto[] = {"Antipasto", "Primo", "Secondo", "Contorno", "Dessert"}; // array per la visualizzazione del tipo del piatto

    int flag_pos = 0;

    piattoPtr->id = 0; // inizializza a 0 l'id del piatto

    model = gtk_tree_view_get_model( treeView );

    // dopo l'istruzione alla riga seguente iter si riferisce al primo elemento del menu
    // cond vale FALSE se il menu è vuoto.
    cond = gtk_tree_model_get_iter_first( model, &iter );

    while ( cond == TRUE ) {

        /* La funzione gui_get_dish_from_list permette di ricavare un piatto dalla lista*/
        current = gui_get_dish_from_list( model, &iter ); // current punta al piatto corrente

        if ( core_strcmp_case_insensitive( current->nome, piattoPtr->nome ) == 1  ) {
            return PIATTO_ESISTENTE; // se il nuovo piatto e current hanno lo stesso nome viene restituito un codice di errore
        }
        // i piatti vengono ordinati in base al tipo
        if ( current->tipo <= piattoPtr->tipo ) {
            hold = iter;    // memorizza la posizione in cui inserire il nuovo piatto
            flag_pos = 1;
        }
        // al nuovo piatto viene assegnato un id maggiore rispetto agli id degli altri piatti presenti nella lista
        if ( current->id >= piattoPtr->id ) {
            piattoPtr->id = current->id + 1;
        }
        free( current ); // libera la memoria allocata per il piatto corrente
        cond = gtk_tree_model_iter_next( model, &iter ); // vai al piatto successivo. Se il piatto successivo non è presente cond = FALSE
    }

    // i nuovi ingredienti vengono aggiunti alle scorte
    if ( core_aggiungi_alle_scorte( piattoPtr ) == FILE_ERROR ) return FILE_ERROR;

    listStore = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(treeView)));

    if ( flag_pos == 0 ) // se non è stata trovata la posizione in cui inserire il piatto viene inserito alla fine della lista
        gtk_list_store_prepend(listStore, &iter);
    else
        gtk_list_store_insert_after( listStore, &iter, &hold );  // inserisce il piatto dopo hold

    // inserimento del piatto
    sprintf( id_str, "%03d", piattoPtr->id );
    sprintf( prezzo_str, "%.2f Euro", piattoPtr->prezzo );

    gtk_list_store_set(listStore, &iter, 0, id_str, 1, tipi_piatto[piattoPtr->tipo], 2, piattoPtr->nome, 3, piattoPtr->ingredienti[0],
                       4, piattoPtr->ingredienti[1], 5, piattoPtr->ingredienti[2], 6, prezzo_str, 7, FALSE, -1 );


    return NO_ERRORS;
}

/* La seguente procedura crea la schermata per la gestione delle ordinazioni. Chiama la funzione subroutine gui_print_menu
 * per visualizzare la lista dei piatti e aggiunge i comandi per impostare il numero di porzioni per il piatto selezionato */
void gui_new_bill( GtkWidget* widget, GdkEventButton* eventButton, Gui* guiPtr ) {

    GtkTreeViewColumn* col = NULL; // variabile utilizzata per modificare la rappresentazione dell'ultima colonna
    GtkTreeSelection* selection = NULL; // variabile per gestire la selezione degli elementi nel widget treeView
    GtkWidget* vbox = NULL, *hbox = NULL;
    GtkWidget *incrementButton = NULL, *decrementButton = NULL; // bottoni per impostafre il numero delle porzioni
    GtkWidget* label = NULL;
    GtkWidget* apply = NULL;
    GList* list = NULL;

    gui_box_new( VERTICAL, guiPtr); // pulisce la schermata e crea una nuova vbox
    gui_print_menu(NULL, NULL, guiPtr); // visualizza il menu

    // crea un bottone "applica"
    apply = gui_apply_button(guiPtr);
    g_signal_connect( apply, "button_press_event", G_CALLBACK(gui_set_table_for_order), guiPtr );

    // ricava la colonna con indice 7
    col = gtk_tree_view_get_column( guiPtr->treeView, 7 );

    if ( col != NULL ) gtk_tree_view_remove_column(guiPtr->treeView, col); // rimuove la colonna

    // creazione di una nuova colonna per visualizzare il numero di porzioni
    col = gtk_tree_view_column_new_with_attributes( "Numero porzioni", gtk_cell_renderer_text_new(), "text", 7, NULL);
    gtk_tree_view_append_column( guiPtr->treeView, col);

    // azzera il contenuto dell'ultima colonna
    list = gui_menu_list_selected_items(GTK_TREE_MODEL(guiPtr->listStore));
    g_list_free_full( list, free );

    vbox = guiPtr->treeView_box;
    hbox = gtk_hbox_new (FALSE, 16);

    label = gtk_label_new("Imposta il numero di porzioni per il piatto selezionato:  ");
    gtk_box_pack_start (GTK_BOX(hbox), label, TRUE, TRUE, 0);

    // creazione dei pulsanti per impostare il  numero delle porzioni
    incrementButton = gtk_button_new_with_label ("\n      +      \n");
    g_signal_connect (G_OBJECT (incrementButton), "clicked", G_CALLBACK (increment), guiPtr);
    gtk_box_pack_start (GTK_BOX (hbox), incrementButton, FALSE, FALSE, 0);

    decrementButton = gtk_button_new_with_label ("\n      -      \n");
    g_signal_connect (G_OBJECT (decrementButton), "clicked", G_CALLBACK(decrement), guiPtr);
    gtk_box_pack_start (GTK_BOX (hbox), decrementButton, FALSE, FALSE, 0);

    gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, FALSE, 0);
    g_object_set_data( G_OBJECT(guiPtr->layout), "counterBox", hbox );

    // permette di chiamare la funzione gui_order_dish quando il piatto selezionato cambia
    selection = gtk_tree_view_get_selection(guiPtr->treeView);
    g_signal_connect(selection, "changed", G_CALLBACK(gui_order_dish), guiPtr);

    // hbox e il suo contenuto verranno distrutti insieme a guiPtr->box
    g_signal_connect( G_OBJECT(guiPtr->box), "destroy", G_CALLBACK(gui_destroy), hbox );

    // impostazione del bottone per tornare indietro
    g_signal_handler_disconnect( guiPtr->back_button, guiPtr->handler_id);
    guiPtr->handler_id = g_signal_connect( guiPtr->back_button, "button_press_event", G_CALLBACK(gui_main_window), guiPtr );

    gtk_widget_show_all(vbox);

}

// la seguente procedura permette di distruggere un widget al verificarsi di un evento
void gui_destroy( GtkWidget* widget, GtkWidget* toBeDestroyed ) {

    if ( toBeDestroyed != NULL )
        gtk_widget_destroy(toBeDestroyed);

}

/* Questa subroutine permette di ottenere il numero di porzioni per la riga selezionata.
 * I valori del GtkTreeIter relativo alla riga e del contatore del numero di porzioni vengono
 * memorizzati in variabili static */
void gui_order_dish( GtkTreeSelection* selection, Gui* guiPtr ) {

    static GtkTreeIter iter; // posizione della riga selezionata
    GtkTreeModel* model = NULL; // modello del widget per la visualizzazione dei piatti
    static int counter = 0; // contatore del numero di porzioni

    gtk_image_set_from_file( GTK_IMAGE(guiPtr->bg), "images/bg.jpg" );

    // la funzione gtk_tree_selection_get_selected imposta il valore di iter in modo che si riferisca alla
    // riga selezionata. Restituisce FALSE se nessuna riga è selezionata.
    if ( gtk_tree_selection_get_selected(GTK_TREE_SELECTION(selection), &model, &iter) ) {
        gtk_tree_model_get(model, &iter, 7, &counter,  -1); // recupera il valore dell'intero contenuto nell'ultima colonna
    }

    // Gli indirizzi di counter e iter potranno essere ricavati tramite guiPtr->box e le relative keyword
    // impostate tramite le istruzioni seguenti.
    g_object_set_data( G_OBJECT(guiPtr->box), "counter", &counter );
    g_object_set_data( G_OBJECT(guiPtr->box), "iter", &iter );
}

// procedura chiamata quando viene premuto il bottone per incrementare il numero di porzioni
void increment(GtkWidget *button, Gui* guiPtr ) {

    int* cont = NULL;   // puntatore al contatore del numero di porzioni
    GtkTreeIter* iter = NULL; // puntatore all'iter della riga selezionata

    // recupera gli indirizzi di cont e iter dichiarati nella subroutine gui_order_dish
    cont = g_object_get_data(G_OBJECT(guiPtr->box), "counter");
    iter = g_object_get_data(G_OBJECT(guiPtr->box), "iter");

    // incrementa il valore di cont e lo salva nel list store
    if ( cont != NULL ) {
        ++*cont;
        gtk_list_store_set( guiPtr->listStore, iter , 7 , *cont , -1 );
    }
}

// procedura chiamata quando viene premuto il bottone per decrementare il numero di porzioni
void decrement(GtkWidget *button, Gui* guiPtr ) {
    int* cont = NULL; // contatore del numero di porzioni
    GtkTreeIter* iter = NULL; // iter della riga selezionata

    // recupera gli indirizzi di cont e iter dichiarati nella subroutine gui_order_dish
    iter = g_object_get_data(G_OBJECT(guiPtr->box), "iter");
    cont = g_object_get_data(G_OBJECT(guiPtr->box), "counter");

     // se il numero di porzioni è maggiore di 0 decrementa il valore di cont e lo salva nel list store
    if ( cont != NULL ) {
        if ( *cont > 0 ) --*cont;
        gtk_list_store_set( guiPtr->listStore, iter , 7 , *cont , -1 );
    }
}

/* La seguente funzione invoca la subroutine gui_table_new per creare un'interfaccia che consenta all'utente di
 * scegliere il tavolo */
void gui_set_table_for_order( GtkWidget* widget, GdkEventButton* eventButton, Gui* guiPtr ) {

    gui_box_new(VERTICAL, guiPtr ); // pulisce il contenuto della finestra

    gui_table_new( guiPtr, G_CALLBACK( gui_send_to_bill ) ); // crea una nuova table per la selezione del tavolo
                                                             // Una volta selezionato il tavolo verrà passato il controllo a gui_send_to_bill

    gtk_widget_hide(guiPtr->treeView_box); // nasconde la box contenente il widget per la visualizzazione dei piatti
}

// La seguente procedura crea una table per la selezione dei tavoli
void gui_table_new( Gui* guiPtr, GCallback callback ) {

    GtkWidget *table = NULL, *button = NULL; // dichiarazione della table e dei bottoni da inserire al suo interno
    gchar label[10]; // label del bottone
    int i = 0, j = 0, n = 0; // indici dei cicli e contatore incrementato ad ogni iterazione

    table = gtk_table_new( 5, 3, TRUE ); // crea una table con 5 righe e 3 colonne

    for( i=0; i < 5; i++) {
        for( j=0; j < 3; j++) {
            n++; // n viene incrementato a ogni iterazione: rappresenta il numero del tavolo
            sprintf( label, "Tavolo %d", n ); // scrive su label una stringa contenente il numero del tavolo
            button = gtk_button_new_with_label(label); // crea un nuovo bottone contenente la label corrente
            g_signal_connect(button, "clicked", callback, guiPtr );
            gtk_table_attach_defaults(GTK_TABLE(table), button, j, j+1, i, i+1 ); // inserisce il bottone nella table
        }
    }
    gtk_box_pack_start(GTK_BOX(guiPtr->box), table, TRUE, TRUE, 0);
    gtk_widget_show_all(guiPtr->box);
}

/* La seguente procedura individua gli elementi che l'utente desidera ordinare e chiama la funzione
 * core_add_to_bill per aggiungerli alla lista dei piatti ordinati*/
void gui_send_to_bill( GtkButton* button, Gui* guiPtr ) {

    int table = 0, porzioni = 0; // numero del tavolo e numero di porzioni
    gboolean cond = FALSE; // condizione del ciclo
    GtkTreeModel* model = NULL;
    GtkTreeIter iter;
    Piatto* piattoPtr = NULL; // piatto corrente
    Bill* cash_register = NULL; // puntator al primo elemento dell'array degli scontrini/tavoli

    // Il parametro button rappresenta il bottone premuto dall'utente
    // Dalla label di button è possibile ricavare il numero del tavolo
    sscanf( gtk_button_get_label(button), "%*s%d", &table ); // ignora la stringa "Tavolo" e memorizza in table il numero corrispondente al tavolo
    model = GTK_TREE_MODEL( guiPtr->listStore );

    cond = gtk_tree_model_get_iter_first( model, &iter ); // iter si riferisce al primo elemento della lista

    // scorre tutti gli elementi della lista
    while ( cond == TRUE ) {
        gtk_tree_model_get( model, &iter, 7, &porzioni, -1 ); // ottiene il numero di porzioni per la riga corrente
        if ( porzioni > 0 ) { // se il numero di porzioni per l'elemento corrente è maggiore di 0
            piattoPtr = gui_get_dish_from_list( model, &iter ); // ricava il piatto corrispondente alla riga corrente
            cash_register = core_add_to_bill( *piattoPtr, porzioni, table ); // aggiungi il piatto alla lista dei piatti ordinati
            g_object_set_data( G_OBJECT(guiPtr->window), "cash_register", cash_register ); // consente di ricavare l'array dei tavoli in altre subroutine che ricevono
                                                                                            // guiPtr come parametro
            free( piattoPtr ); // libera la memoria allocata
            gtk_list_store_set( GTK_LIST_STORE( model ), &iter , 7 , 0, -1 ); // reimposta a 0 il numero delle porzioni
        }
        cond = gtk_tree_model_iter_next( model, &iter ); // vai alla riga successiva, se esiste.
    }

    gui_new_bill( NULL, NULL, guiPtr ); // torna alla schermata per le ordinazioni
}

/* Questa subroutine crea l'interfaccia che consente all'utente di stampare lo scontrino per un tavolo*/
void gui_print_button_clicked( GtkButton* widget, GdkEventButton* eventButton, Gui* guiPtr ) {

    gui_box_new( HORIZONTAL, guiPtr ); // pulisce l'interfaccia
    gtk_image_set_from_file( GTK_IMAGE(guiPtr->bg), "images/bg.jpg" ); // reimposta lo sfondo della finestra
    gui_table_new( guiPtr, G_CALLBACK( gui_switch_table_to_print ) ); // crea una nuova table per la selezione del tavolo

    // impostazioni per il tornare alla schermata precedente
    g_signal_handler_disconnect( guiPtr->back_button, guiPtr->handler_id);
    guiPtr->handler_id = g_signal_connect( guiPtr->back_button, "button_press_event", G_CALLBACK(gui_main_window), guiPtr );
}

/* Questa procedura identifica il tavolo per il quale si desidera stampare lo scontrino e, se
 * è presente uno scontrino per il tavolo selezionato, viene chiamata la funzione core_print_bill */
void gui_switch_table_to_print( GtkButton* button, Gui* guiPtr ) {

    int table; // numero del tavolo
    int index; // indice del tavolo nell'array cash_register
    Bill* cash_register = NULL; // array degli scontrini dichiarato nella subroutine core_add_to_bill
    GtkWidget* dialog = NULL; // finestra di dialogo
    int res = 0; // stabilisce se sono avvenuti errori nella stampa dello scontrino

    // individua il numero del tavolo tramite la label del bottone premuto dall'utente
    sscanf( gtk_button_get_label(button), "%*s%d", &table );
    index = table - 1; // l'indice è uguale al numero del tavolo diminuito di uno

    // recupera l'array cash_register
    cash_register = g_object_get_data( G_OBJECT(guiPtr->window), "cash_register" );

    if ( cash_register == NULL ) {
        // se cash_register vale NULL non è mai stata chiamata la funzione gui_send_to_bill, dunque non è mai
        // stato creato alcuno scontrino. Viene visualizzato un messaggio di errore
        dialog = gtk_message_dialog_new(GTK_WINDOW(guiPtr->window), GTK_DIALOG_DESTROY_WITH_PARENT,
                                        GTK_MESSAGE_ERROR , GTK_BUTTONS_OK, "Errore");
        gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(dialog) , "Non e' stato trovato alcuno scontrino da stampare.");
    }
    else if ( cash_register[index].order == NULL ) {
        // non è stato ordinato alcun piatto per il tavolo selezionato.
        dialog = gtk_message_dialog_new(GTK_WINDOW(guiPtr->window), GTK_DIALOG_DESTROY_WITH_PARENT,
                                        GTK_MESSAGE_ERROR , GTK_BUTTONS_OK, "Errore");
        gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(dialog) , "Non e' stato registrato alcun scontrino per\nil tavolo selezionato.");
    }
    else {
        res = core_print_bill( &cash_register[index] ); // stampa lo scontrino
        if ( res == 1 ) { // nessun errore rilevato
            dialog = gtk_message_dialog_new(GTK_WINDOW(guiPtr->window), GTK_DIALOG_DESTROY_WITH_PARENT,
                                            GTK_MESSAGE_INFO , GTK_BUTTONS_OK, "Scontrino stampato correttamente!");
            gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(dialog) , "La stampa dello scontrino e' riuscita con successo");
        }
        else {
            dialog = gtk_message_dialog_new(GTK_WINDOW(guiPtr->window), GTK_DIALOG_DESTROY_WITH_PARENT,
                                        GTK_MESSAGE_ERROR , GTK_BUTTONS_OK, "Errore");
            gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(dialog) , "Errore nella stampa dello scontrino");
        }
    }

    gtk_dialog_run( GTK_DIALOG(dialog) );
    gtk_widget_destroy(dialog);
}

/* Procedura per visualizzare un messaggio di errore. Riceve in ingresso un puntatore alla finestra madre e il testo del messaggio*/
void gui_error( GtkWindow* win, const char* message ) {
    GtkWidget* dialog = NULL;

    dialog = gtk_message_dialog_new(GTK_WINDOW(win) , GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_ERROR  , GTK_BUTTONS_OK, "Errore");
    gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(dialog), "%s", message );
    gtk_dialog_run( GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);

}
