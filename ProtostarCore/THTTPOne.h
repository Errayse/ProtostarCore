#pragma once
#include "IHTTP.h"
#include "THTMLView.h"
#include <iostream>
#include <sstream>
#include <string>

class THTTPOne : public IHTTP
{
public:
	THTTPOne();
	~THTTPOne();

	void Request()
	{
		
	}

	std::stringstream Response(THTMLView get_cHTMLView)
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
};

