/*
* Warning: this program assumes cp1250 encoding of text files.
* I know this sucks but it was part of the specification at the
* time I was programming it.
*/

#include "solver/words.h"
#include "solver/board.h"
#include "solver/solver.h"
#include "solver/defines.h"

#include <stdlib.h>
#include <locale.h>
#include <cairo.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <string.h>
#include <stdio.h>

/*===global data===*/
Board     *board=0;
Words     *words=0;
Node      *selected_node=0;
GtkWidget *drawing_area=0;
GtkWidget *list_view=0;
int        selected_area[MAX_WORD_LEN][MAX_WORD_LEN];

/*===enums and defines===*/
enum modes {structure_mode,fill_mode};
enum modes mode = fill_mode;
enum
{
  LIST_ITEM = 0,
  N_COLUMNS
};

/* defines for board widget */
#define CELL_SIZE_PX 32
#define FONT_SIZE (CELL_SIZE_PX/2)
#define BORDER_SIZE_PX 1
#define BOARD_SIZE_X (board->size_x)
#define BOARD_SIZE_Y (board->size_y)
#define MAP_POS_TO_TB_INDEX_X(pos) ((pos)/(CELL_SIZE_PX+BORDER_SIZE_PX))
#define MAP_POS_TO_TB_INDEX_Y(pos) ((pos)/(CELL_SIZE_PX+BORDER_SIZE_PX))
#define IS_OUT_OF_BOUNDS(x,y) ((x)<0||(y)<0||(x)>=BOARD_SIZE_X||(y)>=BOARD_SIZE_Y)
/* end of defines for board widget */

/*===functions' prototypes===*/

static void initCluesList(GtkWidget *list_view);
static void addToList(GtkWidget *list_view, const gchar *str);
static void clearList(GtkWidget *list_view);
static void fillClues(GtkWidget *list_view,GPtrArray *str_list);

static void selectArea(Node *n);
gboolean drawBoardOnDrawable(GtkWidget *widget, GdkEventExpose *event, gpointer data);
gboolean handleMouseOnDrawable(GtkWidget *widget, GdkEvent  *event, gpointer list_view);

void show_info_box(GtkWidget *widget, gpointer window,const char *msg,const char *title);
void gtk_filter_add(GtkWidget *file_chooser,const gchar *title,const gchar *pattern);

static void on_word_selection_changed(GtkWidget *widget, gpointer entry);
void on_file_open_activate(GtkWidget *widget, gpointer data);
void on_solve_activate(GtkWidget *widget, gpointer data);
void on_insert_word_clicked(GtkWidget *widget, gpointer entry);
void on_erase_word_clicked(GtkWidget *widget, gpointer data);
void on_structure_mode_activate(GtkWidget *widget, gpointer data);
void on_fill_mode_activate(GtkWidget *widget, gpointer data);
void on_file_new_activated(GtkWidget *widget, gpointer data);
void on_file_save_as_activated(GtkWidget *widget, gpointer data);

char *getOpenFileName(const char *title,const char *filter_name,const char *filter);
char *getSaveFileName(const char *title,const char *filter_name,const char *filter);

/*===function definitions===*/

void initCross(const char *board_name,const char *words_name,int size_x,int size_y)
{
    if(board!=0)
        freeBoard(board);
    if(words!=0)
        freeWords(words);
    selected_node = 0;
    deinitSolver();
    if(words_name==0)
    {
        fprintf(stderr,"No dictionary selected for the crossword in initCross()");
        exit(1);
    }
    WordsData words_data = wordsDataFromFile(words_name);
	words = newWords();
	initWords(words,words_data.data,words_data.size);
	free(words_data.data);
    if(board_name==0)
    {
        int *data = (int*)malloc(sizeof(int)*size_x*size_y);
        for(int i=0;i<size_y*size_x;++i)
        {
            data[i]=BLANK;
        }
        board = newBoard();
        initBoard(board,data,size_x,size_y);
        updateNodes(board);
        free(data);
        initSolver(board,words);
        fillClues(list_view,0);
        gtk_widget_queue_draw (drawing_area);
        return;
    }
	int *board_data = readBoardDataFromFile(board_name,&size_x,&size_y);
	board = newBoard();
	initBoard(board,board_data,size_x,size_y);
	updateNodes(board);
	free(board_data);
	initSolver(board,words);
    fillClues(list_view,0);
	gtk_widget_queue_draw (drawing_area);
}


