/*
 * SocketServer.hpp
 *
 *  Created on: Mar 4, 2011
 *      Author: Tuan Nguyen
 */

#ifndef SocketServer_hpp
#define SocketServer_hpp

#ifndef _WIN32
#define LINUX_MAC
#endif


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
#define TCP_PORT	3000


class SocketServer
{
public:
	SocketServer();
	virtual ~SocketServer();

	void Close();

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
		SocketServer* pThis = (SocketServer*)objPtr;
		pThis->OnAccept(pThis->client_sock);
		return NULL;
	}
	static void* StaticRunServer(void* objPtr)
	{
		SocketServer* pThis = (SocketServer*)objPtr;
		pThis->RunServer();
      return NULL;
	}
#else
	void OnAccept(SOCKET sock);
	static void StaticOnAccept(void* objPtr)
	{
		SocketServer* pThis = (SocketServer*)objPtr;
		pThis->OnAccept(pThis->client_sock);
	}
	static void StaticRunServer(void* objPtr)
	{
		SocketServer* pThis = (SocketServer*)objPtr;
		pThis->RunServer();
	}
#endif
private:
	int error;
	int m_numClients;
	bool shutdownserver;

#ifdef LINUX_MAC
	int Server;
	int client_sock;
#else
	SOCKET Server;
	SOCKET client_sock;
#endif
};

#endif /* SocketServer_hpp */
