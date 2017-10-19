#include "zDlgCommand.h"

#include <QColorDialog>
#include <QPixmap>
#include <QDialogButtonBox>

zDlgCommand::zDlgCommand(QWidget *parent, zCommandItem * p)
	: QDialog(parent)
{
	ui.setupUi(this);

	if (p)
	{
		m_Cmd = *p;
	}

	ui.m_ksButton->setKeySequence(m_Cmd.m_ksButton);
	ui.m_leName->setText(m_Cmd.m_strName);
	if (m_Cmd.m_Color.isValid())
	{
		QPixmap p(24, 24);
		p.fill(m_Cmd.m_Color);
		ui.m_lCmdColor->setPixmap(p);
	}

	updateButtons();
}

zDlgCommand::~zDlgCommand()
{
}

void zDlgCommand::updateButtons()
{
	ui.m_bbOkCancel->button(QDialogButtonBox::Ok)->setEnabled(
		!ui.m_leName->text().isEmpty() &&
		!ui.m_ksButton->keySequence().isEmpty() &&
		m_Cmd.m_Color.isValid()
	);
}

void zDlgCommand::onSelectColor()
{
	QColor color = QColorDialog::getColor(m_Cmd.m_Color);
	if (!color.isValid()) 
	{
		return;
	}
	m_Cmd.m_Color = color;
	QPixmap p(24, 24);
	p.fill(color);
	ui.m_lCmdColor->setPixmap(p);
	updateButtons();
}

zCommandItem zDlgCommand::command()
{ 
	m_Cmd.m_strName = ui.m_leName->text();
	m_Cmd.m_ksButton = ui.m_ksButton->keySequence();
	return m_Cmd; 
}