char *getOpenFileName(const char *title,const char *filter_name,const char *filter)
{
    GtkWidget *dialog;
    dialog = gtk_file_chooser_dialog_new (title,
                                      0,
                                      GTK_FILE_CHOOSER_ACTION_OPEN,
                                      GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                                      GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
                                      NULL);
    gtk_filter_add(dialog, filter_name, filter);
    char *filename=0;
    if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)
    {

        filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));

    }
    gtk_widget_destroy (dialog);
    return filename;
}

char *getSaveFileName(const char *title,const char *filter_name,const char *filter)
{
    GtkWidget *dialog;
    dialog = gtk_file_chooser_dialog_new (title,
                                      0,
                                      GTK_FILE_CHOOSER_ACTION_SAVE,
                                      GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                                      GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT,
                                      NULL);
    gtk_file_chooser_set_do_overwrite_confirmation (GTK_FILE_CHOOSER (dialog), TRUE);
    gtk_filter_add(dialog, filter_name, filter);
    char *filename=0;
    if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)
    {
        filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
    }
    gtk_widget_destroy (dialog);
    return filename;
}

static void selectArea(Node *n)
{
    for(int i=0;i<board->size_y;++i)
        for(int j=0;j<board->size_x;++j)
            selected_area[i][j]=0;
    Cord cord = n->cord;
    while(1)
	{
		if(cord.x<0||cord.x>=board->size_x||cord.y<0||cord.y>=board->size_y)
			break;
		if(board->data[cord.y][cord.x]==SOLID)
			break;
        selected_area[cord.y][cord.x]=1;
		cord = nextCord(cord);
	}
}

static void initCluesList(GtkWidget *list_view)
{
    GtkCellRenderer *renderer;
    GtkTreeViewColumn *column;
    GtkListStore *store;
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("List Items",renderer, "text", LIST_ITEM, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(list_view), column);
    store = gtk_list_store_new(N_COLUMNS, G_TYPE_STRING);
    gtk_tree_view_set_model(GTK_TREE_VIEW(list_view),GTK_TREE_MODEL(store));
    g_object_unref(store);
}

static void addToList(GtkWidget *list_view, const gchar *str)
{
    GtkListStore *store;
    GtkTreeIter iter;
    store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(list_view)));
    gtk_list_store_append(store, &iter);
    /*The stored value will be referenced by the store if it is a G_TYPE_OBJECT,
    * and it will be copied if it is a G_TYPE_STRING or G_TYPE_BOXED.*/
    char *utf_str = g_convert(str,-1,"UTF-8","CP1250",0,0,0);
    gtk_list_store_set(store, &iter, LIST_ITEM, utf_str, -1);
    free(utf_str);
}

static void clearList(GtkWidget *list_view)
{
    GtkListStore *store;
    store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(list_view)));
    gtk_list_store_clear(store);
}

