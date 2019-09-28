//by Ivan Pavlov

#include "displayedobjects.h"
qreal DisplayedWire::_nodeRadius;
qreal DisplayedWire::_wireThickness;
QPainterPath* DisplayedORGate::pathInstance = nullptr;
QPainterPath* DisplayedXORGate::pathInstance = nullptr;

//coments here mostly help debugging

void Rectangle::paint(QPainter & painter)
{
	painter.drawRect(QRectF(QPointF(0, 0), positionAndArea.size()));
}

void Circle::paint(QPainter & painter)
{
	painter.drawEllipse(QRectF(QPointF(0, 0), positionAndArea.size()));
}

DisplayedWire::DisplayedWire(const QList<QPointF> &nodes, int drawingLayer)
	:DisplayedWire::DisplayedObject(drawingLayer), TransmitterContainer(new Wire())
{
	_nodes.append(nodes);
	recalculatePositionAndAreaOverall();
}

void DisplayedWire::paint(QPainter & painter)
{
	painter.save();
	QPen pen = QPen(Qt::PenStyle::SolidLine);
	pen.setWidthF(_wireThickness);

	QPen yellowPen = pen;
	yellowPen.setWidthF(_wireThickness * 3);
	yellowPen.setColor(QColor(Qt::GlobalColor::yellow));

	painter.setPen(pen);
	//painter.drawRect(QRectF(QPointF(0, 0), positionAndArea.size()));
	QPointF point_next;
	QPointF point_i;
	int size = _nodes.size();
	for (int i = 0; i < size - 1; i++)
	{
		point_i = _nodes[i] - positionAndArea.topLeft();
		point_next = _nodes[i + 1] - positionAndArea.topLeft();
		if (point_i != point_next)
		{
			if (_value)
			{
				painter.setPen(yellowPen);
				painter.drawLine(point_i, point_next);
			}

			painter.setPen(pen);
			painter.drawLine(point_i, point_next);
		}
		if (i != 0 || !frozenForward)//Careful with this
			painter.drawEllipse(point_i, _nodeRadius, _nodeRadius);
	}

	if (!frozenBackward)//also with this
		painter.drawEllipse(_nodes.last() - positionAndArea.topLeft(), _nodeRadius, _nodeRadius);
	painter.restore();
}

const bool DisplayedWire::processPoint(const QPointF & point)
{
	if (!positionAndArea.contains(point))
		return false;
	else
	{
		//nodes intersection
		for (int i = 0; i < _nodes.size(); i++)
		{
			if (pointWithinRadius(point, _nodes[i], _nodeRadius))
			{
				draggedNode = i;
				pressedPointWithinObject = point - _nodes[i];
				return true;
			}
		}

		if (frozenForward || frozenBackward)
			return false;

		//lines intersection
		bool r = false;
		qreal x1, x2, y1, y2, b, k, xMin, yMin, xMax, yMax, thicknessMargin;
		for (int i = 0; i < _nodes.size() - 1; i++)
		{
			findFramesForTwoPoints(_nodes[i], _nodes[i + 1], xMin, yMin, xMax, yMax);
			if (xMin != xMax)
			{
				//y = kx + b
				x1 = _nodes[i].x(); y1 = _nodes[i].y();
				x2 = _nodes[i + 1].x(); y2 = _nodes[i + 1].y();
				b = (y2*x1 - y1*x2) / (x1 - x2);
				if (x1 != 0)
					k = (y1 - b) / x1;
				else
					k = (y2 - b) / x2;
				thicknessMargin = (_wireThickness / 2) / cos(atan(k));
				if (point.x() >= xMin && point.x() <= xMax && point.y() >= k*point.x() + b - thicknessMargin && point.y() <= k*point.x() + b + thicknessMargin)
				{
					r = true;
					break;
				}
			}
			else
			{
				if (point.x() >= xMin - _wireThickness / 2 && point.x() <= xMin + _wireThickness / 2 && point.y() >= yMin && point.y() <= yMax)
				{
					r = true;
					break;
				}
			}
		}

		if (r)
		{
			draggedNode = -1;
			pressedPointWithinObject = point - positionAndArea.topLeft();
		}
		return r;
	}
}

