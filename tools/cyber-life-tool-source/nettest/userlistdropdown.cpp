#include <gtk/gtk.h>
#include "userlistdropdown.h"
#include <iostream>
#include <sstream>

#include <BabelClient.h>
#include <DSNetManager.h>
#include <NetUtilities.h>

void UserListDropDown::Destroy(GtkWidget *widget, gpointer data)
{
    delete (UserListDropDown*)data;
}

void UserListDropDown::Refresh(GtkCList *clist, gpointer data)
{
	((UserListDropDown*)data)->FillInUsers();
}

UserListDropDown::UserListDropDown(GtkCombo *combo)
{
	myCombo = combo;
}

GtkWidget* UserListDropDown::GenerateCombo()
{
	// The drop down list
	GtkWidget* combo = gtk_combo_new();

	// Make the associated C++ class, and connect it to the window's destructor
	UserListDropDown* userListDropDown = new UserListDropDown(GTK_COMBO(combo));
	gtk_signal_connect(GTK_OBJECT(combo), "destroy", GTK_SIGNAL_FUNC(UserListDropDown::Destroy), userListDropDown);

	// Put initial data in
	userListDropDown->FillInUsers();
	gtk_widget_show(combo);
   
	return combo;
}

void UserListDropDown::FillInUsers()
{
	// Have to log in as a user to get online info
	DSNetManager net;

	// Have to log in as a user to get online info
	// aeeeoouuuqwwww is the Warp Monster (29+4)
	net.SetUser("aeeeoouuuqwwww", "wibble");
	if (!net.Connect())
	{
		std::cout << "Failed to connect as aeeeoouuuqwwww 29+4 Warp Monster" << std::endl;
		return;
	}

	std::vector<CBabelShortUserDataObject> users;
	NetDebugListUsers::ErrorCode error = myNetListUsers.GetUserDump(users);

	if (error == NetDebugListUsers::SUCCESS)
	{
		std::cout << "Success, " << users.size() << " users";
		for (int i = 0; i < users.size(); ++i)
		{
			std::string uid, hid;
			{
				std::ostringstream uidStream, hidStream;
				uidStream << users[i].m_UIN.UID;
				hidStream << users[i].m_UIN.HID;
				uid = uidStream.str();
				hid = hidStream.str();
			}
			std::string labelText;
			labelText += users[i].FirstName() + std::string(" ");
			labelText += users[i].LastName() + std::string(" (");
			labelText += users[i].NickName() + std::string(") ");
			std::string uinText;
			uinText = uid.c_str() + std::string("+") + hid.c_str();
			labelText += uinText;

			if (net.IsUserOnline(NetUtilities::UserToString(users[i].m_UIN)))
				labelText = "* " + labelText;
			else 
				labelText = "  " + labelText;

			GtkWidget* item = gtk_list_item_new();
			gtk_widget_show (item);
			GtkWidget* label = gtk_label_new(labelText.c_str());
			gtk_container_add (GTK_CONTAINER (item), label);
			gtk_widget_show(label);
			gtk_combo_set_item_string (GTK_COMBO (myCombo), GTK_ITEM (item), uinText.c_str());
			gtk_container_add (GTK_CONTAINER (GTK_COMBO (myCombo)->list), item);

		}	
	}
	else if (error == NetDebugListUsers::CONNECTION_FAILED)
		std::cout << "Connection to server failed";
	else if (error == NetDebugListUsers:: SENDING_DATA_FAILED)
		std::cout << "Sending data failed";
	else if (error == NetDebugListUsers:: RECEIVING_DATA_FAILED)
		std::cout << "Receiving data failed";
	else
		std::cout << "Unknown error";
	std::cout << std::endl;
}

std::string UserListDropDown::GetText(GtkWidget* widget)
{
	gchar* ch = gtk_entry_get_text(GTK_ENTRY(GTK_COMBO(widget)->entry));
	return std::string(ch);
}

