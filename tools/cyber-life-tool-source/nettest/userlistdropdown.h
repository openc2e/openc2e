// Test harness for Creatures Engine networking layer,
// on top of Babel.

// userlistdropdown.h - combobox of users

#ifndef USER_LIST_DROPDOWN_H
#define USER_LIST_DROPDOWN_H

#ifdef _WIN32
	#include <windows.h>
#endif
#include <NetDebugListUsers.h>

class UserListDropDown
{
public:
	static GtkWidget* GenerateCombo();
	static std::string GetText(GtkWidget* widget);

private:
	UserListDropDown(GtkCombo *combo);

	static void Destroy(GtkWidget *widget, gpointer data);
	static void Refresh(GtkCList *clist, gpointer data);

	void FillInUsers();

	GtkCombo* myCombo;
	NetDebugListUsers myNetListUsers;
};

#endif

