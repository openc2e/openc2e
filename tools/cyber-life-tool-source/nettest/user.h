// Test harness for Creatures Engine networking layer,
// on top of Babel.

// user.h - Window specific to one user, letting you log in and out,
// and do lots of things with them.

#ifndef USER_H
#define USER_H

#include <DSNetManager.h>
#include "../../common/GtkAdapter.h"

class UserWindow : GtkAdapter<UserWindow>
{
public:
	static void GenerateWindow(const std::string& user);
	static void GenerateWindow(const std::string& userName, const std::string& password);

private:
	UserWindow(const std::string& user);
	UserWindow(const std::string& userName, const std::string& password);
	void BuildDialog(const std::string& title);
	~UserWindow();

	void Destroy();
	void Connect();
	void ConnectToggle();
	void Disconnect();
	void SendMessage();
	void RandomUser();

	static gint Timer(gpointer data);

	DSNetManager myNetManager;
	GtkWidget* myCombo;
	GtkWidget* myConnectToggle;
	guint myTimerID;
};

#endif