void DisplayedWire::movePressedPoint(const QPointF &newLogicPressedPoint)
{
	QPointF newLogicPoint = newLogicPressedPoint - pressedPointWithinObject;
	if (draggedNode != -1)
	{
		setNewNodePosition(draggedNode, newLogicPoint);
	}
	else
	{
		for (QList<QPointF>::iterator it = _nodes.begin(); it < _nodes.end(); it++)
		{
			*it = newLogicPoint + *it - positionAndArea.topLeft();
		}
		positionAndArea.moveTopLeft(newLogicPoint);
	}
}

void DisplayedWire::setNewNodePosition(int index, const QPointF & newLogicPoint)
{
	_nodes[index] = newLogicPoint;
	recalculatePositionAndAreaOverall();
}

void DisplayedWire::processMouseButtonUp(const QPointF & p)
{
	if (draggedNode != 0 && draggedNode != _nodes.size() - 1)
		return;
	QVector<DisplayedObject*> objects = Camera::getInstance().getAllObjectsContainPoint(p);
	for (int i = 0; i < objects.size(); i++)
	{
		if (objects[i] != this)
		{
			objects[i]->processWire(this, draggedNode);
			return;
		}
	}
}


//TODO: rethink these 2 functions below

void DisplayedWire::setEmitter(DisplayedLogicDevice * displayedLogicDevice)
{
	//emitter = displayedLogicDevice;
	if (draggedNode == 0)
		frozenForward = true;
	else frozenBackward = true;
}

void DisplayedWire::setReciever(DisplayedLogicDevice * displayedLogicDevice)
{
	//reciever = displayedLogicDevice;
	if (draggedNode == 0)
		frozenForward = true;
	else frozenBackward = true;
}

void DisplayedWire::recalculatePositionAndAreaOverall()
{
	if (_nodes.size() == 0)
		throw new std::exception("no nodes in a wire");

	QPointF point = _nodes.first();

	minX = point.x(); minY = point.y();
	maxX = point.x(); maxY = point.y();

	for (int i = 1; i < _nodes.size(); i++)
	{
		determineRectFrames(_nodes[i]);
	}
	setPosAndAreaWithMargins();
}

void DisplayedWire::determineRectFrames(const QPointF & point)
{
	if (point.x() > maxX)
		maxX = point.x();
	else if (point.x() < minX)
		minX = point.x();

	if (point.y() > maxY)
		maxY = point.y();
	else if (point.y() < minY)
		minY = point.y();
}

void DisplayedWire::setPosAndAreaWithMargins()
{
	positionAndArea = QRectF(QPointF(minX - _nodeRadius, minY - _nodeRadius), QPointF(maxX + _nodeRadius, maxY + _nodeRadius));
}

void DisplayedWire::findFramesForTwoPoints(const QPointF & p1, const QPointF & p2, qreal & xMin, qreal & yMin, qreal & xMax, qreal & yMax) const
{
	if (p1.x() > p2.x())
	{
		xMax = p1.x();
		xMin = p2.x();
	}
	else
	{
		xMax = p2.x();
		xMin = p1.x();
	}

	if (p1.y() > p2.y())
	{
		yMax = p1.y();
		yMin = p2.y();
	}
	else
	{
		yMax = p2.y();
		yMin = p1.y();
	}
}

void DisplayedLogicDevice::movePressedPoint(const QPointF & newLogicPoint)
{
	DisplayedObject::movePressedPoint(newLogicPoint);
	setNodesPositions();
}

