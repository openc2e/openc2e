
#include "ProgressDlg.h"

static gint delete_event( GtkWidget* w, GdkEvent* ev, gpointer data )
{
	return TRUE;	// keep the window open
}

ProgressDlg::ProgressDlg( std::string const& title,
	std::string const& label )
{
//	gtk_init( &argc, &argv );

	myWindow = gtk_window_new( GTK_WINDOW_TOPLEVEL );
	gtk_window_set_title( GTK_WINDOW( myWindow ), title.c_str() );

	// disable the close button (ugh :-)
	gtk_signal_connect( GTK_OBJECT( myWindow ), "delete_event",
		GTK_SIGNAL_FUNC( delete_event ), NULL );

	myVBox = gtk_vbox_new( FALSE, 5 );

	gtk_container_set_border_width( GTK_CONTAINER(myVBox), 10 );
	gtk_container_add( GTK_CONTAINER(myWindow), myVBox );

	myBar = gtk_progress_bar_new();
	gtk_box_pack_start( GTK_BOX(myVBox), myBar, FALSE, TRUE, 0 );


	myLabel = gtk_label_new( label.c_str() );
	gtk_box_pack_start( GTK_BOX(myVBox),myLabel, FALSE,TRUE, 0 );


	gtk_widget_show_all( myWindow );
}


ProgressDlg::~ProgressDlg()
{
}

void ProgressDlg::SetProgress( float f )
{
	gtk_progress_bar_update( GTK_PROGRESS_BAR( myBar ), (gfloat)f );
}

void ProgressDlg::SetText( std::string const& text )
{
	gtk_label_set( GTK_LABEL( myLabel ), text.c_str() );
}


void ProgressDlg::EventPump()
{
	while( gtk_events_pending() )
		gtk_main_iteration();
}

