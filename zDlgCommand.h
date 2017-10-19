#pragma once

#include <QDialog>
#include "ui_zDlgCommand.h"

#include "zModels.h"

class zDlgCommand : public QDialog
{
	Q_OBJECT

public:
	zDlgCommand(QWidget *parent = Q_NULLPTR, zCommandItem * p = nullptr);
	~zDlgCommand();

	zCommandItem command();
private slots:
	void updateButtons();
	void onSelectColor();
private:
	Ui::zDlgCommand ui;

	zCommandItem m_Cmd;
};
