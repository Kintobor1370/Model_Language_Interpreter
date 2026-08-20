#include "Interpreter.hpp"
#include "Terminal.hpp"

int main()
{
	Terminal terminal;

    terminal.clearWindow();
    std::string sourceCode = terminal.getFilePath();
    Interpreter interpreter(sourceCode);
	
    terminal.clearWindow();
    interpreter.interpret();
    
	terminal.displayMessage("Press any key to finish");
    terminal.getUserInput();
    
	return 0;
}
