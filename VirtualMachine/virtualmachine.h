//by Ivan Pavlov

#pragma once
#include <list>

class VirtualMachine
{
	
};

class LogicDevice
{
public:
		virtual ~LogicDevice() {}
};

class TransmitterEventHandler
{
public:
	virtual void handleValueChange(bool value) = 0;
};

class Transmitter : public LogicDevice
{
public:
	virtual ~Transmitter() {}
	virtual void changeVariable(bool value);
	virtual void abandonThisTransmitter() = 0;
	virtual void setChangeHandler(TransmitterEventHandler* changeHandler);
private:
	TransmitterEventHandler* _changeHandler = nullptr;
};

class LogicOutput : public Transmitter
{
public:
	virtual void abandonThisTransmitter() {}
};

class LogicGate : public LogicDevice
{
public:
	LogicGate();
	virtual ~LogicGate();
	virtual bool recalculate() = 0;

	class Port : public Transmitter
	{
	public:
		void changeVariable(bool value) override;
		void abandonThisTransmitter() override;
		friend class LogicGate;
	private:
		Port(LogicGate* gate);
		bool* gateVariable;
		LogicGate* _gate;
	};

	virtual Port* getAvailablePort();
	virtual void appendOutput(Transmitter* output);
protected:
	std::list<Port*> *availablePorts;
	std::list<bool> *inputVariables;
	std::list<Transmitter*> *outputs;

	Port* createPort();
	void emmitOutput(bool result);
};

class NOTGate : public LogicGate
{
public:
	bool recalculate() override;
	Port* getAvailablePort() override;
};

class ANDGate : public LogicGate
{
public:
	virtual bool recalculate() override;
};

class ORGate : public LogicGate
{
public:
	virtual bool recalculate() override;
};

class XORGate : public LogicGate
{
public:
	virtual bool recalculate() override;
	Port* getAvailablePort() override;
};

class NANDGate : public ANDGate
{
public:
	bool recalculate() override;
};

class NORGate : public ORGate
{
public:
	bool recalculate() override;
};

class XNORGate : public XORGate
{
public:
	bool recalculate() override;
};

class Wire : public Transmitter
{
public:
	Wire();
	~Wire();
	void changeVariable(bool value) override;
	void abandonThisTransmitter() override;
	void appendFollowingTransmitter(Transmitter* transmitter);
private:
	std::list<Transmitter*> *followingTransmitters;
	bool _value;
};

void awareTransmittersBeforeDelitingTheEmitter(std::list<Transmitter*> *list);