Lamp::Lamp(const QRectF & rect, int drawingLayer)
	: DisplayedLogicDevice(rect, drawingLayer), TransmitterContainer(new LogicOutput())
{
	if (rect.size().width() != rect.size().height())
		throw new std::exception("Rect must be a square");
}

void Lamp::paint(QPainter & painter)
{
	painter.save();
	if (_value)
	{
		QBrush brush = QBrush(Qt::yellow);
		painter.setBrush(brush);
	}
	painter.drawEllipse(QRectF(QPointF(0, 0), positionAndArea.size()));
	qreal half = positionAndArea.size().width() / 2.;
	qreal a = half / sqrt(2);
	painter.drawLine(QPointF(half - a, half - a), QPointF(half + a, half + a));
	painter.drawLine(QPointF(half + a, half - a), QPointF(half - a, half + a));
	painter.restore();
}

const bool Lamp::simplifiedAreaContainsPoint(const QPointF & point)
{
	return pointWithinRadius(point, positionAndArea.center(), positionAndArea.size().width() / 2.);
}

void Lamp::processWire(DisplayedWire * wire, int nodeIndex)
{
	if (emitterWires.size() != 0)
		return;
	wire->setReciever(this);
	dynamic_cast<Wire*>(wire->getTransmitter())->appendFollowingTransmitter(_transmitter);
	addEmitterWire(wire, nodeIndex);
	setNodesPositions();
}

void Lamp::setNodesPositions()
{
	if (emitterWires.size() == 0)
		return;
	QPointF p = positionAndArea.center() - QPointF(positionAndArea.width() / 4, 0);
	emitterWires.firstKey()->setNewNodePosition(emitterWires.first(), p);
}

bool pointWithinRadius(const QPointF & point, const QPointF & center, qreal radius)
{
	return sqrt(pow(point.x() - center.x(), 2) + pow(point.y() - center.y(), 2)) <= radius;
}

Switcher::Switcher(const QRectF & rect, int drawingLayer)
	: DisplayedLogicDevice(rect, drawingLayer), TransmitterContainer(new Wire())
{
}

void Switcher::paint(QPainter & painter)
{
	painter.save();
	painter.drawRoundedRect(QRectF(QPointF(0, 0), positionAndArea.size()), switcherCornerRadius, switcherCornerRadius);
	painter.setBrush(painter.background());
	qreal w = positionAndArea.width() / 2, h = positionAndArea.height();

	painter.drawRoundedRect(QRectF(QPointF(_value ? w * (2. - switcherWidthRatio - switcherMarginRatio) : switcherMarginRatio * w, (h - h * switcherHeightRatio) / 2),
		QSizeF(w*switcherWidthRatio, h*switcherHeightRatio)), switcherCornerRadius, switcherCornerRadius);
	painter.restore();
}

const bool Switcher::processPoint(const QPointF & point)
{
	initialLogicPressedPoint = point;
	return DisplayedLogicDevice::processPoint(point);
}

void Switcher::processMouseButtonUp(const QPointF & p)
{
	if (p == initialLogicPressedPoint)
		_transmitter->changeVariable(!_value);
}

void Switcher::processWire(DisplayedWire * wire, int nodeIndex)
{
	wire->setEmitter(this);
	dynamic_cast<Wire*>(_transmitter)->appendFollowingTransmitter(wire->getTransmitter());
	addRecieverWire(wire, nodeIndex);
	setNodesPositions();
}

void Switcher::setNodesPositions()
{
	setWiresPositionsEvenly(recieverWires, positionAndArea.center() + QPointF(positionAndArea.width() / 4, 0), positionAndArea.height());
}

void DisplayedEmitterInterface::addRecieverWire(DisplayedWire * wire, int nodeIndex)
{
	if (recieverWires.contains(wire))
		throw std::exception("Wire is already attached?");
	else
		recieverWires.insert(wire, nodeIndex);
}

