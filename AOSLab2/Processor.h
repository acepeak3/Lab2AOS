#pragma once

#include <string>
#include <list>
#include <bitset>
#include <iostream>

class Processor
{
public:

	static const size_t processorBitCapacity_ = 12;

	Processor(std::list<std::string> _commands);
	~Processor();

	bool nextTick();

	void makeOutput();

private:

	std::list<std::string> commands_;

	std::list<std::string>::iterator currentCommand_;

	std::string
		registerIR_,
		instruction_;

	char registerPS_;

	bool registerOperand_;

	size_t operandRegisterNumber_;
	int16_t operand_;

	std::bitset<processorBitCapacity_>
		registerAccumulator_,
		registerR1_,
		registerR2_,
		registerR3_,
		registerPC_,
		registerTC_;

	static std::bitset<Processor::processorBitCapacity_> makeRandomBits();
	static std::string getCommandInstruction(const std::string _command);
	static bool isRegisterOperand(const std::string _command);
	static uint16_t getOperandFromCommand(const std::string _command);
	static size_t getRegisterNumberFromCommand(const std::string _command);

	static int compareAbsolute(
		const std::bitset<Processor::processorBitCapacity_> _first,
		const std::bitset<Processor::processorBitCapacity_> _second);
	static void add(
		std::bitset<Processor::processorBitCapacity_> &_result,
		const std::bitset<Processor::processorBitCapacity_> _added);

	void executeCurrentCommand();

	void executeLOAD();
	void executeSAVE();
	void executeADD();
	void executeCYCLE();
};