static void fillClues(GtkWidget *list_view,GPtrArray *str_list)
{
    clearList(list_view);
    if(str_list==0)
        return;
    for(int i=0;i<str_list->len;++i)
    {
        char *str = g_ptr_array_index(str_list,i);
        addToList(list_view,str);
    }
}
/*
int checkIfWordCanBePlaced(Node *n,const char*str)
{
    char *original_str = getNodeString(board,n);
	fillNodeString(board,n,str);


	GPtrArray *intersections = getIntersectingNodesArray(board,n);
	for(int i=0;i<intersections->len;++i)
    {
        Node *intersection = g_ptr_array_index(intersections,i);
        char *intersection_str = getNodeString(board,intersection);
        int does_have_match = doesHaveMatch(words,intersection_str) || (wildcardMatch(intersection_str,str)==1);
        if(does_have_match == 0)
        {
            fillNodeString(board,n,original_str);
            return 0;
        }
    }
	fillNodeString(board,n,original_str);
	return 1;
}
*/
gboolean drawBoardOnDrawable(GtkWidget *widget, GdkEventExpose *event, gpointer data)
{
    if(board==0)
        return FALSE;
    int request_x = BOARD_SIZE_X*CELL_SIZE_PX+BORDER_SIZE_PX*(BOARD_SIZE_X-1);
    int request_y = BOARD_SIZE_Y*CELL_SIZE_PX+BORDER_SIZE_PX*(BOARD_SIZE_Y-1);
    gtk_widget_set_size_request (widget, request_x, request_y);


    char buff[2];
    cairo_t *cr;
    cr = gdk_cairo_create(widget->window);
    cairo_set_font_size(cr,FONT_SIZE);

    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_rectangle(cr,0,0,request_x,request_y);
    cairo_fill(cr);
    for(int i=0;i<board->size_y;++i)
    {
        for(int j=0;j<board->size_x;++j)
        {

            int type = board->data[i][j];


            if(type==SOLID)
            {
                cairo_set_source_rgb(cr, 0, 0, 0);
            }
            else
            {
                if(selected_node!=0 && selected_area[i][j]==1)
                {
                    cairo_set_source_rgb(cr, .75, .75, .75);
                }
                else
                {
                    cairo_set_source_rgb(cr, 1, 1, 1);
                }
            }
            int top_left_x = j*CELL_SIZE_PX+BORDER_SIZE_PX*j;
            int top_left_y = i*CELL_SIZE_PX+BORDER_SIZE_PX*i;
            cairo_rectangle(cr,top_left_x,top_left_y,CELL_SIZE_PX,CELL_SIZE_PX);
            cairo_fill(cr);
            if(board->data[i][j]!=BLANK)
            {
                /*later look at size_extents in cairo doc*/
                cairo_move_to(cr, top_left_x+CELL_SIZE_PX/3.0, top_left_y+2*CELL_SIZE_PX/3);
                buff[0] = board->data[i][j];
                buff[1]=0;
                //char *utf_str = g_locale_to_utf8(buff,-1,0,0,0);
                //g_convert (text, length, "UTF-8", "ISO-8859-1",
                //                NULL, NULL, &error);
                char *utf_str = g_convert(buff,-1,"UTF-8","CP1250",0,0,0);
                cairo_set_source_rgb(cr, 0, 0, 0);
                cairo_show_text(cr,utf_str);
            }
        }
    }
    cairo_destroy(cr);
    return FALSE;
}
gboolean handleMouseOnDrawable(GtkWidget *widget, GdkEvent  *event, gpointer list_view)
{
    GdkEventButton * e = (GdkEventButton *)event;
    int which_button = e->button;
    int pos_x = e->x;
    int pos_y = e->y;
    enum direction dir;
    if(which_button==1)
        dir = ACROSS;
    else
        dir = DOWN;
    //g_print("Button pressed at (%dpx,%dpx)\n",pos_x,pos_y);
    int x = MAP_POS_TO_TB_INDEX_X(pos_x);
    int y = MAP_POS_TO_TB_INDEX_Y(pos_y);
    //g_print("This is index (%d,%d)\n",x,y);
    if(IS_OUT_OF_BOUNDS(x,y)==1)
    {
        //g_print("Out of bounds\n");
        return FALSE;
    }

    if(mode == fill_mode)
    {
        selected_node = findNodeAt(board,x,y,dir);
        if(selected_node!=0)
        {
            selectArea(selected_node);
            char *node_str = getNodeString(board,selected_node);
            GPtrArray *matching_words = arrayOfMatchingWords(words,node_str);
            /*
            for(int i=0;i<matching_words->len;++i)
            {
                char *tmp_str = g_ptr_array_index(matching_words,i);
                if(checkIfWordCanBePlaced(selected_node,tmp_str)==0)
                    g_ptr_array_remove_index_fast(matching_words,i);
            }*/
            fillClues(GTK_WIDGET(list_view),matching_words);
            free(node_str);
        }
    }
    else if(mode==structure_mode)
    {
        selected_node = 0;
        if(board->data[y][x]==SOLID)
            board->data[y][x]=BLANK;
        else
            board->data[y][x]=SOLID;
        updateNodes(board);
    }
    gtk_widget_queue_draw (widget);
    return FALSE;
}