void DisplayedRecieverInterface::addEmitterWire(DisplayedWire * wire, int nodeIndex)
{
	if (emitterWires.contains(wire))
		throw new std::exception("Wire is already attached?");
	else
		emitterWires.insert(wire, nodeIndex);
}

TransmitterContainer::TransmitterContainer(Transmitter * transmitter)
{
	_transmitter = transmitter;
	transmitter->setChangeHandler(this);
}

LogicGateContainer::LogicGateContainer(LogicGate * gate)
{
	_logicGate = gate;
}

void DisplayedGate::processWire(DisplayedWire * wire, int nodeIndex)
{
	if (pressedPointWithinObject.x() > positionAndArea.width() / 2)
	{
		wire->setEmitter(this);
		_logicGate->appendOutput(wire->getTransmitter());
		addRecieverWire(wire, nodeIndex);
		wire->setNewNodePosition(nodeIndex, getEmittPosition());
	}
	else
	{
		if (emitterWires.size() == _maxEmittersCount)//a bit nasty, but i decided to let -1 work here
			return;
		wire->setReciever(this);
		dynamic_cast<Wire*>(wire->getTransmitter())->appendFollowingTransmitter(_logicGate->getAvailablePort());
		addEmitterWire(wire, nodeIndex);
		setEmittersPostion();
	}
}

DisplayedGate::DisplayedGate(LogicGate* gate, bool is_n_Gate, int maxEmittersCount, int drawingLayer)
	: DisplayedLogicDevice(drawingLayer), LogicGateContainer(gate)
{
	_is_n_Gate = is_n_Gate;
	_maxEmittersCount = maxEmittersCount;
}

void DisplayedGate::setNodesPositions()
{
	setEmittersPostion();
	setRecieversPostion();

}

void DisplayedGate::setRecieversPostion()
{
	QPointF emittPoint = getEmittPosition();

	for (auto wireIt = recieverWires.keyBegin(); wireIt != recieverWires.keyEnd(); wireIt++)
	{
		(*wireIt)->setNewNodePosition(recieverWires[*wireIt], emittPoint);
	}
}

void DisplayedGate::setEmittersPostion()
{
	setWiresPositionsEvenly(emitterWires, positionAndArea.center() - QPointF(positionAndArea.width() / 4, 0), positionAndArea.height());
}

void setWiresPositionsEvenly(QMap<DisplayedWire*, int> &wiresNodes, const QPointF & linePosition, const qreal lineHeight)
{
	qreal size = wiresNodes.size();
	if (size == 0)
		return;
	int i = 0;
	for (auto wireIt = wiresNodes.keyBegin(); wireIt != wiresNodes.keyEnd(); wireIt++)
	{
		(*wireIt)->setNewNodePosition(wiresNodes[*wireIt], linePosition + QPointF(0, lineHeight *(2 * i + 1 - size) / (2 * (size + 1))));//QPointF(0,h *(i + 1) /(size + 1) - h/2));
		i++;
	}
}

const QPointF DisplayedGate::getEmittPosition()
{
	QPointF emittPoint = QPointF(positionAndArea.right(), positionAndArea.center().y());
	if (_is_n_Gate)
	{
		emittPoint -= QPointF(n_gatesPointRadius, 0);
	}
	return emittPoint;
}

DisplayedNOTGate::DisplayedNOTGate(const QPointF & position, int drawingLayer)
	: DisplayedGate(new NOTGate(), true, _NOTGateMaxEmittersCount, drawingLayer)
{
	positionAndArea.setSize(QSizeF(gatesHeight * (gatesWidthToHeightRatio + 2 * n_gatesPointRadiusToHeightRatio), gatesHeight));
	positionAndArea.moveTopLeft(position);
}

