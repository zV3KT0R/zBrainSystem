#include "zBrainSystem.h"
#include <QtWidgets/QApplication>
#include <QSystemSemaphore>
#include <QSharedMemory>
#include <QMessageBox>
#include <QString>

#pragma comment(lib, "runtimeobject.lib")
//#pragma comment(lib, "widowsapi.lib")


const char g_szSemName[] = "-=<zBrainSystem semaphore>=-";
const char g_szSemName2[] = "123-=<zBrainSystem semaphore>=-321";

int main(int argc, char *argv[])
{
	QStringList paths = QCoreApplication::libraryPaths();
	paths.append(".");
	paths.append("imageformats");
	paths.append("platforms");
	paths.append("mediaservice");
	QCoreApplication::setLibraryPaths(paths);

	QApplication a(argc, argv);

	QSystemSemaphore semaphore(g_szSemName, 1);  // ������ �������
	semaphore.acquire(); // ��������� �������, �������� ������ ����������� �������� � ����������� �������

#ifndef Q_OS_WIN32
						 // � linux/unix ����������� ������ �� ������������� ��� ��������� ���������� ����������,
						 // ������� ���������� ���������� �� ������� ������
	QSharedMemory nix_fix_shared_memory(g_szSemName2);
	if (nix_fix_shared_memory.attach()) {
		nix_fix_shared_memory.detach();
	}
#endif

	QSharedMemory sharedMemory(g_szSemName2);  // ������ ��������� ����������� ������
	bool is_running;            // ���������� ��� �������� ���� ����������� ����������
	if (sharedMemory.attach())  // �������� ������������ ��������� ����������� ������
	{							// � ��� ������������� ��������								
		is_running = true;      // ���� �������, �� ����������, ��� ��� ���� ���������� ���������
	}
	else 
	{
		sharedMemory.create(1); // � ��������� ������ �������� 1 ���� ������
		is_running = false;     // � ����������, ��� ������ ����������� �� ��������
	}
	semaphore.release();        // �������� �������

								// ���� ��� ������� ���� ��������� ����������, �� �������� �� ���� ������������
								// � ��������� ������ �������� ���������� ����������
	if (is_running) 
	{
		QMessageBox msgBox;
		msgBox.setIcon(QMessageBox::Warning);
		msgBox.setText(QString::fromLocal8Bit("���������� ��� ��������.\n"
			"�� ������ ��������� ������ ���� ��������� ����������."));
		msgBox.exec();
		return 1;
	}

	zBrainSystem w;
	w.show();
	return a.exec();
}
