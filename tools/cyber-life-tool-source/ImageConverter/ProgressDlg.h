#ifndef PROGRESSDLG_H
#define PROGRESSDLG_H

#include <gtk/gtk.h>
#include <string>


class ProgressDlg
{
public:
	ProgressDlg( std::string const& title,
		std::string const& label );
	~ProgressDlg();

	void SetText( std::string const& text );
	void SetProgress( float f );	// range 0..1
	void EventPump();

private:
	GtkWidget* myWindow;
	GtkWidget* myBar;
	GtkWidget* myVBox;
	GtkWidget* myLabel;
};

#endif // PROGRESSDLG_H

