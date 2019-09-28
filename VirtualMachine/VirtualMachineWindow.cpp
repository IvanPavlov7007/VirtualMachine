//by Ivan Pavlov

#include <QPainter>
#include <QMouseEvent>
#include "VirtualMachineWindow.h"

VirtualMachineWindow::VirtualMachineWindow(QWidget *parent)
	:QWidget(parent)
{
	//setAttribute(Qt::WA_StaticContents);
	camera = new Camera(60,QSize(),QPointF(-1,-2));
	resize(480, 660);
}

VirtualMachineWindow::~VirtualMachineWindow()
{
	delete camera;
}

void VirtualMachineWindow::mousePressEvent(QMouseEvent * event)
{
	if (event->button() & Qt::MouseButton::LeftButton)
	{
		camera->onMouseDown(event->localPos());
	}
	QWidget::mousePressEvent(event);
}

void VirtualMachineWindow::mouseMoveEvent(QMouseEvent * event)
{
	if (event->buttons() & Qt::MouseButton::LeftButton)
	{
		camera->onPressedMouseMove(event->localPos());
		update();
	}
	QWidget::mouseMoveEvent(event);
}

void VirtualMachineWindow::mouseReleaseEvent(QMouseEvent * event)
{
	if (event->button() & Qt::MouseButton::LeftButton)
	{
		camera->onMouseUp(event->localPos());
		update();
	}
	QWidget::mouseReleaseEvent(event);
}

void VirtualMachineWindow::paintEvent(QPaintEvent * event)
{
	QPainter painter(this);
	camera->paintImage(painter);
}

void VirtualMachineWindow::resizeEvent(QResizeEvent * event)
{
	camera->changeArea(event->size());
	QWidget::resizeEvent(event);
}

void VirtualMachineWindow::keyPressEvent(QKeyEvent * event)
{
	static qreal dpi = 60;
	if (event->key() == Qt::Key_Plus)
	{
		dpi += 5;
		camera->change_dpi(dpi);
		update();
	}
	else if (event->key() == Qt::Key_Minus)
	{
		dpi = std::max(dpi - 5, 5.);
		camera->change_dpi(dpi);
		update();
	}
	else
		QWidget::keyPressEvent(event);
}
