#include "zBgFrame.h"
#include <QPainter>

zBgFrame::zBgFrame(QWidget *parent)
	: QFrame(parent)
{

	setAttribute(Qt::WA_TranslucentBackground);
}

zBgFrame::~zBgFrame()
{
}

void zBgFrame::paintEvent(QPaintEvent * /*e*/)
{
	QPainter p(this);
	p.drawPixmap(0, 0, width(), height(), m_pixmap);
}

void zBgFrame::setPixmap(const QString& file)
{
	m_pixmap.load(file);	
}

