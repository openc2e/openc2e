// nettest.cpp is the main file

#ifdef _MSC_VER
#pragma warning(disable:4786 4503)
#endif

#include <iostream>

#ifdef _WIN32
	#include <windows.h>
#endif
#include <gtk/gtk.h>
#include <sstream>

// These are in the project settings - they're here as
// well to
#define PRODUCTCODE 2 // BABEL_PRODUCT_CREATURES_DOCKING_STATION
#define CLIENTVERSION 1

#include <BabelClient.h>
#include <DSNetManager.h>
#include <NetUtilities.h>
#include <NetDebugListUsers.h>

#include "nettest.h"
#include "userlist.h"
#include "user.h"

#include <stdlib.h>

int TestOther()
{
	// Login
	DSNetManager net;
	net.SetOutboxDirectory("outbox\\");
	net.SetInboxDirectory("inbox\\");

	std::cout << "Setting user...";
	net.SetUser("stilton", "wobble");
	std::cout << "connecting...";
	net.Connect();
	std::cout << "done" << std::endl;

	// Getting user data
	// TODO: Make compatible with Block
	std::string userGetData = "11+1";
	std::cout << "Getting user data for " << userGetData << "..." << std::endl;
	std::string nickName;
	bool block = true;
	while (block)
		net.FetchUserData(userGetData, nickName, block);
	std::cout << "Nick name: " << nickName;
	std::cout << std::endl;

	// User online
	bool online = net.IsUserOnline("3+1");
	if (online)
		std::cout << "User 3+1 is online" << std::endl;
	else
		std::cout << "User 3+1 is not online" << std::endl;

	// Check bogus name doesn't crash Babel
	bool onlineNull = net.IsUserOnline("");
	if (onlineNull)
		std::cout << "User null is online" << std::endl;
	else
		std::cout << "User null is not online" << std::endl;

// std::cout << "Falling into infinite loop" << std::endl;
//	while (true)
//		Sleep(10);

	std::cout << "disconnecting...";
	net.Disconnect();
	std::cout << "done" << std::endl;

	std::cout << std::endl;
	std::cout << std::endl;

	return 0;
}

////////////////////////////////////////////////

void MainWindowDestroy(GtkWidget *widget, gpointer data)
{
    gtk_main_quit();
}

void MainWindowUserList(GtkWidget *widget, gpointer data)
{
	UserListWindow::GenerateWindow();
}

void StressfulLogin(GtkWidget *widget, gpointer data)
{
	int n = 8;
	std::vector<DSNetManager*> nets;
	for (int i = 0; i < n; ++i)
	{
		nets.push_back(new DSNetManager);
	}
	nets[0]->SetUser("elmo", "wibble");
	nets[1]->SetUser("snowmain", "wibble");
	nets[2]->SetUser("frog", "wibble");
	nets[3]->SetUser("paradise", "wibble");
	nets[4]->SetUser("dot", "wibble");
	nets[5]->SetUser("kiss", "wibble");
	nets[6]->SetUser("smile", "wibble");
	nets[7]->SetUser("hairslide", "wibble");

	while(true)
	{
		int i = rand() % n;
		if (nets[i]->Online())
		{
			std::cout << "Disconnecting as " << i << std::endl;
			nets[i]->Disconnect();
			if (nets[i]->Online())
				std::cerr << "  Failed to disconnect " << i << std::endl;
		}
		else
		{
			std::cout << "Connecting as " << i << std::endl;
			if (!nets[i]->Connect())
				std::cerr << "  Failed to connect " << i << std::endl;
		}
	}

	// delete NetManagers ;-)
}

GtkWidget *ourUsername, *ourPassword;
GtkWidget *ourHost, *ourPort;

void MainWindowLogin(GtkWidget *widget, gpointer data)
{
	std::string username = gtk_entry_get_text(GTK_ENTRY(ourUsername));
	std::string password = gtk_entry_get_text(GTK_ENTRY(ourPassword));
	UserWindow::GenerateWindow(username, password);
}

void ChangeHost(GtkWidget *widget, gpointer data)
{
	std::string host = gtk_entry_get_text(GTK_ENTRY(ourHost));
	std::string port = gtk_entry_get_text(GTK_ENTRY(ourPort));
	DSNetManager::OverrideHost(host.c_str(), atoi(port.c_str()));
}