static void on_word_selection_changed(GtkWidget *widget, gpointer entry)
{
    GtkTreeIter iter;
    GtkTreeModel *model;
    char *value;
    if (gtk_tree_selection_get_selected(GTK_TREE_SELECTION(widget), &model, &iter))
    {
        gtk_tree_model_get(model, &iter, LIST_ITEM, &value,  -1);
        gtk_entry_set_text(GTK_ENTRY(entry), value);
        g_free(value);
    }
}


/*
usage is like this:
gtk_filter_add(dialog, "Audio Files", "*.mp3;*.ogg;*.wav");
gtk_filter_add(dialog, "MP3 Files", "*.mp3");
gtk_filter_add(dialog, "OGG Files", "*.ogg");
gtk_filter_add(dialog, "WAV Files", "*.wav");
*/

void gtk_filter_add(GtkWidget *file_chooser,const gchar *title,const gchar *pattern)
{
   gchar **patterns;
   gint i;

    GtkFileFilter *f = gtk_file_filter_new();
    gtk_file_filter_set_name(f, title);
    /* get the patterns */
    patterns = g_strsplit(pattern, ";", -1);
    for(i = 0; patterns[i] != NULL; i++)
    {
        gtk_file_filter_add_pattern(f, patterns[i]);
        /*g_print("%s\n", patterns[i]);*/
    }
    /* free the patterns */
    g_strfreev(patterns);
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(file_chooser), f);
}

void on_file_open_activate(GtkWidget *widget, gpointer data)
{
    char *board_filename = getOpenFileName("Open board","TXT File (*.txt)", "*.txt");
    if(board_filename==0)
        return;
    char *words_filename = getOpenFileName("Open words","LST File (*.lst)", "*.lst");
    if(words_filename==0)
    {
        g_free (board_filename);
        return;
    }
    initCross(board_filename,words_filename,1,1);
    g_free (board_filename);
    g_free (words_filename);
}

