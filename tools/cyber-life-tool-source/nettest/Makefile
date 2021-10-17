SRCS = gtkhelp.cpp \
	nettest.cpp \
	user.cpp \
	userlist.cpp \
	userlistdropdown.cpp \
	../../../Babel/BabelCloak/DSNetManager.cpp \
	../../../Babel/BabelCloak/MessageThread.cpp \
	../../../Babel/BabelCloak/NetDebugListUsers.cpp \
	../../../Babel/BabelCloak/NetDirectLink.cpp \
	../../../Babel/BabelCloak/NetManager.cpp \
	../../../Babel/BabelCloak/NetMemoryPack.cpp \
	../../../Babel/BabelCloak/NetMemoryUnpack.cpp \
	../../../Babel/BabelCloak/NetUtilities.cpp \
	../../../Babel/BabelCloak/QueuedMessage.cpp \
	../../common/FileFuncs.cpp \
	../../common/GtkAdapter.cpp \
	../../common/SimpleMutex.cpp \
	../../../Babel/BabelClient/BabelClient.cpp \
	../../../Babel/BabelClient/BabelClientPeerLink.cpp \
	../../../Babel/BabelCommon/BabelAsyncCore.cpp \
	../../../Babel/BabelCommon/BabelCriticalSection.cpp \
	../../../Babel/BabelCommon/BabelDebugMemory.cpp \
	../../../Babel/BabelCommon/BabelRawSocket.cpp \
	../../../Babel/BabelCommon/BabelSystemSocket.cpp \
	../../../Babel/BabelCommon/BabelSystemThread.cpp \
	../../../Babel/BabelCommon/BabelSystemUnicode.cpp \
	../../../Babel/BabelCommon/BabelTrace.cpp \
	../../../Babel/BabelCommon/BabelUserDataObject.cpp \
	../../../Babel/BabelClient/BabelConnection.cpp \
	../../../Babel/BabelClient/BabelDConnection.cpp \
	../../../Babel/BabelClient/BabelRadioLink.cpp \
	../../../Babel/BabelClient/BabelRemoteSwitch.cpp \
	../../../Babel/BabelClient/BabelSocket.cpp \
	../../../Babel/BabelClient/BabelUDPConnection.cpp \
	../../../Babel/BabelClient/BabelVirtualSocket.cpp \
	../../../Babel/BabelClient/StdAfx.cpp

OBJS = $(patsubst %.cpp,%.nettesto,$(SRCS) )

CFLAGS = `gtk-config --cflags` -I../../../Babel/BabelCommon -I../../../Babel/BabelClient \
		-I../../../Babel/BabelCloak -I../../../Tech/Base \
		-DSSTREAM_INLINE -DPRODUCTCODE=2 -DCLIENTVERSION=1 -g
LIBS = `gtk-config --libs` -lpthread

nettest: $(OBJS)
	g++ $(CFLAGS) $(LIBS) -o nettest $(OBJS)

clean:
	rm -fr $(OBJS)

%.nettesto : %.cpp
	g++ $(CFLAGS) -o $@ -c $<

depend:
	makedepend -- $(CFLAGS) -- $(SRCS)

