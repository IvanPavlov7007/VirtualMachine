//by Ivan Pavlov

#include "QVector"
#include "viewenvironment.h"
#include "displayedobjects.h"

Camera* Camera:: _instance;

//coments here mostly help debugging

Camera::Camera(qreal dpi, const QSize & size, const QPointF & logicPos)
{
	_instance = this;
	DisplayedWire::setNodeRadiusAndWireThickness(0.1f, 0.05f);

#pragma region Test
	//Graphic tests
	/*for (int i = 0; i < 55; i++)
	{
		appendNewDisplayedObject_WithoutRecalculating(new Rectangle(QRectF(0 + i *sin(i), 0 + i, 1, 1),1));
		appendNewDisplayedObject_WithoutRecalculating(new Rectangle(QRectF(4 + i, 3 + i * cos(i), 2, 2),1));
		appendNewDisplayedObject_WithoutRecalculating(new Circle(QRectF(-2 + i, 0 + i, 2, 2),1));
	}*/
	
	//Object tests
	/*QList<QPointF> nodes;
	nodes.append(QPointF(0, 0));
	nodes.append(QPointF(0, 2));
	nodes.append(QPointF(2, 2));
	nodes.append(QPointF(2, 0));
	appendNewDisplayedObject_WithoutRecalculating(new DisplayedWire(nodes));
	appendNewDisplayedObject_WithoutRecalculating(new DisplayedWire(nodes));
	appendNewDisplayedObject_WithoutRecalculating(new DisplayedWire(nodes));
	appendNewDisplayedObject_WithoutRecalculating(new DisplayedWire(nodes));

	QList<QPointF> nodes1;
	nodes1.append(QPointF(-1, -1));
	nodes1.append(QPointF(2, 2));
	nodes1.append(QPointF(3, 5));
	nodes1.append(QPointF(6, -4));
	appendNewDisplayedObject_WithoutRecalculating(new DisplayedWire(nodes1));

	QList<QPointF> nodes2;
	nodes2.append(QPointF(-1, -1));
	nodes2.append(QPointF(2, 2));
	appendNewDisplayedObject_WithoutRecalculating(new DisplayedWire(nodes2));

	QList<QPointF> nodes3;
	nodes3.append(QPointF(-5, -2));
	nodes3.append(QPointF(-2, 2));
	nodes3.append(QPointF(4, 2));
	nodes3.append(QPointF(2, -5));
	nodes3.append(QPointF(1, -3));
	appendNewDisplayedObject_WithoutRecalculating(new DisplayedWire(nodes3));

	QList<QPointF> nodes4;
	nodes4.append(QPointF(-4, 7));
	appendNewDisplayedObject_WithoutRecalculating(new DisplayedWire(nodes4));

	appendNewDisplayedObject_WithoutRecalculating(new Switcher(QRectF(-5, 3, 2, 1), 1));
	appendNewDisplayedObject_WithoutRecalculating(new Switcher(QRectF(-5, 3, 2, 1), 1));
	appendNewDisplayedObject_WithoutRecalculating(new Lamp(QRectF(1, 1, 1, 1),1));
	appendNewDisplayedObject_WithoutRecalculating(new Lamp(QRectF(1, 1, 1, 1), 1));
	appendNewDisplayedObject_WithoutRecalculating(new DisplayedNOTGate(QPointF(2,2), 1));
	appendNewDisplayedObject_WithoutRecalculating(new DisplayedNOTGate(QPointF(2, 2), 1));
	appendNewDisplayedObject_WithoutRecalculating(new DisplayedNOTGate(QPointF(2, 2), 1));
	appendNewDisplayedObject_WithoutRecalculating(new DisplayedANDGate(QPointF(2, 4), 1));
	appendNewDisplayedObject_WithoutRecalculating(new DisplayedORGate(QPointF(2, 3), 1));
	appendNewDisplayedObject_WithoutRecalculating(new DisplayedXORGate(QPointF(2, 5), 1));

	QList<QPointF> nodes;
	nodes.append(QPointF(0, 0));
	nodes.append(QPointF(0, 2));
	nodes.append(QPointF(2, 2));
	nodes.append(QPointF(2, 0));
	for (int i = 0; i < 60; i++)
	{
		appendNewDisplayedObject_WithoutRecalculating(new DisplayedWire(nodes));
	}

	nodes.clear();
	nodes.append(QPointF(0, 4));
	nodes.append(QPointF(1, 5));
	nodes.append(QPointF(2, 4));
	for (int i = 0; i < 3; i++)
	{
		appendNewDisplayedObject_WithoutRecalculating(new DisplayedWire(nodes));
	}

	nodes.clear();
	nodes.append(QPointF(0, 3));
	nodes.append(QPointF(2, 3));
	for (int i = 0; i < 3; i++)
	{
		appendNewDisplayedObject_WithoutRecalculating(new DisplayedWire(nodes));
	}

	for (int i = 0; i < 4; i++)
	{
		appendNewDisplayedObject_WithoutRecalculating(new Switcher(QRectF(3, 0, 2, 1), 1));
		appendNewDisplayedObject_WithoutRecalculating(new Lamp(QRectF(3, 1, 1, 1), 1));
		appendNewDisplayedObject_WithoutRecalculating(new DisplayedNOTGate(QPointF(3, 2)));
		appendNewDisplayedObject_WithoutRecalculating(new DisplayedANDGate(QPointF(3, 3)));
		appendNewDisplayedObject_WithoutRecalculating(new DisplayedORGate(QPointF(3, 4)));
		appendNewDisplayedObject_WithoutRecalculating(new DisplayedXORGate(QPointF(3, 5)));
	}*/

	for (int i = 0; i < 60; i++)
	{
		appendNewDisplayedObject_WithoutRecalculating(new DisplayedORGate(QPointF(-3, -4 )));
	}

#pragma endregion

	coveredLogicArea.moveTopLeft(logicPos);
	change_dpi(dpi);
	if(size != QSize())
		changeArea(size);
}