void show_info_box(GtkWidget *widget, gpointer window,const char *msg,const char *title)
{
    GtkWidget *dialog;
    dialog = gtk_message_dialog_new(GTK_WINDOW(window),
            GTK_DIALOG_DESTROY_WITH_PARENT,
            GTK_MESSAGE_INFO,
            GTK_BUTTONS_OK,
            msg);
    gtk_window_set_title(GTK_WINDOW(dialog), title);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

void on_solve_activate(GtkWidget *widget, gpointer data)
{
    if(board==0 || words==0)
        return;

    int res = solve();

    fPrintBoard(board,stdout);
    gtk_widget_queue_draw (drawing_area);
    if(res==0)
    {
        show_info_box(widget,0,"Could not find solution using provided dictionary","Solution not found");
    }
}

void on_insert_word_clicked(GtkWidget *widget, gpointer entry)
{
    /*entry_text should not be freed*/
    const char *entry_text = gtk_entry_get_text (GTK_ENTRY(entry));

    char *entry_text_cp1250 = g_convert(entry_text,-1,"CP1250","UTF-8",0,0,0);

    if(selected_node==0)
        return;
    putWord(selected_node,entry_text_cp1250);
    gtk_widget_queue_draw (drawing_area);
    free(entry_text_cp1250);
}
void on_erase_word_clicked(GtkWidget *widget, gpointer data)
{
    static char clear_string[MAX_WORD_LEN];
    static int string_initialised;
    if(string_initialised==0)
    {
        for(int i=0;i<MAX_WORD_LEN;++i)
            clear_string[i]=BLANK;
        clear_string[MAX_WORD_LEN-1]=0;
    }
    string_initialised = 1;
    if(selected_node==0)
        return;
    putWord(selected_node,clear_string);
    gtk_widget_queue_draw (drawing_area);
}
void on_structure_mode_activate(GtkWidget *widget, gpointer data)
{
    mode = structure_mode;
    selected_node = 0;
    gtk_widget_queue_draw (drawing_area);
    gpointer *arr = data;
    //gtk_widget_set_sensitive(GTK_WIDGET(widget),FALSE);
    gtk_widget_set_sensitive(GTK_WIDGET(widget),FALSE);
    gtk_widget_set_sensitive(GTK_WIDGET(arr[0]),TRUE);
    gtk_widget_set_sensitive(GTK_WIDGET(arr[1]),FALSE);
}
void on_fill_mode_activate(GtkWidget *widget, gpointer data)
{
    deinitSolver();
    initSolver(board,words);
    mode = fill_mode;
    gpointer *arr = data;
    gtk_widget_set_sensitive(GTK_WIDGET(widget),FALSE);
    gtk_widget_set_sensitive(GTK_WIDGET(arr[0]),TRUE);
    gtk_widget_set_sensitive(GTK_WIDGET(arr[1]),TRUE);
}

void on_file_new_activated(GtkWidget *widget, gpointer data)
{
    GtkWidget *label, *content_area, *spin_size_x, *spin_size_y;
    GtkWidget *dialog = gtk_dialog_new_with_buttons ("My dialog",
                                                 0,
                                                 GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                                 GTK_STOCK_OK,
                                                 GTK_RESPONSE_ACCEPT,
                                                 GTK_STOCK_CANCEL,
                                                 GTK_RESPONSE_REJECT,
                                                 NULL);
    content_area = gtk_dialog_get_content_area (GTK_DIALOG (dialog));
    label = gtk_label_new ("Size x");
    gtk_container_add (GTK_CONTAINER (content_area), label);
    spin_size_x = gtk_spin_button_new_with_range  (2,MAX_WORD_LEN-1,1);
    gtk_container_add (GTK_CONTAINER (content_area), spin_size_x);
    label = gtk_label_new ("Size y");
    gtk_container_add (GTK_CONTAINER (content_area), label);
    spin_size_y = gtk_spin_button_new_with_range  (2,MAX_WORD_LEN-1,1);
    gtk_container_add (GTK_CONTAINER (content_area), spin_size_y);
    gtk_widget_show_all(dialog);
    gint result = gtk_dialog_run (GTK_DIALOG (dialog));
    if(result == GTK_RESPONSE_ACCEPT)
    {
        char *words_filename = getOpenFileName("Open words","LST File (*.lst)", "*.lst");
        if(words_filename==0)
            return;
        initCross(0,words_filename,gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON(spin_size_x)),gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON(spin_size_y)));
        g_free(words_filename);
    }
    gtk_widget_destroy (dialog);

}
void on_file_save_as_activated(GtkWidget *widget, gpointer data)
{
    char *filename = getSaveFileName("Save board","TXT File (*.txt)", "*.txt");
    if(filename==0)
        return;
    saveBoardToFile(board,filename);
    g_free (filename);
}


