#include <iostream>
#include <fstream>
#include <algorithm>
#include "Terminal.hpp"

#ifdef _WIN32
	#include <conio.h>
#else
	#include <curses.h>
	#include <termios.h>
	#include <unistd.h>
#endif

using namespace std;


Terminal::Terminal() {};

string Terminal::getFilePath()
{ 
	string path;
	cout << "Enter code file path: ";
	cin >> path;
	int extStartIndex = path.find_last_of(".") + 1;
	string ext = "";
	copy(path.begin() + extStartIndex, path.end(), std::back_inserter(ext));
	while (ext != "mdl")
	{
		cout << "Unsupported file extension \"." << ext << "\". Please try a different file.\n" << endl;
		cout << "Enter code file path: ";
		cin >> path;
		extStartIndex = path.find_last_of(".") + 1;
		ext = "";
		copy(path.begin() + extStartIndex, path.end(), std::back_inserter(ext));
	}
	ifstream f(path);
	if (!f.good())
	{
		cout << "Cannot open file \'" << path << "\'. Please try a different file.\n" << endl;
		path = getFilePath();
	}
	transform(path.begin(), path.end(), path.begin(), [](char c){
		if (c == '/')
		{
			return '\\';
		}
		return c;
	});
	return path;
}

char Terminal::getUserInput()
{
	char ch;
	#ifdef _WIN32
		ch = getch();
	#else
      	termios oldSettings{};
      	tcgetattr(STDIN_FILENO, &oldSettings);
      
      	termios newSettings = oldSettings;
      	newSettings.c_lflag &= ~(ICANON | ECHO);
      
      	tcsetattr(STDIN_FILENO, TCSANOW, &newSettings);
      	read(STDIN_FILENO, &ch, 1);
      	tcsetattr(STDIN_FILENO, TCSANOW, &oldSettings);
    #endif

    return ch;
}

void Terminal::displayMessage(string msg)
{
    cout << msg << endl;
}

void Terminal::clearWindow()
{
    // ANSI Escape sequences:
	// \033[2J clears the screen
	// \033[H moves the cursor to the top-left
	//cout << "\033[2J\033[1;1H";
	//cout.flush();
	#ifdef _WIN32
		system("cls");
	#else
		system("clear");
	#endif
}