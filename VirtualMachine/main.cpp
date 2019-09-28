//by Ivan Pavlov

#include <QApplication>
#include "VirtualMachineWindow.h"


int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	VirtualMachineWindow *virtualMachine = new VirtualMachineWindow;
	virtualMachine->show();
	return a.exec();
}
