#include "Processor.h"

Processor::Processor(std::list<std::string> _commands) :
	commands_(_commands),
	currentCommand_(commands_.begin()),
	registerIR_("NO COMMAND"),
	instruction_("no instruction"),
	registerPS_('+'),
	registerOperand_(false),
	operandRegisterNumber_(0),
	operand_(0),
	registerAccumulator_(makeRandomBits()),
	registerR1_(makeRandomBits()),
	registerR2_(makeRandomBits()),
	registerR3_(makeRandomBits()),
	registerPC_(0),
	registerTC_(2)
{
}

Processor::~Processor()
{
}

bool Processor::nextTick()
{
	const size_t tcValue = registerTC_.to_ulong();

	if ((currentCommand_ != commands_.end()) || (tcValue < 2))
	{
		if (tcValue < 2)
		{
			// command execution

			registerTC_ = 2;
			executeCurrentCommand();
		}
		else
		{
			// command loading

			registerTC_ = 1;

			registerIR_ = *currentCommand_;

			instruction_ = getCommandInstruction(registerIR_);

			registerOperand_ = isRegisterOperand(registerIR_);

			if (registerOperand_)
			{
				// operation with register

				operandRegisterNumber_ = getRegisterNumberFromCommand(registerIR_);
			}
			else
			{
				// operation with constant

				operand_ = getOperandFromCommand(registerIR_);
			}

			registerPC_ = registerPC_.to_ulong() + 1;
			currentCommand_++;
		}

		return true;
	}
	else
	{
		return false;
	}
}

void Processor::makeOutput()
{
	std::cout << "COMMAND: " << registerIR_ << "\n\n";

	if (registerOperand_)
	{
		std::cout << "INSTRUCTION: " << instruction_ << ", OPERAND: R" << operandRegisterNumber_ << "\n";
	}
	else
	{
		std::cout << "INSTRUCTION: " << instruction_ << ", OPERAND: " << operand_ << "\n";
	}

	std::cout << "register accumulator = " << registerAccumulator_.to_string() << "\n";

	std::cout << "R1 = " << registerR1_.to_string() << "\n";
	std::cout << "R2 = " << registerR2_.to_string() << "\n";
	std::cout << "R3 = " << registerR3_.to_string() << "\n";

	std::cout << "PS: " << registerPS_ << ", PC = " << registerPC_.to_ulong() << ", TC = " << registerTC_.to_ulong() << "\n\n";

	std::cout << "=================================\n";
}

std::bitset<Processor::processorBitCapacity_> Processor::makeRandomBits()
{
	std::bitset<Processor::processorBitCapacity_> result;

	for (int i = 0; i < processorBitCapacity_; i++)
	{
		result[i] = rand() % 2;
	}

	return result;
}

bool Processor::isRegisterOperand(const std::string _command)
{
	return (_command.find('R') != std::string::npos);
}

std::string Processor::getCommandInstruction(const std::string _command)
{
	const size_t spacePosition = _command.find(' ');
	return _command.substr(0, spacePosition);
}

uint16_t Processor::getOperandFromCommand(const std::string _command)
{
	const size_t spacePosition = _command.find(' ');
	return std::stoi(_command.substr(spacePosition, _command.size() - spacePosition));
}

size_t Processor::getRegisterNumberFromCommand(const std::string _command)
{
	const size_t spacePosition = _command.find('R');
	return std::stoi(_command.substr(spacePosition + 1, _command.size() - spacePosition - 1));
}

void Processor::executeCurrentCommand()
{
	if (instruction_ == "LOAD") executeLOAD();
	if (instruction_ == "SAVE") executeSAVE();
	if (instruction_ == "ADD") executeADD();
	if (instruction_ == "CYCLE") executeCYCLE();
}

void Processor::executeLOAD()
{
	if (registerOperand_)
	{
		//load from register

		switch (operandRegisterNumber_)
		{
		case 1:

			for (int i = 0; i < processorBitCapacity_; i++)
			{
				registerAccumulator_[i] = registerR1_[i];
			}

			break;
		case 2:

			for (int i = 0; i < processorBitCapacity_; i++)
			{
				registerAccumulator_[i] = registerR2_[i];
			}

			break;

		case 3:

			for (int i = 0; i < processorBitCapacity_; i++)
			{
				registerAccumulator_[i] = registerR3_[i];
			}

			break;


		}

		registerPS_ = (registerAccumulator_[processorBitCapacity_ - 1] == 1 ? '-' : '+');
	}
	else
	{
		// load constant

		int16_t
			operand = operand_,
			operandAbsoluteValue = (operand < 0 ? -operand : operand);

		operandAbsoluteValue = operandAbsoluteValue % (1 << processorBitCapacity_);

		// load binary digits
		for (int i = 0; i < processorBitCapacity_; i++)
		{
			registerAccumulator_[i] = operandAbsoluteValue % 2;
			operandAbsoluteValue /= 2;
		}

		if (operand < 0)
		{
			registerAccumulator_[processorBitCapacity_ - 1] = 1 - registerAccumulator_[processorBitCapacity_ - 1];
		}

		registerPS_ = (registerAccumulator_[processorBitCapacity_ - 1] == 0 ? '+' : '-');

	}
}

