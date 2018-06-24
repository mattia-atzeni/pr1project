#ifndef GUI_H
#define GUI_H
#include <gtk/gtk.h>
#include "core.h"

#define WIN_WIDTH 1024
#define WIN_HEIGHT 600

// posizione della barra e dei pulsanti al suo interno
#define BAR_POS WIN_HEIGHT- 40
#define BAR_LEFT 0
#define BAR_RIGHT WIN_WIDTH - 100

// tipi della box
#define VERTICAL 'v'
#define HORIZONTAL 'h'

typedef struct {
    GtkWidget* window; // finestra
    GtkWidget* bg; // sfondo
    GtkWidget* layout; // layout
    GtkWidget* box; // hbox o vbox
    GtkWidget* bar; // barra in basso nella finestra
    GtkWidget* menu_button; // bottone posizionato nella barra
    GtkWidget* back_button; // bottone per poter tornare alle schermate precedenti
    GtkTreeView* treeView; // widget per la visualizzazione del menu
    GtkWidget* treeView_box; // box in cui è inserito il widget treeView
    GtkListStore* listStore; // list store relativo al widget treeView
    gulong handler_id; // intero per identificare la callback precedentemente associata alla pressione del bottone back_button
    gpointer ptr;
} Gui; // definizione del tipo Gui. Contiene gli elementi principali che costituiscono l'interfaccia del programma


// prototipi delle funzioni
void gui_init( Gui*, GtkWidget* );
void gui_main_window(GtkWidget*, GdkEventButton*, Gui*);
void gui_box_new( char, Gui* );
void gui_image_button_new( char*, GtkWidget*, GCallback, gpointer );
void gui_print_menu(GtkWidget*, GdkEventButton*, Gui*);
void gui_menu_button_clicked( GtkWidget*, GdkEventButton*, Gui*);
GtkWidget* gui_list_new( char** );
void gui_add_to_list_with_icon(GtkWidget*, const char*, char* );
void gui_add_to_list_dish(GtkWidget*, Piatto* );
void gui_list_selection( GtkWidget*, Gui*);
void gui_on_toggle( GtkCellRenderer*, char*, GtkTreeView*);
void gui_scegli_tipo_piatto(GtkWidget*, Gui*);
void gui_memorizza_tipo_piatto(GtkWidget*, GdkEventButton*, Gui*);
void gui_entry_new( Gui*, char* );
void gui_combo_new( Gui*, TipoPiatto );
void gui_nuovo_piatto_info( Gui*, gboolean );
int gui_memorizza_info_piatto( Gui*, gboolean );
void gui_dialog_creazione_piatto(GtkWidget*, GdkEventButton*, Gui*);
void gui_del_from_list( GtkWidget*, Gui* );
void gui_switch_fields_to_mod( GtkWidget*, Gui* );
int gui_mod_dish( Gui* guiPtr, Piatto* piattoPtr );
GList* gui_menu_list_selected_items( GtkTreeModel* );
Piatto* gui_get_dish_from_list( GtkTreeModel*, GtkTreeIter* );
void gui_search_by_ingredient( GtkWidget*, Gui* win );
void gui_search_button_clicked( GtkWidget* button, Gui* );
gboolean ingredient_filter( GtkTreeModel* model, GtkTreeIter* iter, char* str );
int save_list_to_file( GtkWidget*, GtkWidget*, Gui* );
void gui_load_list( GtkWidget** treeView );
int gui_add_new_dish( GtkTreeView* treeView, Piatto* piattoPtr );
GtkWidget* gui_apply_button(Gui*);
void gui_destroy( GtkWidget*, GtkWidget* );
void gui_set_table_for_order(GtkWidget*, GdkEventButton*, Gui*);
void gui_error(GtkWindow*, const char* message);
void gui_new_bill( GtkWidget*, GdkEventButton*, Gui* );
void gui_order_dish( GtkTreeSelection*, Gui* );
void increment(GtkWidget *button, Gui* );
void decrement(GtkWidget *button, Gui* );
void gui_table_new( Gui*, GCallback );
void gui_send_to_bill( GtkButton*, Gui* );
void gui_print_button_clicked( GtkButton* button, GdkEventButton*, Gui* guiPtr );
void gui_switch_table_to_print( GtkButton*, Gui* );

#endif
