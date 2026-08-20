#pragma once

#include <string>


class Terminal
{
public:
    Terminal();

    // Read the source code file path and
    // validate it for correntness and existence of the file
    std::string getFilePath();

    // Read the user input
    char getUserInput();

    // Display a message
    void displayMessage(std::string msg);

    // Clear terminal window of previous output
    void clearWindow();
};