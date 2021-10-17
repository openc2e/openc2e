// $Id: DSStress.h,v 1.4 2000/09/07 13:54:25 firving Exp $

#ifndef DS_STRESS_H
#define DS_STRESS_H

#include <DSNetManager.h>
#include <string>

#include "../../Server/HistoryFeed/BabelHistoryFeed.h"

class DSStress
{
public:
	DSStress(const std::string& username, const std::string& password);
	~DSStress();

	void OnlineOffline();
	void NewDynastyWorld(int initialNorns);
	void UploadNewHistory();

	std::string FetchRandomUser();

	int BytesReceivedEver();
	int BytesSentEver();

	DSNetManager& GetNet();

private:
	DSNetManager myNet;
	BabelHistoryFeed myFeed;

	std::string myUserName;

	int myPreviousBytesSent;
	int myPreviousBytesReceived;
};

#endif

