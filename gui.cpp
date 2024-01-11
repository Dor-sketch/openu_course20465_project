#include <gtk/gtk.h>


extern "C" void assemble(char *argv);

class gui {
  public:
    gui() : window(nullptr), button(nullptr), entry(nullptr), label(nullptr) {}
    ~gui() {
        exit(0);
    }

    static void on_button_clicked(GtkWidget *widget, gpointer data) {
        // Call the assemble function here
        // You need to get the filename from the entry widget
        const char *filename =
            gtk_entry_get_text(GTK_ENTRY(((gui *)data)->entry));
        assemble(const_cast<char *>(filename));
    }

    void init(int argc, char *argv[]) {
        gtk_init(&argc, &argv);

        window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
        gtk_window_set_title(GTK_WINDOW(window), "Assembler");
        gtk_window_set_default_size(GTK_WINDOW(window), 300, 100);
        gtk_container_set_border_width(GTK_CONTAINER(window), 10);

        button = gtk_button_new_with_label("Assemble");
        entry = gtk_entry_new();
        label = gtk_label_new("Enter filename:");

        g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
        g_signal_connect(button, "clicked", G_CALLBACK(on_button_clicked),
                         this);

        GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
        gtk_box_pack_start(GTK_BOX(vbox), label, FALSE, FALSE, 0);
        gtk_box_pack_start(GTK_BOX(vbox), entry, FALSE, FALSE, 0);
        gtk_box_pack_start(GTK_BOX(vbox), button, FALSE, FALSE, 0);
        gtk_container_add(GTK_CONTAINER(window), vbox);

        gtk_widget_show_all(window);
    }

    char *get_filename() {
        return const_cast<char *>(gtk_entry_get_text(GTK_ENTRY(entry)));
    }

    char *get_filename() const {
        return const_cast<char *>(gtk_entry_get_text(GTK_ENTRY(entry)));
    }

    GtkWidget *window;
    GtkWidget *button;
    GtkWidget *entry;
    GtkWidget *label;
};

extern "C" void start_gui(int argc, char *argv[]) {
    gui g;
    g.init(argc, argv);
    gtk_main();
}

extern "C" int get_gui_input(char ***argv) {
    gui g;
    gtk_main();
    g_object_unref(g.window);
    *argv = g_strsplit(gtk_entry_get_text(GTK_ENTRY(g.entry)), " ", 0);
    gtk_main_quit();
    return gtk_init_check(0, argv);
}
