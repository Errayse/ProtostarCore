#pragma once
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

class THTMLView
{
public:
	THTMLView();
	~THTMLView();

	string GetView(string getHTMLMarkupPath)
	{
		// Окрываем файл для чтения.
		// Читаем HTML разметку и закрываем файл.
		ifstream fileStream(getHTMLMarkupPath);
		if (fileStream.is_open())
		{
			getline(fileStream, m_htmlMarkup, '\0');
		}
		fileStream.close();
		return m_htmlMarkup;
	}

private:
	string m_htmlMarkup;
};

