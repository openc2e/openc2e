// $Id: DSStress.cpp,v 1.9 2001/04/24 15:01:36 firving Exp $

#include "DSStress.h"
#include "NornDynasty.h"
#include <iostream>

// TODO: Make netloginterface that writes to console
DSStress::DSStress(const std::string& username, const std::string& password)
	: myUserName(username),
	  myPreviousBytesSent(0), myPreviousBytesReceived(0)
{
	myNet.SetUser(myUserName, password);
}

DSStress::~DSStress()
{
}

void DSStress::OnlineOffline()
{
	if (myNet.Online())
	{
		// Update our cumulative byte counts
		int timeOnline, usersOnline, bytesReceived, bytesSent;
		bool block = true;
		while (block)
		{
			myNet.GetStatus(timeOnline, usersOnline, bytesReceived, bytesSent, block);
			Sleep(1);
		}
		if (bytesReceived > 0)
			myPreviousBytesReceived += bytesReceived;
		else
			std::cerr << "  Failed to get received bytes while online " << bytesReceived << std::endl;
		if (bytesSent > 0)
			myPreviousBytesSent += bytesSent;
		else
			std::cerr << "  Failed to get bytesSent bytes while online " << bytesSent << std::endl;

		// Actually break connection
		myNet.Disconnect();
		if (myNet.Online())
			std::cerr << "  Failed to disconnect " << myUserName << std::endl;
	}
	else
	{
		if (!myNet.Connect())
		{
			std::string message;
			NetManager::ErrorCode err = myNet.GetLastError();
			switch(err)
			{
			case NetManager::NME_UNKNOWN:
				message = "Unknown";
				break;
			case NetManager::NME_CONNECT_OKAY:
				message = "OK";
				break;
			case NetManager::NME_CONNECT_FAIL_SERVER_NOT_THERE:
				message = "Server not there";
				break;
			case NetManager::NME_CONNECT_FAIL_INVALID_USER_OR_PASSWORD:
				message = "Invalid user or password";
				break;
			case NetManager::NME_CONNECT_FAIL_LOGGED_IN_ELSEWHERE:
				message = "Logged in elsewhere alrady";
				break;
			case NetManager::NME_CONNECT_FAIL_TOO_MANY_USERS:
				message = "Too many users";
				break;
			case NetManager::NME_CONNECT_FAIL_INTERNAL_ERROR:
				message = "Internal error";
				break;
			default:
				message = "Doubly unknown!!!";
				break;
			}
			std::cerr << "  Failed to connect " << myUserName
				<< " Reason: " << message << std::endl;
		}
	}
}

void DSStress::NewDynastyWorld(int initialNorns)
{
	// Go online
	if (!myNet.Online())
		OnlineOffline();

	if (!myNet.Online())
	{
		// TODO: Throw exception
		return;
	}

	if (!NornDynasty::NewWorld(&myFeed, initialNorns, myNet.GetUser()))
	{
		// TODO: Throw exception
	}
}

void DSStress::UploadNewHistory()
{
	if (myNet.Online())
	{
		myFeed.SendEverythingOut(myNet);
	}
}


std::string DSStress::FetchRandomUser()
{
	bool block = true;
	std::string user;
	while (block)
	{
		user = myNet.DSFetchRandomUser(block);
		Sleep(1);
	}
	return user;
}


int DSStress::BytesReceivedEver()
{
	int timeOnline, usersOnline, bytesReceived, bytesSent;
	bool block = true;
	while (block)
	{
		myNet.GetStatus(timeOnline, usersOnline, bytesReceived, bytesSent, block);
		Sleep(1);
	}
	return myPreviousBytesReceived + (bytesReceived >= 0 ? bytesReceived : 0);
}

int DSStress::BytesSentEver()
{
	int timeOnline, usersOnline, bytesReceived, bytesSent;
	bool block = true;
	while (block)
	{
		myNet.GetStatus(timeOnline, usersOnline, bytesReceived, bytesSent, block);
		Sleep(1);
	}
	return myPreviousBytesSent + (bytesSent >= 0 ? bytesSent : 0);
}

DSNetManager& DSStress::GetNet()
{
	return myNet;
}