void GenerateMainWindow()
{
	GtkWidget* window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window), "Net Test - Babel/Creatures Engine");
	gtk_signal_connect(GTK_OBJECT(window), "destroy", GTK_SIGNAL_FUNC(MainWindowDestroy), NULL);

	GtkWidget* box = gtk_vbox_new(FALSE, 0);
	gtk_container_add(GTK_CONTAINER(window), box);

	// User / password

	GtkWidget* usernameBox = gtk_hbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(box), usernameBox, TRUE, TRUE, 0);

		GtkWidget* usernameLabel = gtk_label_new("Username:");
		gtk_box_pack_start(GTK_BOX(usernameBox), usernameLabel, TRUE, TRUE, 0);

		ourUsername = gtk_entry_new();
		gtk_box_pack_start(GTK_BOX(usernameBox), ourUsername, TRUE, TRUE, 0);

	GtkWidget* passwordBox = gtk_hbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(box), passwordBox, TRUE, TRUE, 0);

		GtkWidget* passwordLabel = gtk_label_new("Password:");
		gtk_box_pack_start(GTK_BOX(passwordBox), passwordLabel, TRUE, TRUE, 0);

		ourPassword = gtk_entry_new();
		gtk_box_pack_start(GTK_BOX(passwordBox), ourPassword, TRUE, TRUE, 0);
		gtk_entry_set_visibility (GTK_ENTRY(ourPassword), FALSE);


	GtkWidget* registration = gtk_button_new_with_label("Open User Window");
	gtk_signal_connect(GTK_OBJECT(registration), "clicked", GTK_SIGNAL_FUNC(MainWindowLogin), NULL);
	gtk_box_pack_start(GTK_BOX(box), registration, TRUE, TRUE, 0);

	GtkWidget* users = gtk_button_new_with_label("New User List Window");
	gtk_signal_connect(GTK_OBJECT(users), "clicked", GTK_SIGNAL_FUNC(MainWindowUserList), NULL);
	gtk_box_pack_start(GTK_BOX(box), users, TRUE, TRUE, 0);

	GtkWidget* stressful_login = gtk_button_new_with_label("Stressful Login");
	gtk_signal_connect(GTK_OBJECT(stressful_login), "clicked", GTK_SIGNAL_FUNC(StressfulLogin), NULL);
	gtk_box_pack_start(GTK_BOX(box), stressful_login, TRUE, TRUE, 0);

	// Change host/port
	GtkWidget* hostBox = gtk_hbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(box), hostBox, TRUE, TRUE, 0);

		GtkWidget* hostLabel = gtk_label_new("Host:");
		gtk_box_pack_start(GTK_BOX(hostBox), hostLabel, TRUE, TRUE, 0);

		ourHost = gtk_entry_new();
		gtk_box_pack_start(GTK_BOX(hostBox), ourHost, TRUE, TRUE, 0);

		GtkWidget* portLabel = gtk_label_new("Port:");
		gtk_box_pack_start(GTK_BOX(hostBox), portLabel, TRUE, TRUE, 0);

		ourPort = gtk_entry_new();
		gtk_box_pack_start(GTK_BOX(hostBox), ourPort, TRUE, TRUE, 0);

		GtkWidget* change_host = gtk_button_new_with_label("Change Host/Port");
		gtk_signal_connect(GTK_OBJECT(change_host), "clicked", GTK_SIGNAL_FUNC(ChangeHost), NULL);
		gtk_box_pack_start(GTK_BOX(box), change_host, TRUE, TRUE, 0);

	gtk_widget_show_all(window);        
}

////////////////////////////////////////////////

void MakeLotsUsers()
{
	std::vector<DSNetManager*> foo;
	int num = 11;
	for (int i = 0; i < num; ++i)
	{
		DSNetManager* newnm = new DSNetManager;
		foo.push_back(newnm);
		newnm->SetUser("3+1");
		if (!newnm->Connect())
			ASSERT(false);
	}

	ASSERT(foo.size() == num);

	{
		for (int i = 0; i < num; ++i)
		{
			delete foo[i];
		}
	}
}

// Float from [0-1) (i.e. including 0, excluding 1)
float RandFloat()
{
	return (float)rand() / ((float)RAND_MAX + 1);
}

void StressConnectDisconnecter()
{
	const int noManagers = 10;

	std::vector<DSNetManager*> net;
	for (int i = 0; i < noManagers; ++i)
		net.push_back(new DSNetManager);

	NetDebugListUsers myNetListUsers;
	std::vector<CBabelShortUserDataObject> users;
	NetDebugListUsers::ErrorCode error;
	while(true)
	{
		int x = RandFloat() * noManagers;
		if (x < 0 || x >= noManagers)
		{
			ASSERT(false);
			continue;
		}

		std::cout << "Machine " << x;

		if (net[x]->Online())
		{
			std::cout << " disconnect...";
			net[x]->Disconnect();
			std::cout << "done" << std::endl;
		}
		else
		{
			std::cout << " user...";
			net[x]->SetUser("3+1");
			std::cout << " connect...";
			net[x]->Connect();
			std::cout << "done" << std::endl;
		}

		std::cout << "Get user dump...";
		error = myNetListUsers.GetUserDump(users);
		std::cout << "done" << std::endl;
	}
}

void DoSomethingSimpleWithAModule()
{
	DSNetManager net;
	std::cout << "Setting user...";
	std::string user = "2+1";
	net.SetUser(user);
	std::cout << "connecting...";
	if (!net.Connect())
	{
		std::cout << "failed" << std::endl;
		return;
	}
	std::cout << "done" << std::endl;

	std::cout << "Trying to use the module...";
	int result = net.DSMultiplyBySeventeen();
	std::cout << "Got back " << result << std::endl;
}

int main(int argc, char *argv[])
{	
	std::cout << "Creatures Engine Babel network tester" << std::endl;
//	DSNetManager::OverrideHost("ctlcpc179", 1966);

//	DoSomethingSimpleWithAModule();
//	StressConnectDisconnecter()
//	TestOther();
//	return 0;

	gtk_init(&argc, &argv);
	GenerateMainWindow();
//	UserListWindow::GenerateWindow();
//	UserWindow::GenerateWindow(std::string("20+1"));
	gtk_main();

	return 0;
}