void DisplayedNOTGate::paint(QPainter & painter)
{
	static const QPointF points[3] =
	{
		QPointF(0,0),QPointF(0,gatesHeight),QPointF(gatesHeight*gatesWidthToHeightRatio, gatesHeight / 2)
	};
	static const QPointF center = QPointF(gatesHeight * gatesWidthToHeightRatio + n_gatesPointRadius, gatesHeight / 2);

	painter.drawPolygon(points, 3);
	painter.drawEllipse(center, n_gatesPointRadius, n_gatesPointRadius);
}

DisplayedANDGate::DisplayedANDGate(const QPointF & position, int drawingLayer)
	: DisplayedGate(new ANDGate(), false)
{
	positionAndArea.setSize(QSizeF(gatesWidth, gatesHeight));
	positionAndArea.moveTopLeft(position);
}

void DisplayedANDGate::paint(QPainter & painter)
{
	static const QPointF points[4] =
	{
		QPointF(gatesWidth / 2,0),QPointF(0,0),QPointF(0,gatesHeight),QPointF(gatesWidth / 2, gatesHeight)
	};
	painter.drawEllipse(QRectF(0, 0, gatesWidth, gatesHeight));
	painter.save();
	painter.setPen(Qt::NoPen);
	painter.drawPolygon(points, 4);
	painter.restore();
	painter.drawPolyline(points, 4);

}

DisplayedORGate::DisplayedORGate(const QPointF & position, int drawingLayer)
	: DisplayedGate(new ORGate(), false)
{
	positionAndArea.setSize(QSizeF(gatesWidth + 2 * n_gatesPointRadius, gatesHeight));
	positionAndArea.moveTopLeft(position);

	if (pathInstance == nullptr)
	{
		pathInstance = new QPainterPath();
		pathInstance->lineTo(gatesWidth / 2, 0);
		pathInstance->cubicTo(gatesWidth / 2, 0, gatesWidth, 0, gatesWidth + 2 * n_gatesPointRadius, gatesHeight / 2);
		pathInstance->cubicTo(gatesWidth, gatesHeight, gatesWidth / 2, gatesHeight, gatesWidth / 2, gatesHeight);
		pathInstance->lineTo(0, gatesHeight);
		pathInstance->cubicTo(gatesWidth / 2, gatesHeight / 2, 0, 0, 0, 0);
	}
}

void DisplayedORGate::paint(QPainter & painter)
{
	painter.drawPath(*pathInstance);
}

DisplayedXORGate::DisplayedXORGate(const QPointF & position, int drawingLayer)
	: DisplayedGate(new XORGate(), false)
{
	positionAndArea.setSize(QSizeF(gatesWidth + 3 * n_gatesPointRadius, gatesHeight));
	positionAndArea.moveTopLeft(position);

	if (pathInstance == nullptr)
	{
		pathInstance = new QPainterPath();
		pathInstance->cubicTo(0, 0, gatesWidth / 2, gatesHeight / 2, 0, gatesHeight);
		pathInstance->cubicTo(gatesWidth / 2, gatesHeight / 2, 0, 0, 0, 0);
		pathInstance->moveTo(n_gatesPointRadius, 0);
		pathInstance->lineTo(gatesWidth / 2 + n_gatesPointRadius, 0);
		pathInstance->cubicTo(gatesWidth / 2 + n_gatesPointRadius, 0, gatesWidth + n_gatesPointRadius, 0, gatesWidth + 3 * n_gatesPointRadius, gatesHeight / 2);
		pathInstance->cubicTo(gatesWidth + n_gatesPointRadius, gatesHeight, gatesWidth / 2 + n_gatesPointRadius, gatesHeight, gatesWidth / 2 + n_gatesPointRadius, gatesHeight);
		pathInstance->lineTo(0 + n_gatesPointRadius, gatesHeight);
		pathInstance->cubicTo(gatesWidth / 2 + n_gatesPointRadius, gatesHeight / 2, n_gatesPointRadius, 0, n_gatesPointRadius, 0);
	}
}

void DisplayedXORGate::paint(QPainter & painter)
{
	painter.drawPath(*pathInstance);
}