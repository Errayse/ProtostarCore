// ProtostarCore.cpp: определяет точку входа для консольного приложения.

#include "stdafx.h"
#include "TProtostarServer.h"

int main()
{
	TProtostarServer m_cServer;

	m_cServer.Init();
	m_cServer.SetIpEndPoint("127.0.0.1", "8000");
	m_cServer.Start();
	m_cServer.Abort();

	return 0;
}

