#pragma once

#include <QFrame>

class zBgFrame : public QFrame
{
	Q_OBJECT

	QPixmap m_pixmap;
public:
	zBgFrame(QWidget *parent);
	~zBgFrame();

	void setPixmap(const QString& file);

	virtual void paintEvent(QPaintEvent * e);
};
