#include <gtk/gtk.h>
#include "user.h"
#include <iostream>
#include <sstream>

#ifdef _WIN32
	#include <windows.h>
#endif
#include <BabelClient.h>
#include <DSNetManager.h>
#include <NetUtilities.h>
#include <NetMemoryPack.h>

#include "userlistdropdown.h"

void UserWindow::Destroy()
{
    delete this;
}

void UserWindow::ConnectToggle()
{
	if (GTK_TOGGLE_BUTTON(myConnectToggle)->active) 
		Connect();
	else
		Disconnect();
}

void UserWindow::Connect()
{
	if (myNetManager.Connect())
		std::cout << "Connected " << myNetManager.GetUser();
	else
		std::cout << "Failed to connect " << myNetManager.GetUser();
	std::cout << std::endl;
}

void UserWindow::Disconnect()
{
	myNetManager.Disconnect();
	std::cout << "Disconnected " << myNetManager.GetUser() << std::endl;
}

void UserWindow::SendMessage()
{
	std::string user = UserListDropDown::GetText(myCombo);
	std::cout << "Sending direct signal to user " << user << std::endl;

	// TODO: Widgetise
	std::string text = "Flooble, said someone";

	NetMessages::BinaryMessage messageHeader;
	messageHeader.length = sizeof(messageHeader);
	messageHeader.type = NetMessages::TEXT_TEST;
	messageHeader.reserved = 0;

	int totalSize = sizeof(messageHeader) + text.size();
	NetMemoryPack pack(totalSize);

	pack.PackIn(&messageHeader, sizeof(messageHeader));
	pack.PackIn(&text[0], text.size());

	bool result = myNetManager.PostDirectMessage(user, (char*)pack.GetAddress(), totalSize);
	if (result)
		std::cout << "Managed to send direct signal" << std::endl;
	else
		std::cout << "Failed to send direct signal" << std::endl;
}

gint UserWindow::Timer(gpointer data)
{
	DSNetManager& net = ((UserWindow*)data)->myNetManager;

	// Set online state
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(((UserWindow*)data)->myConnectToggle), net.Online() ? TRUE : FALSE);

	// Poll for messages
	std::vector<std::string> users;
	std::vector<NetMessages::MessageType> types;
	std::vector<std::string> messages;
	net.GetMessages(users, types, messages);
	ASSERT(users.size() == messages.size());
	if (!users.empty())
	{
		std::cout << "Messages arrived:" << std::endl;
		for (int i = 0; i < users.size(); ++i)
		{
			if (types[i] == NetMessages::TEXT_TEST)
				std::cout << "TEXT TEST " << messages[i];
			else if (types[i] == NetMessages::MESG_WRIT)
				std::cout << "NET WRIT";
			else
				std::cout << "UNKNOWN " << (int)types[i];

			std::cout << " User: " << users[i] << std::endl;
		}
	}

	// Send ordinary messages
	net.SendOrdinaryMessages();

	return TRUE;
}

void UserWindow::GenerateWindow(const std::string& user)
{
	UserWindow* userWindow = new UserWindow(user);
}

void UserWindow::GenerateWindow(const std::string& userName, const std::string& password)
{
	UserWindow* userWindow = new UserWindow(userName, password);
}

UserWindow::~UserWindow()
{
	gtk_timeout_remove(myTimerID);
}

UserWindow::UserWindow(const std::string& userName, const std::string& password)
{
	myNetManager.SetUser(userName, password);
	BuildDialog(userName);
}

UserWindow::UserWindow(const std::string& user)
{
	myNetManager.SetUser(user);
	BuildDialog(user);
}

void UserWindow::BuildDialog(const std::string& title)
{
	GtkWidget* window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window), title.c_str());

	// Outer box to hold things
	GtkWidget* box = gtk_vbox_new(FALSE, 0);
	gtk_container_add(GTK_CONTAINER(window), box);

	// Connect to the window's destructor
	gtk_signal_connect(GTK_OBJECT(window), "destroy", GtkSignal(), GtkAdapt(&UserWindow::Destroy));

	// Connection buttons box
	GtkWidget* connectBox = gtk_hbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(box), connectBox, TRUE, TRUE, 0);

		// Connect toggle button
		myConnectToggle = gtk_toggle_button_new_with_label("Connect");
		gtk_box_pack_start(GTK_BOX(connectBox), myConnectToggle, TRUE, TRUE, 0);
		gtk_signal_connect(GTK_OBJECT(myConnectToggle), "toggled", GtkSignal(), GtkAdapt(&UserWindow::ConnectToggle));

	// Other user things
	GtkWidget* userlistBox = gtk_vbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(box), userlistBox, TRUE, TRUE, 0);

		// Combo
		GtkWidget* userlistCombo = UserListDropDown::GenerateCombo();
		gtk_box_pack_start(GTK_BOX(userlistBox), userlistCombo, TRUE, TRUE, 0);
		myCombo = userlistCombo;

		// Send button
		GtkWidget* sendButton = gtk_button_new_with_label("Send");
		gtk_box_pack_start(GTK_BOX(userlistBox), sendButton, TRUE, TRUE, 0);
		gtk_signal_connect(GTK_OBJECT(sendButton), "clicked", GtkSignal(), GtkAdapt(&UserWindow::SendMessage));

	// Disconnect button
	GtkWidget* randomUserButton = gtk_button_new_with_label("Random User");
	gtk_box_pack_start(GTK_BOX(box), randomUserButton, TRUE, TRUE, 0);
	gtk_signal_connect(GTK_OBJECT(randomUserButton), "clicked", GtkSignal(), GtkAdapt(&UserWindow::RandomUser));

	// Timer to poll net things
	myTimerID = gtk_timeout_add(1000, &Timer, (gpointer)this);

	// Show the window
	gtk_widget_show_all(window);
}

void UserWindow::RandomUser()
{
	// TODO: Make compatible with block
/*
	std::cout << "Finding a random online ... ";

	std::string randomUser = myNetManager.DSFetchRandomUser();
	if (randomUser.empty())
		std::cout << "none";
	else
		std::cout << randomUser;

	std::cout << std::endl;
*/
}