Camera::~Camera()
{
	for (auto lst : activatedDisplayObjects)
	{
		qDeleteAll(lst);
	}
	qDeleteAll(inactivatedDisplayObjects);
}

void Camera::changeArea(const QSize & size)
{
	resolution = size;
	coveredLogicArea.setSize(QSizeF(size) / _dpi);
	recalculateObjectsOnScreen();
}

void Camera::paintImage(QPainter & painter)
{
	//painter.drawText(0, 0, 300, 300, 0, QString::number(activatedDisplayObjects.size())); 
	painter.setTransform(transform);
	painter.setRenderHint(QPainter::RenderHint::Antialiasing);
	//painter.drawRect(QRectF(QPointF(0, 0), coveredLogicArea.size()));
	QBrush brush(Qt::white);
	painter.setBrush(brush);
	QPen pen = QPen(Qt::PenStyle::SolidLine);
	pen.setWidthF(penLineWidth);
	painter.setPen(pen);
	
	QPointF currentDrawingPos = coveredLogicArea.topLeft();
	QPointF p;
	for (QMap<int, QList<DisplayedObject*>>::iterator listIt = activatedDisplayObjects.begin(); listIt != activatedDisplayObjects.end(); listIt++)
	{
		for (QList<DisplayedObject*>::iterator it = listIt->begin(); it != listIt->end(); it++)
		{
			p = (*it)->getPositionAndArea().topLeft();
			painter.translate(p - currentDrawingPos);
			(*it)->paint(painter);
			currentDrawingPos = p;
		}
	}
	//painter.drawEllipse(QPointF(2, 2) - coveredLogicArea.topLeft(), 1, 1);
}

void Camera::change_dpi(qreal value)
{
	_dpi = value;
	transform = QTransform();
	transform.scale(_dpi, _dpi);
	if (resolution != QSize())
	{
		coveredLogicArea.setSize(QSizeF(resolution) / _dpi);
		recalculateObjectsOnScreen();
	}
}

