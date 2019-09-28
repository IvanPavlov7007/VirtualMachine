//by Ivan Pavlov

#pragma once
#include <QPainter>
#include <QMap>

//Preferences:
const qreal penLineWidth = 0.05;
const int displayedObjectContainingCellSide = 1;

class DisplayedWire;//external DisplayedObject

class DisplayedObject
{
public:
	virtual void paint(QPainter &painter) = 0;// paint always from (0,0) in top-left corner of the object
	virtual const QRectF getPositionAndArea() const { return positionAndArea; }
	virtual void movePressedPoint(const QPointF & newLogicPressedPoint); // pressedPointWithinObject will matsch newLogicPoint in terms of world space. Within the object it remains the same
	const int getLayer() const { return _drawingLayer; }
	virtual const bool simplifiedAreaContainsPoint(const QPointF &point) { return positionAndArea.contains(point); }//not necessary simplified, only in case of wires
	virtual const bool processPoint(const QPointF &point);
	virtual const bool processMouseButtonUpPoint(const QPointF &point) { return false; }
	virtual void processMouseButtonUp(const QPointF &p) {};
	virtual void processWire(DisplayedWire *wire,int nodeIndex) {};//calls just right after processPoint was called so it doesnt have to calculate pressedPointWithinObject again
protected:
	DisplayedObject(const QRectF &rect, int drawingLayer = 0) {
		positionAndArea = rect; _drawingLayer = drawingLayer; pressedPointWithinObject = QPointF();
	}
	DisplayedObject(int drawingLayer = 0) { _drawingLayer = drawingLayer; pressedPointWithinObject = QPointF();}

	QPointF pressedPointWithinObject;
	QRectF positionAndArea;
	int _drawingLayer;
};

struct DObjectCointainingCell
{
	QList<DisplayedObject*> containedDObjects;
};

class Camera
{
public:
	Camera(qreal dpi, const QSize& size = QSize(), const QPointF &logicPos = QPointF());
	~Camera();
	DisplayedObject* getPressedObject() { return pressedObject; }
	void changeArea(const QSize& size);
	void paintImage(QPainter &painter);
	void change_dpi(qreal value);
	void onPressedMouseMove(const QPointF &screenPos);
	void onMouseDown(const QPointF &screenPos);
	void onMouseUp(const QPointF &screenPos);

	static Camera & getInstance() { return *_instance; }
	QVector<DisplayedObject*> getAllObjectsContainPoint(QPointF logicPoint);
private:
	static Camera* _instance;

	void calculateCursorLogicPosition(const QPointF &screenPos) { cursorLogicPoint = screenPos / _dpi + coveredLogicArea.topLeft(); }
	void recalculateObjectsOnScreen();

	void appendNewDisplayedObject_WithoutRecalculating(DisplayedObject* obj);

	QMap<QPoint, DObjectCointainingCell> dObjectsCellsGrid;

	QRectF coveredLogicArea;
	QSize resolution;
	QTransform transform;
	qreal _dpi;
	QMap<int,QList<DisplayedObject*>> activatedDisplayObjects;
	QList<DisplayedObject*> inactivatedDisplayObjects;

	inline void checkCursorHitObjects();

	DisplayedObject* pressedObject = nullptr;
	QPointF cursorLogicPoint;
};


