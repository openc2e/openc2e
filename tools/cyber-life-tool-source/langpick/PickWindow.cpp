
#include "PickWindow.h"

#include <vector>
#include <stdio.h>
#include <string>

//static
gint PickWindow::HandleDeleteEvent(GtkWidget* w,GdkEvent* ev,gpointer data )
{
	return FALSE;	// bye bye
}


//static
void PickWindow::HandleDestroy( GtkWidget* w, gpointer data )
{
	gtk_main_quit();
}

//static
void PickWindow::HandleButtonClick( GtkWidget* w, gpointer data )
{
	PickWindow* pw = (PickWindow*)gtk_object_get_data(
		GTK_OBJECT(w), "ThisPtr" );
	Item* item = (Item*)gtk_object_get_data(
		GTK_OBJECT(w), "ItemPtr" );

	pw->mySelectionRetVal = item->ret;
}


void PickWindow::AddItem( std::string const& buttontext,
	std::string const& xpmfilename,
	std::string const& retval )
{
	myItems.push_back( Item() );
	myItems.back().text = buttontext;
	myItems.back().xpm = xpmfilename;
	myItems.back().ret = retval;
}



bool PickWindow::Go()
{
	myFenster = gtk_window_new( GTK_WINDOW_TOPLEVEL );
	gtk_window_set_title( GTK_WINDOW( myFenster ), myTitle.c_str() );

	gtk_signal_connect( GTK_OBJECT( myFenster ), "delete_event",
		GTK_SIGNAL_FUNC( HandleDeleteEvent ), NULL );

	gtk_signal_connect( GTK_OBJECT( myFenster ), "destroy",
		GTK_SIGNAL_FUNC( HandleDestroy ), NULL );


	myVBox = gtk_vbox_new( FALSE, 5 );
	gtk_container_set_border_width( GTK_CONTAINER(myVBox), 10 );
	gtk_container_add( GTK_CONTAINER(myFenster), myVBox );


	myLabelWidget = gtk_label_new( myLabelText.c_str() );
	gtk_box_pack_start( GTK_BOX( myVBox ), myLabelWidget, FALSE, TRUE, 0 );

	// realize the window because we are using it to init pixmaps (below)...
	gtk_widget_realize( myFenster );


	// add buttons

	int i;
	for( i=0; i<myItems.size(); ++i )
	{
		GtkWidget* button;
		GtkWidget* xpmbox;
		xpmbox = xpm_label_box( myFenster,
			myItems[i].xpm.c_str(),
			myItems[i].text.c_str() );
		// TODO: should check for xpm not found...

		button = gtk_button_new();
		gtk_container_add( GTK_CONTAINER( button), xpmbox );


		// Give button links back to PickWindow object and it's item.
		// (Should be OK because items list will be static while
		// window is running...)
		gtk_object_set_data( GTK_OBJECT(button), "ThisPtr", this );
		gtk_object_set_data( GTK_OBJECT(button), "ItemPtr", &myItems[i] );

		gtk_box_pack_start( GTK_BOX( myVBox ), button, FALSE, TRUE, 0 );

		gtk_signal_connect( GTK_OBJECT( button ),
			"clicked",
			GTK_SIGNAL_FUNC( HandleButtonClick ),
			NULL );
		gtk_signal_connect_object( GTK_OBJECT( button ),
			"clicked",
			GTK_SIGNAL_FUNC( gtk_widget_destroy ),
			GTK_OBJECT( myFenster));
	}

	gtk_widget_show_all( myFenster );

	gtk_main();

	if( mySelectionRetVal.empty() )
		return false;
	else
		return true;
}

// Create a new hbox with an image and a label packed into it
// and return the box.
// (snaffled from gtk tutorial)

GtkWidget* PickWindow::xpm_label_box( GtkWidget *parent,
	const gchar     *xpm_filename,
	const gchar     *label_text )
{
	GtkWidget *box1;
	GtkWidget *label;
	GtkWidget *pixmapwid;
	GdkPixmap *pixmap;
	GdkBitmap *mask;
	GtkStyle *style;

	/* Create box for xpm and label */
	box1 = gtk_hbox_new (FALSE, 0);
	gtk_container_set_border_width (GTK_CONTAINER (box1), 2);

	/* Get the style of the button to get the
	 * background color. */
	style = gtk_widget_get_style(parent);

	/* Now on to the xpm stuff */
	pixmap = gdk_pixmap_create_from_xpm (parent->window, &mask,
    	&style->bg[GTK_STATE_NORMAL],
		xpm_filename);
	pixmapwid = gtk_pixmap_new (pixmap, mask);

	/* Create a label for the button */
#ifdef _WIN32
	label = gtk_label_new (g_filename_to_utf8(label_text));
#else
	label = gtk_label_new (label_text);
#endif
	/* Pack the pixmap and label into the box */
	gtk_box_pack_start (GTK_BOX (box1),
		pixmapwid, FALSE, FALSE, 3);

	gtk_box_pack_start (GTK_BOX (box1), label, FALSE, FALSE, 3);

	gtk_widget_show(pixmapwid);
	gtk_widget_show(label);

	return(box1);
}



