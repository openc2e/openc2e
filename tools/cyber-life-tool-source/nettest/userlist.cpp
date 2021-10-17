#include <gtk/gtk.h>
#include "userlist.h"
#include <iostream>
#include <sstream>

#include <BabelClient.h>
#include <DSNetManager.h>
#include <NetUtilities.h>

#include "user.h"

void UserListWindow::Destroy(GtkWidget *widget, gpointer data)
{
    delete (UserListWindow*)gtk_object_get_data(GTK_OBJECT(widget), "associatedClass");
}

void UserListWindow::ClickColumn(GtkCList *clist, gint column)
{
	if(column == clist->sort_column) 
	{ 
		if(clist->sort_type == GTK_SORT_ASCENDING)
			gtk_clist_set_sort_type(clist, GTK_SORT_DESCENDING);
		else 
			gtk_clist_set_sort_type(clist, GTK_SORT_ASCENDING);
	} 
	else 
	{
		gtk_clist_set_sort_type(clist, GTK_SORT_ASCENDING);
		gtk_clist_set_sort_column(clist, column);
	}

	gtk_clist_sort(GTK_CLIST(clist)); 
}

void UserListWindow::ClickList(GtkWidget* widget, GdkEventButton* button, gpointer data)
{
	if (button->button == 1 && button->type == GDK_2BUTTON_PRESS)
	{
		gint row, column;
		if (gtk_clist_get_selection_info(GTK_CLIST(widget), button->x, button->y, &row, &column) == 1)
		{
			gchar* text;
			gtk_clist_get_text(GTK_CLIST(widget), row, 3, &text);
			std::string user = text;
			user += "+";
			gtk_clist_get_text(GTK_CLIST(widget), row, 4, &text);
			user += text;

			UserWindow::GenerateWindow(user);
		}
	}

}

void UserListWindow::Refresh(GtkCList *clist, gpointer data)
{
	((UserListWindow*)data)->FillInUsers();
}

UserListWindow::UserListWindow(GtkCList *clist)
{
	myList = clist;
}

void UserListWindow::GenerateWindow()
{
	GtkWidget* window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window), "User List");

	// Outer box to hold the list and button
	GtkWidget* box = gtk_vbox_new(FALSE, 0);
	gtk_container_add(GTK_CONTAINER(window), box);
	gtk_widget_show(box);

	// The columned list
	gchar *titles[5] = { "First Name", "Last Name", "Nick Name", "UID", "HID" };
	GtkWidget* clist = gtk_clist_new_with_titles(5, titles);
	gtk_clist_set_selection_mode(GTK_CLIST(clist), GTK_SELECTION_BROWSE);
	gtk_signal_connect(GTK_OBJECT(clist),"click_column", GTK_SIGNAL_FUNC(UserListWindow::ClickColumn), NULL);
	gtk_signal_connect(GTK_OBJECT(clist),"button_press_event", GTK_SIGNAL_FUNC(UserListWindow::ClickList), NULL);

	gtk_box_pack_start(GTK_BOX(box), clist, TRUE, TRUE, 0);
	gtk_widget_show(clist);

	// Make the associated C++ class, and connect it to the window's destructor
	UserListWindow* userListWindow = new UserListWindow(GTK_CLIST(clist));
	gtk_object_set_data(GTK_OBJECT(window), "associatedClass", userListWindow);
	gtk_signal_connect(GTK_OBJECT(window), "destroy", GTK_SIGNAL_FUNC(UserListWindow::Destroy), NULL);

	// Refresh button
	GtkWidget* refreshButton = gtk_button_new_with_label("Refresh");
	gtk_box_pack_start(GTK_BOX(box), refreshButton, TRUE, TRUE, 0);
	gtk_signal_connect(GTK_OBJECT(refreshButton), "clicked", GTK_SIGNAL_FUNC(UserListWindow::Refresh), (gpointer)userListWindow);
	gtk_widget_show(refreshButton);

	// Put initial data in
	userListWindow->FillInUsers();

	// Show the window
	gtk_widget_show(window);
}

void UserListWindow::FillInUsers()
{
	gtk_clist_freeze(myList);
	gtk_clist_clear(myList);

	// Have to log in as a user to get online info
	// aeeeoouuuqwwww is the Warp Monster (29+4)
	DSNetManager net;
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
			gchar* text[5];
			text[0] = const_cast<char*>(users[i].FirstName());
			text[1] = const_cast<char*>(users[i].LastName());
			text[2] = const_cast<char*>(users[i].NickName());
			std::string uid, hid;
			{
				std::ostringstream uidStream, hidStream;
				uidStream << users[i].m_UIN.UID;
				hidStream << users[i].m_UIN.HID;
				uid = uidStream.str();
				hid = hidStream.str();
			}
			text[3] = const_cast<char*>(uid.c_str());
			text[4] = const_cast<char*>(hid.c_str());


			gint newRow = gtk_clist_append(myList, text);
			if (net.IsUserOnline(NetUtilities::UserToString(users[i].m_UIN)))
			{
				GdkColor color;
				color.red = 0;
				color.green = 43000;
				color.blue = 0;
				color.pixel = 0;
				gtk_clist_set_background(myList, newRow, &color);
				GdkColor white;
				white.red = 65535;
				white.green = 65535;
				white.blue = 65535;
				white.pixel = 0;
				gtk_clist_set_foreground(myList, newRow, &white);
			}
		}	

		gtk_clist_sort(myList);
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

	gtk_clist_thaw(myList);
	gtk_clist_columns_autosize(myList);
}

