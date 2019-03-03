#pragma once

#include "stdafx.h"
#include "THTMLView.h"
#include <iostream>
#include <sstream>
#include <string>

// Для корректной работы freeaddrinfo в MinGW.
// Подробнее: http://stackoverflow.com/a/20306451
#define _WIN32_WINNT 0x501

// Максимальный размер буфера клиента.
#define D_MAX_CLIENT_BUFFER_SIZE 1024

#include <WinSock2.h>
#include <WS2tcpip.h>

// Необходимо, чтобы линковка происходила с DLL-библиотекой для работы с сокетами.
#pragma comment(lib, "Ws2_32.lib")

using std::cerr;

class TProtostarServer
{
private:
	THTMLView m_cHTMLlView;
	WSADATA	  m_WindowsSocketsData;
	SOCKET    m_listenSocket;

	// Структура, хранящая информацию об IP-адресе слущающего сокета.
	struct addrinfo* m_IpEndPoint = NULL;

	// Старт использования библиотеки сокетов процессом (подгружается Ws2_32.dll).
	void LoadingWindowsSocketLibrary()
	{
		if (WSAStartup(MAKEWORD(2, 2), &m_WindowsSocketsData) != 0) {
			cerr << "WSADATA Startup failed" << WSAGetLastError() << "\n";

			exit(1);
		}
	}

	// Метод для создания сокета.
	void CreateSocket()
	{
		SOCKET client_socket  = CreateClientSocket();
		       m_listenSocket = CreateListenSocket();

		BindSocket(&m_listenSocket);
		InitListenSocket(&m_listenSocket);

		AcceptIncomingConnections(&m_listenSocket, client_socket);
	}

	// Принимаем входящие соединения.
	void AcceptIncomingConnections(SOCKET *get_ListenSocket, SOCKET get_ClientSocket)
	{
		while (true)
		{
			get_ClientSocket = accept(*get_ListenSocket, NULL, NULL);

			if (get_ClientSocket == INVALID_SOCKET) 
			{
				cerr << "Accept denied: " << WSAGetLastError() << "\n";

				closesocket(get_ClientSocket);
				closesocket(*get_ListenSocket);
				WSACleanup();
				exit(1);
			}

			char request_buffer[D_MAX_CLIENT_BUFFER_SIZE];
			int request_result = Request(&get_ClientSocket, request_buffer);
			if (request_result == SOCKET_ERROR) 
			{
				cerr << "Request failed or client disconected: " << request_result << "\n";

				closesocket(get_ClientSocket);
			}
			else if (request_result == 0) 
			{
				cerr << "Client close connection...\n";
			}
			else if (request_result > 0)
			{
				request_buffer[request_result] = '\0';

				cout << request_buffer;

				Response(&get_ClientSocket);
			}
		}
	}

	void Response(SOCKET *get_clientSocket)
	{
		stringstream http_head = HTTPHeadGeneration(m_cHTMLlView);

		if (send(*get_clientSocket, http_head.str().c_str(), http_head.str().length(), 0) == SOCKET_ERROR)
		{
			cerr << "Send failed: " << WSAGetLastError() << "\n";
		}

		closesocket(*get_clientSocket);
	}

	// Метод получения запроса от клиента.
	int Request(SOCKET *get_ClientSocket, char *get_Buffer)
	{ 
		int initResult = recv(*get_ClientSocket, get_Buffer, D_MAX_CLIENT_BUFFER_SIZE, 0);
		return initResult;
	}

	// Метод привязки слушаего сокета к IP-адресу.
	void BindSocket(SOCKET *get_ListenSocket)
	{
		if (bind(*get_ListenSocket, m_IpEndPoint->ai_addr, (int)m_IpEndPoint->ai_addrlen) == SOCKET_ERROR)
		{
			cerr << "Bind failed with error: " << WSAGetLastError() << "\n";
			freeaddrinfo(m_IpEndPoint);

			closesocket(*get_ListenSocket);
			WSACleanup();
			exit(1);
		}
	}

	// Метод инициализации слушающего сокета.
	void InitListenSocket(SOCKET *get_ListenSocket)
	{
		if (listen(*get_ListenSocket, SOMAXCONN) == SOCKET_ERROR)
		{
			cerr << "listen failed with error: " << WSAGetLastError() << "\n";

			closesocket(*get_ListenSocket);
			WSACleanup();
			exit(1);
		}
	}

	// Метод отправки ответа сервера клиенту.
	stringstream HTTPHeadGeneration(THTMLView get_cHTMLView)
	{
		std::stringstream response;
		std::stringstream response_body;

		response_body << get_cHTMLView.GetView("index.html");

		response << "HTTP/1.1 200 OK\r\n"
			<< "Version: HTTP/1.1\r\n"
			<< "Content-Type: text/html; charset=utf-8\r\n"
			<< "Content-Length: " << response_body.str().length()
			<< "\r\n\r\n"
			<< response_body.str();

		return response;
	}

	// Метод для создания слушающего сокета.
	SOCKET CreateListenSocket()
	{
		SOCKET listen_socket = socket(m_IpEndPoint->ai_family, m_IpEndPoint->ai_socktype, m_IpEndPoint->ai_protocol);
		if (listen_socket == INVALID_SOCKET) 
		{
			cerr << "Error at socket: " << WSAGetLastError() << "\n";
			freeaddrinfo(m_IpEndPoint);

			WSACleanup();
			exit(1);
		}
		return listen_socket;
	}

	// Метод для создания клиентского сокета.
	SOCKET CreateClientSocket()
	{
		SOCKET client_socket = INVALID_SOCKET;
		return client_socket;
	}

	// Шаблон для инициализации структуры адреса.
	addrinfo IpEndPointInitTemplate()
	{
		struct addrinfo m_IpEndPoint;
		ZeroMemory(&m_IpEndPoint, sizeof(m_IpEndPoint));

		m_IpEndPoint.ai_family = AF_INET;
		m_IpEndPoint.ai_socktype = SOCK_STREAM;
		m_IpEndPoint.ai_protocol = IPPROTO_TCP;
		m_IpEndPoint.ai_flags = AI_PASSIVE;

		return m_IpEndPoint;
	}

public:
	TProtostarServer();
	~TProtostarServer();

	void Init()
	{
		LoadingWindowsSocketLibrary();
	}

	// Инициализируем структуру, хранящую адрес сокета - m_addressInfo.
	void SetIpEndPoint(PCSTR get_IpAdress, PCSTR get_Port)
	{
		if (getaddrinfo(get_IpAdress, get_Port, &IpEndPointInitTemplate(), &m_IpEndPoint) != 0)
		{
			cerr << "getaddrinfo failed: " << WSAGetLastError() << "\n";

			WSACleanup();
			exit(1);
		}
	}

	void Start()
	{		
		CreateSocket();
	}

	void Abort()
	{
		closesocket(m_listenSocket);
		freeaddrinfo(m_IpEndPoint);
		WSACleanup();
	}
};

