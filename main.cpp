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

	QSystemSemaphore semaphore(g_szSemName, 1);  // создаём семафор
	semaphore.acquire(); // Поднимаем семафор, запрещая другим экземплярам работать с разделяемой памятью

#ifndef Q_OS_WIN32
						 // в linux/unix разделяемая память не освобождается при аварийном завершении приложения,
						 // поэтому необходимо избавиться от данного мусора
	QSharedMemory nix_fix_shared_memory(g_szSemName2);
	if (nix_fix_shared_memory.attach()) {
		nix_fix_shared_memory.detach();
	}
#endif

	QSharedMemory sharedMemory(g_szSemName2);  // Создаём экземпляр разделяемой памяти
	bool is_running;            // переменную для проверки ууже запущенного приложения
	if (sharedMemory.attach())  // пытаемся присоединить экземпляр разделяемой памяти
	{							// к уже существующему сегменту								
		is_running = true;      // Если успешно, то определяем, что уже есть запущенный экземпляр
	}
	else 
	{
		sharedMemory.create(1); // В противном случае выделяем 1 байт памяти
		is_running = false;     // И определяем, что других экземпляров не запущено
	}
	semaphore.release();        // Опускаем семафор

								// Если уже запущен один экземпляр приложения, то сообщаем об этом пользователю
								// и завершаем работу текущего экземпляра приложения
	if (is_running) 
	{
		QMessageBox msgBox;
		msgBox.setIcon(QMessageBox::Warning);
		msgBox.setText(QString::fromLocal8Bit("Приложение уже запущено.\n"
			"Вы можете запустить только один экземпляр приложения."));
		msgBox.exec();
		return 1;
	}

	zBrainSystem w;
	w.show();
	return a.exec();
}
