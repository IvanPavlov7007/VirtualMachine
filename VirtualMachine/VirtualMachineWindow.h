//by Ivan Pavlov

#pragma once
#include <QWidget>
#include "viewenvironment.h"

class VirtualMachineWindow : public QWidget
{
	Q_OBJECT
public:
	VirtualMachineWindow(QWidget *parent = 0);
	~VirtualMachineWindow();
protected:
	void mousePressEvent(QMouseEvent *event) override;
	void mouseMoveEvent(QMouseEvent *event) override;
	void mouseReleaseEvent(QMouseEvent *event) override;
	void paintEvent(QPaintEvent *event) override;
	void resizeEvent(QResizeEvent *event) override;
	void keyPressEvent(QKeyEvent * event) override;
private:
	Camera *camera;
};