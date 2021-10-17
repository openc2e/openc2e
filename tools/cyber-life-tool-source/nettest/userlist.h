// Test harness for Creatures Engine networking layer,
// on top of Babel.

// userlist.h - provide a list of all the users on the server

#ifndef USER_LIST_H
#define USER_LIST_H

#ifdef _WIN32
	#include <windows.h>
#endif
#include <NetDebugListUsers.h>

class UserListWindow
{
public:
	static void GenerateWindow();

private:
	UserListWindow(GtkCList *clist);

	static void Destroy(GtkWidget *widget, gpointer data);
	static void ClickColumn(GtkCList *clist, gint column);
	static void Refresh(GtkCList *clist, gpointer data);
	static void ClickList(GtkWidget* widget, GdkEventButton* button, gpointer data);

	void FillInUsers();

	GtkCList* myList;
	NetDebugListUsers myNetListUsers;
};

#endif

