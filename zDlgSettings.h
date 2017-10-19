#pragma once

#include <QDialog>
#include <QPointer>
#include "ui_zDlgSettings.h"

class zCommandModel;
class zDlgSettings : public QDialog
{
	Q_OBJECT

public:
	zDlgSettings(QWidget *parent = Q_NULLPTR);
	~zDlgSettings();

private slots:
	void addCommand();
	void editCommand();
	void deleteCommand();
	void clearCommands();

	void updateButtons();
	void save();
	void load();

	void onBrowseBackground();
private:

	QPointer<zCommandModel>  m_spModel;

	Ui::zDlgSettings ui;
};
