#include <fstream>
#include <list>
#include "Processor.h"

void main()
{

	std::list<std::string> commands;
	/*
	commands.push_back("LOAD -56");
	commands.push_back("SAVE R1");
	commands.push_back("ADD 19");
	commands.push_back("SAVE R2");
	commands.push_back("LOAD R1");
	commands.push_back("ADD R2");
	commands.push_back("CYCLE -3");
	*/

	std::ifstream infile("instr.txt");

	std::string line;
	while (std::getline(infile, line))
	{
		commands.push_back(line);
	}

	Processor processor(commands);

	while (processor.nextTick())
	{
		std::string s;

		processor.makeOutput();
		std::getline(std::cin, s);
	}

	system("pause");
}