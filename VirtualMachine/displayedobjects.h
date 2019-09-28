//by Ivan Pavlov

#pragma once
#include "viewenvironment.h"
#include "virtualmachine.h"

//maybe in future with breakable wires:
//enum DeviceValue {One,Zero,Inactive};

//Preferences:
const qreal switcherCornerRadius = 0.1;
const qreal switcherHeightRatio = 0.85;
const qreal switcherWidthRatio = 1.;
const qreal switcherMarginRatio = 0.2;

const qreal gatesHeight = 1.;
const qreal gatesWidthToHeightRatio = 1.;
const qreal gatesWidth = gatesHeight * gatesWidthToHeightRatio;

const qreal n_gatesPointRadiusToHeightRatio = 0.1;
const qreal n_gatesPointRadius = gatesHeight * n_gatesPointRadiusToHeightRatio;

//truths:
const int _NOTGateMaxEmittersCount = 1;

class Rectangle : public DisplayedObject
{
public:
	Rectangle(const QRectF &rect, int drawingLayer = 0) : DisplayedObject(rect, drawingLayer) {}
	void paint(QPainter &painter) override;
};

class Circle : public DisplayedObject
{
public:
	Circle(const QRectF &rect, int drawingLayer = 0) : DisplayedObject(rect, drawingLayer) {}
	void paint(QPainter &painter) override;
};

class DisplayedLogicDevice : public DisplayedObject
{
public:
	DisplayedLogicDevice(const QRectF &rect, int drawingLayer = 0) : DisplayedObject(rect, drawingLayer) {}
	DisplayedLogicDevice(int drawingLayer) : DisplayedObject(drawingLayer) {}
	virtual void processWire(DisplayedWire *wire, int nodeIndex) override = 0;
	virtual void movePressedPoint(const QPointF &newLogicPoint) override;
protected:
	virtual void setNodesPositions() = 0;
};


//interfaces below contain appropriate protected variables. Important: they have no information about model
//used only on the level of the view to make visual connections
class DisplayedEmitterInterface
{
protected:
	void addRecieverWire(DisplayedWire * wire, int nodeIndex);
	QMap<DisplayedWire*, int> recieverWires;

};

class DisplayedRecieverInterface
{
protected:
	void addEmitterWire(DisplayedWire * wire, int nodeIndex);
	QMap<DisplayedWire*, int> emitterWires;
};


//------------------------------------------------------------------------

class TransmitterContainer : public TransmitterEventHandler
{
public:
	TransmitterContainer(Transmitter* transmitter);
	virtual ~TransmitterContainer() { delete _transmitter; }
	void handleValueChange(bool value) { _value = value; }
	Transmitter* getTransmitter() { return _transmitter; }
protected:
	Transmitter* _transmitter;
	bool _value = false;
};

class LogicGateContainer
{
public:
	LogicGateContainer(LogicGate* gate);
	virtual ~LogicGateContainer() { delete _logicGate; }
protected:
	LogicGate* _logicGate;
};

class Lamp : public DisplayedLogicDevice, public DisplayedRecieverInterface, public TransmitterContainer
{
public:
	Lamp(const QRectF &rect, int drawingLayer = 0);
	void paint(QPainter &painter) override;
	virtual const bool simplifiedAreaContainsPoint(const QPointF &point) override;
	virtual void processWire(DisplayedWire *wire, int nodeIndex) override;
private:
	virtual void setNodesPositions() override;
};

class Switcher : public DisplayedLogicDevice, public DisplayedEmitterInterface, public TransmitterContainer
{
public:
	Switcher(const QRectF &rect, int drawingLayer = 0);
	void paint(QPainter &painter) override;
	const bool processPoint(const QPointF &point) override;
	void processMouseButtonUp(const QPointF &p) override;
	virtual void processWire(DisplayedWire *wire, int nodeIndex) override;
private:
	void setNodesPositions() override;
	QPointF initialLogicPressedPoint;
};

class DisplayedGate : public DisplayedLogicDevice, public DisplayedRecieverInterface, public DisplayedEmitterInterface, public LogicGateContainer
{
public:
	virtual void processWire(DisplayedWire *wire, int nodeIndex) override;
protected:
	DisplayedGate(LogicGate* gate, bool is_n_Gate, int maxEmittersCount = -1, int drawingLayer = 1);
	virtual void setNodesPositions() override;

	virtual void setRecieversPostion();
	virtual void setEmittersPostion();
private:
	const QPointF getEmittPosition();
	int _maxEmittersCount;
	bool _is_n_Gate;
};

class DisplayedNOTGate : public DisplayedGate
{
public:
	DisplayedNOTGate(const QPointF &position, int drawingLayer = 1);
	void paint(QPainter &painter) override;
};

class DisplayedANDGate : public DisplayedGate
{
public:
	DisplayedANDGate(const QPointF &position, int drawingLayer = 1);
	void paint(QPainter &painter) override;
};

class DisplayedORGate : public DisplayedGate
{
public:
	DisplayedORGate(const QPointF &position, int drawingLayer = 1);
	void paint(QPainter &painter) override;
private:
	static QPainterPath* pathInstance;
};

class DisplayedXORGate : public DisplayedGate
{
public:
	DisplayedXORGate(const QPointF &position, int drawingLayer = 1);
	void paint(QPainter &painter) override;
private:
	static QPainterPath* pathInstance;
};

class DisplayedWire : public DisplayedObject, public TransmitterContainer
{
public:
	DisplayedWire(const QList<QPointF> &nodes, int drawingLayer = 0);
	static void setNodeRadiusAndWireThickness(qreal r, qreal t) { _nodeRadius = r; _wireThickness = t; }
	void paint(QPainter &painter) override;
	const bool processPoint(const QPointF &point) override;
	void movePressedPoint(const QPointF &newLogicPoint) override;
	void setNewNodePosition(int index, const QPointF &newLogicPoint);
	virtual const bool simplifiedAreaContainsPoint(const QPointF &point) override { throw new std::exception("Is not to be invoked"); }//TODO: deal with "simplifiedAreaContainsPoint" overall
	void processMouseButtonUp(const QPointF &p) override;
	void setEmitter(DisplayedLogicDevice *displayedLogicDevice);
	void setReciever(DisplayedLogicDevice *displayedLogicDevice);
private:
	DisplayedWire();
	void recalculatePositionAndAreaOverall();
	void determineRectFrames(const QPointF& point);
	void setPosAndAreaWithMargins();
	void findFramesForTwoPoints(const QPointF &p1, const QPointF &p2, qreal & xMin, qreal & yMin, qreal & xMax, qreal & yMax) const;

	bool frozenForward = false;
	bool frozenBackward = false;

	static qreal _nodeRadius;
	static qreal _wireThickness;

	QList<QPointF> _nodes;
	qreal minX, minY, maxX, maxY;//Rect Without node radiuses margins
	int draggedNode = -1;

	//DisplayedLogicDevice* emitter;
	//DisplayedLogicDevice* reciever;

	Wire *wire;
};

bool pointWithinRadius(const QPointF &point, const QPointF &center, qreal radius);
void setWiresPositionsEvenly(QMap<DisplayedWire*, int> &wiresNodes, const QPointF &linePosition, const qreal lineHeight);