void Camera::onPressedMouseMove(const QPointF & screenPos)
{
	if (pressedObject != nullptr)
	{
		calculateCursorLogicPosition(screenPos);
		pressedObject->movePressedPoint(cursorLogicPoint);
	}
	else
	{
		coveredLogicArea.moveTopLeft(cursorLogicPoint - screenPos / _dpi);
		recalculateObjectsOnScreen();
	}
}

void Camera::onMouseDown(const QPointF & screenPos)
{
	calculateCursorLogicPosition(screenPos);
	checkCursorHitObjects();
}

void Camera::onMouseUp(const QPointF &screenPos)
{
	calculateCursorLogicPosition(screenPos);
	if (pressedObject != nullptr)
	{
		pressedObject->processMouseButtonUp(cursorLogicPoint);
	}
	pressedObject = nullptr;
}

QVector<DisplayedObject*> Camera::getAllObjectsContainPoint(QPointF logicPoint)
{
	QVector<DisplayedObject*> objects;
	QMap<int, QList<DisplayedObject*>>::iterator listIt = activatedDisplayObjects.end();
	auto begin = activatedDisplayObjects.begin();
	while (listIt != begin)
	{
		--listIt;
		QList<DisplayedObject*>::reverse_iterator it = listIt->rbegin();
		while (it != listIt->rend())
		{
			if ((*it)->processPoint(cursorLogicPoint))
			{
				objects.append(*it);
			}
			it++;
		}
	}
	return objects;
}

void Camera::appendNewDisplayedObject_WithoutRecalculating(DisplayedObject * obj)
{
	activatedDisplayObjects[obj->getLayer()].append(obj);
}

void Camera::recalculateObjectsOnScreen()
{
	int initialInactiveObjectsCount = inactivatedDisplayObjects.size();
	for (QMap<int, QList<DisplayedObject*>>::iterator listIt = activatedDisplayObjects.begin(); listIt != activatedDisplayObjects.end(); listIt++)
	{
		for (int i = 0; i < listIt->size(); i++)
		{
			if (!coveredLogicArea.intersects(((*listIt)[i])->getPositionAndArea()))
			{
				inactivatedDisplayObjects.append((*listIt)[i]);
				(*listIt).removeAt(i);
				i--;
			}
		}
	}
	
	int layer;
	for (int i = 0; i < initialInactiveObjectsCount; i++)
	{
		if (coveredLogicArea.intersects((inactivatedDisplayObjects[i])->getPositionAndArea()))
		{
			layer = inactivatedDisplayObjects[i]->getLayer();
			activatedDisplayObjects[layer].prepend(inactivatedDisplayObjects[i]);
			inactivatedDisplayObjects.removeOne(inactivatedDisplayObjects[i]);
			initialInactiveObjectsCount--;
			i--;
		}
	}
}

void Camera::checkCursorHitObjects()
{
	QMap<int, QList<DisplayedObject*>>::iterator listIt = activatedDisplayObjects.end();
	auto begin = activatedDisplayObjects.begin();
	while (listIt != begin)
	{
		--listIt;
		QList<DisplayedObject*>::reverse_iterator it = listIt->rbegin();
		while (it != listIt->rend())
		{
			if ((*it)->processPoint(cursorLogicPoint))
			{
				pressedObject = *it;
				(*listIt).move(listIt->indexOf(*it), listIt->size() - 1);
				return;
			}
			it++;
		}
	}
	pressedObject = nullptr;
}

void DisplayedObject::movePressedPoint(const QPointF & newLogicPressedPoint)
{
	positionAndArea.moveTopLeft(newLogicPressedPoint - pressedPointWithinObject);
}

const bool DisplayedObject::processPoint(const QPointF & point)
{
	bool result = simplifiedAreaContainsPoint(point);
	if(result)
		pressedPointWithinObject = point - positionAndArea.topLeft();
	return result;

}