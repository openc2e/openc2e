#include <gtk/gtk.h>
#include "gtkhelp.h"

char* GtkHelp::ExtractText(GtkWidget* window, char *item)
{
	return gtk_entry_get_text(GTK_ENTRY(gtk_object_get_data(GTK_OBJECT(window), item)));
}

