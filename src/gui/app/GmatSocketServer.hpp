/*
 * GmatSocketServer.h
 *
 *  Created on: Mar 22, 2011
 *      Author: Tuan Nguyen
 */

#ifndef GmatSocketServer_hpp
#define GmatSocketServer_hpp

#include <wx/event.h>

#ifdef LINUX_MAC
	#include <unistd.h>
	#include <pthread.h>

	#include <sys/time.h>
	#include <sys/select.h>
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
	#include <netdb.h>

	#define SOCKET_ERROR	-1
	#define INVALID_SOCKET	-1

#else
	#include <winsock2.h>
	#include <process.h>
#endif


#define IP_ADDRESS	"localhost"
//#define IP_ADDRESS	"128.183.221.98"
#define TCP_PORT	3000


class GmatSocketServer
{
public:
	GmatSocketServer(wxEvtHandler* handler);
	virtual ~GmatSocketServer();

	void Close();
	void SetEventHandler(wxEvtHandler* handler) {evthandler = handler;}

#ifdef LINUX_MAC
	bool RunRequest(int sock);
#else
	bool RunRequest(SOCKET sock);
#endif
	char* OnRequest(char* item);
	bool OnPoke(char* data);

	void RunServer();
#ifdef LINUX_MAC
	void OnAccept(int sock);
	static void* StaticOnAccept(void* objPtr)
	{
		GmatSocketServer* pThis = (GmatSocketServer*)objPtr;
		pThis->OnAccept(pThis->client_sock);
		return NULL;
	}
	static void* StaticRunServer(void* objPtr)
	{
		GmatSocketServer* pThis = (GmatSocketServer*)objPtr;
		pThis->RunServer();
      		return NULL;
	}
#else
	void OnAccept(SOCKET sock);
	static void StaticOnAccept(void* objPtr)
	{
		GmatSocketServer* pThis = (GmatSocketServer*)objPtr;
		pThis->OnAccept(pThis->client_sock);
	}
	static void StaticRunServer(void* objPtr)
	{
		GmatSocketServer* pThis = (GmatSocketServer*)objPtr;
		pThis->RunServer();
	}
#endif
private:
	int m_numClients;
	int error;
	bool shutdownserver;

	wxEvtHandler* evthandler;

#ifdef LINUX_MAC
        int Server;
	int client_sock;
#else
	SOCKET client_sock;
        SOCKET Server;
#endif
};

enum
{
	ID_SOCKET_POKE = 10000,
	ID_SOCKET_REQUEST,
};
#endif /* GmatSocketServer_hpp */
