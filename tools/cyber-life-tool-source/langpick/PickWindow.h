#ifndef PICKWINDOW_H
#define PICKWINDOW_H

#include <gtk/gtk.h>
#include <string>
#include <vector>


class PickWindow
{
public:
	void SetTitle( std::string const& title )
			{ myTitle=title; }
	void SetLabel( std::string const& label )
			{ myLabelText=label; }

	void AddItem( std::string const& buttontext,
		std::string const& xpmfilename,
		std::string const& retval );

	// display window and wait for a selection
	// returns false if no selection made.
	bool Go();

	// get the return val of selection after Go() has been called
	std::string GetSelected()
			{ return mySelectionRetVal; }

//	int WaitForSelection();

private:

	struct Item
	{
		std::string text;
		std::string xpm;
		std::string ret;
	};

	static gint HandleDeleteEvent( GtkWidget* w, GdkEvent* ev, gpointer data );
	static void HandleDestroy( GtkWidget* w, gpointer data );
	static void HandleButtonClick( GtkWidget* w, gpointer data );

	GtkWidget* myFenster;
	GtkWidget* myLabelWidget;
	GtkWidget* myVBox;

	GtkWidget *xpm_label_box( GtkWidget *parent,
		const gchar     *xpm_filename,
		const gchar     *label_text );

	std::string myLabelText;
	std::string myTitle;

	// retval for selection
	std::string mySelectionRetVal;

	std::vector< Item > myItems;

};

#endif // PICKWINDOW_H