void Processor::executeSAVE()
{
	if (registerOperand_)
	{
		registerPS_ = (registerAccumulator_[processorBitCapacity_ - 1] == 0 ? '+' : '-');

		switch (operandRegisterNumber_)
		{
		case 1:

			for (int i = 0; i < processorBitCapacity_; i++)
			{
				registerR1_[i] = registerAccumulator_[i];
			}

			break;
		case 2:

			for (int i = 0; i < processorBitCapacity_; i++)
			{
				registerR2_[i] = registerAccumulator_[i];
			}

			break;
		case 3:

			for (int i = 0; i < processorBitCapacity_; i++)
			{
				registerR3_[i] = registerAccumulator_[i];
			}

			break;
		}
	}
}

void Processor::executeADD()
{
	if (registerOperand_)
	{
		//load from register

		switch (operandRegisterNumber_)
		{
		case 1:
			add(registerAccumulator_, registerR1_);
			break;
		case 2:
			add(registerAccumulator_, registerR2_);
			break;
		case 3:
			add(registerAccumulator_, registerR3_);
			break;
		}

		registerPS_ = (registerAccumulator_[processorBitCapacity_ - 1] == 1 ? '-' : '+');
	}
	else
	{
		std::bitset<Processor::processorBitCapacity_> operandBits;

		int16_t operand = operand_;

		for (int i = 0; i < processorBitCapacity_ - 1; i++)
		{
			operandBits[i] = operand % 2;
			operand /= 2;
		}

		operandBits[processorBitCapacity_ - 1] = (operand_ >= 0 ? 0 : 1);

		add(registerAccumulator_, operandBits);
	}
}

void Processor::executeCYCLE()
{
	std::bitset<Processor::processorBitCapacity_ * 3> result;

	const int operand = (operand_ <= 0 ? (-operand_) % (processorBitCapacity_ * 3) : processorBitCapacity_ * 3 - operand_ % (processorBitCapacity_ * 3));

	for (int i = 0; i < processorBitCapacity_; i++)
	{
		result[i] = registerR1_[i];
	}
	for (int i = 0; i < processorBitCapacity_; i++)
	{
		result[processorBitCapacity_ + i] = registerR2_[i];
	}
	for (int i = 0; i < processorBitCapacity_; i++)
	{
		result[2 * processorBitCapacity_ + i] = registerR3_[i];
	}

	for (int i = 0; i < processorBitCapacity_; i++)
	{
		registerR1_[i] = result[(i + operand) % (processorBitCapacity_ * 3)];
	}
	for (int i = 0; i < processorBitCapacity_; i++)
	{
		registerR2_[i] = result[(i + operand + processorBitCapacity_) % (processorBitCapacity_ * 3)];
	}
	for (int i = 0; i < processorBitCapacity_; i++)
	{
		registerR3_[i] = result[(i + operand + 2 * processorBitCapacity_) % (processorBitCapacity_ * 3)];
	}
}

int Processor::compareAbsolute(
	const std::bitset<Processor::processorBitCapacity_> _first,
	const std::bitset<Processor::processorBitCapacity_> _second)
{
	int result = 0;
	for (int i = 0; i < processorBitCapacity_ - 1; i++)
	{
		if (_first[i] < _second[i]) result = -1;
		if (_first[i] > _second[i]) result = 1;
	}
	return result;
}

void Processor::add(
	std::bitset<Processor::processorBitCapacity_> &_result,
	const std::bitset<Processor::processorBitCapacity_> _added)
{
	if (_result[processorBitCapacity_ - 1] == _added[processorBitCapacity_ - 1])
	{
		uint8_t rest = 0;

		for (int i = 0; i < processorBitCapacity_ - 1; i++)
		{
			uint8_t sum = _result[i] + _added[i] + rest;
			_result[i] = sum % 2;
			rest = sum / 2;
		}
	}
	else
	{
		int compare = compareAbsolute(_result, _added);
		if (compare == 0)
		{
			for (int i = 0; i < processorBitCapacity_; i++)
			{
				_result[i] = 0;
			}
		}
		else if (compare < 0)
		{
			int8_t rest = 0;

			for (int i = 0; i < processorBitCapacity_ - 1; i++)
			{
				int8_t sum = _added[i] - _result[i] + rest;

				if (sum == -2)
				{
					_result[i] = 0;
					rest = -1;
				}
				else if (sum == -1)
				{
					_result[i] = 1;
					rest = -1;
				}
				else
				{
					_result[i] = sum;
					rest = 0;
				}
			}

			_result[processorBitCapacity_ - 1] = 1 - _result[processorBitCapacity_ - 1];
		}
		else
		{
			int8_t rest = 0;

			for (int i = 0; i < processorBitCapacity_ - 1; i++)
			{
				int8_t sum = _result[i] - _added[i] + rest;

				if (sum == -2)
				{
					_result[i] = 0;
					rest = -1;
				}
				else if (sum == -1)
				{
					_result[i] = 1;
					rest = -1;
				}
				else
				{
					_result[i] = sum;
					rest = 0;
				}
			}
		}
	}
}