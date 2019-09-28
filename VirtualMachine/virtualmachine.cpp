//by Ivan Pavlov

#include "virtualmachine.h"

void Transmitter::changeVariable(bool value)
{
	if (_changeHandler != nullptr)
		_changeHandler->handleValueChange(value);
}

void Transmitter::setChangeHandler(TransmitterEventHandler* changeHandler)
{
	_changeHandler = changeHandler;
}

LogicGate::LogicGate()
{
	availablePorts = new std::list<Port*>();
	inputVariables = new std::list<bool>();
	outputs = new std::list<Transmitter*>();
}

LogicGate::~LogicGate()
{
	delete inputVariables;
	for (std::list<Port*>::iterator it = availablePorts->begin(); it != availablePorts->end(); it++)
	{
		delete *it;
	}
	delete availablePorts;
	awareTransmittersBeforeDelitingTheEmitter(outputs);
}

LogicGate::Port * LogicGate::getAvailablePort()
{
	Port *port;
	if (!availablePorts->empty())
	{
		port = availablePorts->front();
		availablePorts->pop_front();
	}
	else
	{
		port = createPort();
	}
	return port;
}

void LogicGate::appendOutput(Transmitter * output)
{
	outputs->push_back(output);
	recalculate();
}

LogicGate::Port * LogicGate::createPort()
{
	return new Port(this);
}

void LogicGate::emmitOutput(bool result)
{
	if (outputs->size() != 0)
		for (auto transmitter : *outputs)
		{
			transmitter->changeVariable(result);
		}
}

void LogicGate::Port::changeVariable(bool value)
{
	Transmitter::changeVariable(value);
	*gateVariable = value;
	_gate->recalculate();
}

void LogicGate::Port::abandonThisTransmitter()
{
	_gate->availablePorts->push_back(this);
	changeVariable(false);
}

LogicGate::Port::Port(LogicGate * gate)
{
	_gate = gate;
	gate->inputVariables->emplace_back();
	gateVariable = &(gate->inputVariables->back());
}

bool NOTGate::recalculate()
{
	bool result;
	if (inputVariables->size() == 0)
		result = true;
	else
		result = !inputVariables->front();
	emmitOutput(result);
	return result;
}

LogicGate::Port * NOTGate::getAvailablePort()
{
	Port *port;
	
	if (!availablePorts->empty())
	{
		port = availablePorts->front();
		availablePorts->pop_front();
	}
	else if (inputVariables->empty())
	{
		port = createPort();
	}
	else
		throw std::exception("No ports available");
	return port;
}

bool ANDGate::recalculate()
{
	bool result;
	if (inputVariables->size() != 0)
	{
		result = true;
		for (std::list<bool>::iterator it = inputVariables->begin(); it != inputVariables->end(); it++)
		{
			if (!*it)
			{
				result = false;
				break;
			}
		}
	}
	else
		result = false;
	emmitOutput(result);
	return result;
}

bool ORGate::recalculate()
{
	bool result = false;
	for (std::list<bool>::iterator it = inputVariables->begin(); it != inputVariables->end(); it++)
	{
		if (*it)
		{
			result = true;
			break;
		}
	}
	emmitOutput(result);
	return result;
}

bool XORGate::recalculate()
{
	bool result;
	if (inputVariables->size() == 0)
		result = false;
	else
		result = inputVariables->front() ^ inputVariables->back();
	emmitOutput(result);
	return result;
}

LogicGate::Port * XORGate::getAvailablePort()
{
	Port *port;

	if (!availablePorts->empty())
	{
		port = availablePorts->front();
		availablePorts->pop_front();
	}
	else if (inputVariables->size() < 2)
	{
		port = createPort();
	}
	else
		throw std::exception("No ports available");
	return port;
}

bool NANDGate::recalculate()
{
	return !ANDGate::recalculate();
}

bool NORGate::recalculate()
{
	return !ORGate::recalculate();
}

bool XNORGate::recalculate()
{
	return !XORGate::recalculate();
}

Wire::Wire()
{
	followingTransmitters = new std::list<Transmitter*>();
	_value = false;
}

Wire::~Wire()
{
	awareTransmittersBeforeDelitingTheEmitter(followingTransmitters);
}

void Wire::changeVariable(bool value)
{
	if (_value == value)
		return;
	_value = value;
	Transmitter::changeVariable(value);
	for (std::list<Transmitter*>::iterator it = followingTransmitters->begin(); it != followingTransmitters->end(); it++)
	{
		(*it)->changeVariable(value);
	}
}

void Wire::abandonThisTransmitter()
{
	changeVariable(false);
}

void Wire::appendFollowingTransmitter(Transmitter * transmitter)
{
	followingTransmitters->push_back(transmitter);
	transmitter->changeVariable(_value);
}

void awareTransmittersBeforeDelitingTheEmitter(std::list<Transmitter*>* list)
{
	for (std::list<Transmitter*>::iterator it = list->begin(); it != list->end(); it++)
	{
		(*it)->abandonThisTransmitter();
	}
	delete list;
}