int main(int argc, char **argv)
{
    //setlocale(LC_ALL,"pl_PL");

    GtkWidget *win=0;

    GtkWidget *vbox1=0;
    GtkWidget *vbox2=0;
    GtkWidget *hbox1=0;
    GtkWidget *hbox2=0;

    GtkWidget *menubar=0;
    GtkWidget *file_menu_item=0;
    GtkWidget *file_menu=0;
    GtkWidget *new_file=0;
    GtkWidget *open_file=0;
    //GtkWidget *save_file=0;
    GtkWidget *save_as_file=0;
    GtkWidget *quit_file=0;
    GtkWidget *edit_menu_item=0;
    GtkWidget *edit_menu=0;
    GtkWidget *structure_mode_edit=0;
    GtkWidget *fill_mode_edit=0;
    GtkWidget *solve_edit=0;
    GtkWidget *sep=0;

    GtkWidget *scrolled_window=0;

    GtkWidget *word_entry=0;
    GtkWidget *insert_word_button=0;
    GtkWidget *erase_word_button=0;
    GtkWidget *scrolled_window2=0;

    GtkTreeSelection *list_view_selection = 0;

    /* Initialize GTK+ */
    g_log_set_handler ("Gtk", G_LOG_LEVEL_WARNING, (GLogFunc) gtk_false, NULL);
    gtk_init (&argc, &argv);
    g_log_set_handler ("Gtk", G_LOG_LEVEL_WARNING, g_log_default_handler, NULL);

    /* Create the main window */
    win = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title (GTK_WINDOW (win), "CROSS");
    gtk_window_set_position (GTK_WINDOW (win), GTK_WIN_POS_CENTER);
    gtk_widget_realize (win);
    g_signal_connect (win, "destroy", gtk_main_quit, NULL);
    gtk_widget_set_size_request (win, 780, 550);


    /* Menu */
    menubar = gtk_menu_bar_new();

    file_menu = gtk_menu_item_new_with_mnemonic("_File");
    file_menu_item = gtk_menu_new();

    new_file = gtk_image_menu_item_new_from_stock(GTK_STOCK_NEW, NULL);
    open_file = gtk_image_menu_item_new_from_stock(GTK_STOCK_OPEN, NULL);
    //save_file = gtk_image_menu_item_new_from_stock(GTK_STOCK_SAVE, NULL);
    save_as_file = gtk_image_menu_item_new_from_stock(GTK_STOCK_SAVE_AS, NULL);
    quit_file = gtk_image_menu_item_new_from_stock(GTK_STOCK_QUIT, NULL);
    sep = gtk_separator_menu_item_new();

    gtk_menu_item_set_submenu(GTK_MENU_ITEM(file_menu), file_menu_item);
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu_item), new_file);
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu_item), open_file);
    //gtk_menu_shell_append(GTK_MENU_SHELL(file_menu_item), save_file);
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu_item), save_as_file);
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu_item), sep);
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu_item), quit_file);
    gtk_menu_shell_append(GTK_MENU_SHELL(menubar), file_menu);

    edit_menu = gtk_menu_item_new_with_mnemonic("_Edit");
    edit_menu_item = gtk_menu_new();

    structure_mode_edit = gtk_menu_item_new_with_label("Structure");
    fill_mode_edit = gtk_menu_item_new_with_label("Fill");
    gtk_widget_set_sensitive(fill_mode_edit,FALSE);
    solve_edit = gtk_menu_item_new_with_label("Solve");
    sep = gtk_separator_menu_item_new();

    gtk_menu_item_set_submenu(GTK_MENU_ITEM(edit_menu), edit_menu_item);
    gtk_menu_shell_append(GTK_MENU_SHELL(edit_menu_item), structure_mode_edit);
    gtk_menu_shell_append(GTK_MENU_SHELL(edit_menu_item), fill_mode_edit);
    gtk_menu_shell_append(GTK_MENU_SHELL(edit_menu_item), sep);
    gtk_menu_shell_append(GTK_MENU_SHELL(edit_menu_item), solve_edit);
    gtk_menu_shell_append(GTK_MENU_SHELL(menubar), edit_menu);

    /* Scrolled windows and drawing area */
    scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window),GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(scrolled_window),GTK_SHADOW_IN);

    drawing_area = gtk_drawing_area_new();
    gtk_widget_add_events(drawing_area,GDK_BUTTON_PRESS_MASK);
    gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scrolled_window),drawing_area);

    /* Word entry */
    word_entry = gtk_entry_new();

    /* List view */
    list_view = gtk_tree_view_new();
    gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(list_view), FALSE);
    initCluesList(list_view);
    /*addToList(list_view,"test");
    clearList(list_view);
    addToList(list_view,"test1");
    addToList(list_view,"test2");
    addToList(list_view,"test3");*/

    scrolled_window2 = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window2),GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(scrolled_window2),GTK_SHADOW_IN);
    gtk_container_add(GTK_CONTAINER(scrolled_window2),list_view);
    list_view_selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(list_view));

    /* Buttons */

    insert_word_button=gtk_button_new_with_label("Insert");
    erase_word_button=gtk_button_new_with_label("Erase Selected");

    /* Create layout */
    vbox1 = gtk_vbox_new(FALSE, 0);
    vbox2 = gtk_vbox_new(FALSE, 0);
    hbox1 = gtk_hbox_new(FALSE, 0);
    hbox2 = gtk_hbox_new(FALSE, 0);
    gtk_container_add (GTK_CONTAINER (win), vbox1);
    /* First vbox1 child is menubar */
    //gtk_container_add (GTK_CONTAINER (vbox1), menubar);
    gtk_box_pack_start(GTK_BOX(vbox1), menubar, FALSE, FALSE, 0);
    /* Second vbox1 child is hbox1 */
    gtk_container_add (GTK_CONTAINER (vbox1), hbox1);
    /* First hbox1 child is scrolled_window */
    gtk_box_pack_start (GTK_BOX (hbox1), scrolled_window, TRUE,TRUE,0);
    /* Second hbox1 child is vbox2 */
    gtk_box_pack_start (GTK_BOX (hbox1), vbox2, FALSE,FALSE,0);
    /* First vbox2 child is word entry */
    gtk_box_pack_start (GTK_BOX (vbox2),  word_entry, FALSE,FALSE,0);
    /* Second vbox2 child is hbox2 */
    gtk_box_pack_start (GTK_BOX (vbox2), hbox2, FALSE,FALSE,0);
    /* Two buttons */
    gtk_box_pack_start (GTK_BOX (hbox2), insert_word_button, FALSE,FALSE,0);
    gtk_box_pack_start (GTK_BOX (hbox2), erase_word_button, FALSE,FALSE,0);
    /* Third vbox2 child is list view */
    gtk_box_pack_start (GTK_BOX (vbox2), scrolled_window2, TRUE,TRUE,0);


    /* Hacking area */
    //reinitCrossword("board/15x15.txt","dict/eng.lst");
    //initCross("board/15x15.txt","dict/eng.lst",1,1);
    /*
    selected_node = findNodeAt(board,3,3,DOWN);
    selectArea(selected_node);
    fillNodeString(board,selected_node,"be");
    char *node_str = getNodeString(board,selected_node);
    GPtrArray *matching_words = arrayOfMatchingWords(words,node_str);
    fillClues(list_view,matching_words);
    */

    /* Connect signals and events */
    g_signal_connect (G_OBJECT (drawing_area), "expose_event",G_CALLBACK (drawBoardOnDrawable), NULL);
    g_signal_connect (G_OBJECT (drawing_area), "button-press-event",G_CALLBACK (handleMouseOnDrawable), list_view);
    g_signal_connect(G_OBJECT(quit_file), "activate",G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(G_OBJECT(open_file), "activate",G_CALLBACK(on_file_open_activate), NULL);
    g_signal_connect(G_OBJECT(solve_edit), "activate",G_CALLBACK(on_solve_activate), NULL);
    gpointer solve_and_fill[2];
    solve_and_fill[0] = (gpointer)fill_mode_edit;
    solve_and_fill[1] = (gpointer)solve_edit;
    gpointer structure_and_solve[2];
    structure_and_solve[0] = (gpointer)structure_mode_edit;
    structure_and_solve[1] = (gpointer)solve_edit;
    g_signal_connect(G_OBJECT(structure_mode_edit), "activate",G_CALLBACK(on_structure_mode_activate), solve_and_fill);
    g_signal_connect(G_OBJECT(fill_mode_edit), "activate",G_CALLBACK(on_fill_mode_activate), structure_and_solve);
    g_signal_connect(list_view_selection, "changed", G_CALLBACK(on_word_selection_changed), word_entry);
    g_signal_connect(insert_word_button, "clicked", G_CALLBACK(on_insert_word_clicked), word_entry);
    g_signal_connect(erase_word_button, "clicked", G_CALLBACK(on_erase_word_clicked), word_entry);
    g_signal_connect(G_OBJECT(new_file), "activate",G_CALLBACK(on_file_new_activated), NULL);
    g_signal_connect(G_OBJECT(save_as_file), "activate",G_CALLBACK(on_file_save_as_activated), NULL);

    /* Enter event loop */
    gtk_widget_show_all (win);
    gtk_main ();
	return 0;
}

