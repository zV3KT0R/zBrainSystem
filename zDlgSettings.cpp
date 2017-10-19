#include "zDlgSettings.h"

#include "zBrainSystem.h"
#include <QSettings>

#include <QFileDialog>

#include "zModels.h"

zDlgSettings::zDlgSettings(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	m_spModel = new zCommandModel(this);
	
	load();

	ui.m_tvCommands->setModel(m_spModel);

	ui.m_tvCommands->setColumnWidth(0, 40);
	ui.m_tvCommands->setColumnWidth(1, 60);
	ui.m_tvCommands->setColumnWidth(2, 60);
	ui.m_tvCommands->setColumnWidth(3, 170);

	ui.m_tvSettings->setCurrentIndex(0);

	updateButtons();
}

zDlgSettings::~zDlgSettings()
{
}

void zDlgSettings::addCommand()
{
	m_spModel->addItem();
	updateButtons();
}
void zDlgSettings::editCommand()
{
	QModelIndex ind = ui.m_tvCommands->currentIndex();

	if (ind.isValid())
		m_spModel->editItem(ind.row());
	else
		m_spModel->addItem();

	updateButtons();
}
void zDlgSettings::deleteCommand()
{
	QModelIndex ind = ui.m_tvCommands->currentIndex();

	if (ind.isValid())
		m_spModel->deleteItem(ind.row());
	updateButtons();
}

void zDlgSettings::updateButtons()
{
	QModelIndex ind = ui.m_tvCommands->currentIndex();

	ui.m_btnEdit->setEnabled(ind.isValid());
	ui.m_btnDelete->setEnabled(ind.isValid());
	ui.m_btnClear->setEnabled(m_spModel->rowCount());
}

void zDlgSettings::clearCommands()
{
	m_spModel->clear();
	updateButtons();
}

void zDlgSettings::load()
{
	QSettings * settings = zBrainSystem::settings();

	settings->beginGroup(QLatin1String("zBrainSystem"));
	if (settings->value(QLatin1String("Type"), 1).toInt() == 1)
		ui.m_rbBrainRing->setChecked(true);
	else
		ui.m_rbSvojaIgra->setChecked(true);

	ui.m_leBackground->setText(settings->value(QLatin1String("Background"), "").toString());

	ui.m_ksStart->setKeySequence(QKeySequence::fromString(settings->value(tr("StartKey"), "").toString()));
	ui.m_ksStop ->setKeySequence(QKeySequence::fromString(settings->value(tr("StopKey"), "").toString()));


	settings->endGroup();

	settings->beginGroup(QLatin1String("Sound"));
	if (settings->value(QLatin1String("Type"), 1).toInt() == 1)
		ui.m_rbPcSpeacker->setChecked(true);
	else
		ui.m_rbSoundCard->setChecked(true);
	settings->endGroup();

	settings->beginGroup(QLatin1String("Brain"));
	ui.m_sbBrainMainTime->setValue(settings->value(QLatin1String("BrainTimeout"), 60).toInt());
	ui.m_sbBrainTime1->setValue(settings->value(QLatin1String("ErrBrainTimeout1"), 20).toInt());
	ui.m_sbBrainTime2->setValue(settings->value(QLatin1String("ErrBrainTimeout2"), 10).toInt());
	ui.m_sbBrainTime3->setValue(settings->value(QLatin1String("ErrBrainTimeout3"), 5).toInt());
	ui.m_sbBrainTime4->setValue(settings->value(QLatin1String("ErrBrainTimeout4"), 5).toInt());
	ui.m_sbBrainTime5->setValue(settings->value(QLatin1String("ErrBrainTimeout5"), 5).toInt());
	ui.m_sbBrainTime6->setValue(settings->value(QLatin1String("ErrBrainTimeout6"), 5).toInt());
	ui.m_sbBrainTime7->setValue(settings->value(QLatin1String("ErrBrainTimeout7"), 5).toInt());
	ui.m_sbBrainTime ->setValue(settings->value(QLatin1String("ErrBrainTimeout"), 5).toInt());
	settings->endGroup();

	settings->beginGroup(QLatin1String("Sound"));
	ui.m_sbSvoyMainTime->setValue(settings->value(QLatin1String("SvojakTimeout"), 60).toInt());
	ui.m_cbFalseStart->setChecked(settings->value(QLatin1String("SvojakFalseStart"), false).toBool());
	settings->endGroup();

	m_spModel->load();

}
void zDlgSettings::onBrowseBackground()
{
	QString file = QFileDialog::getOpenFileName(this,
		QString::fromLocal8Bit("Выберите файл подложки"));

	if (!file.isEmpty())
		ui.m_leBackground->setText(file);
}
void zDlgSettings::save()
{
	QSettings * settings = zBrainSystem::settings();

	settings->beginGroup(QLatin1String("zBrainSystem"));
	if (ui.m_rbBrainRing->isChecked())
		settings->setValue(QLatin1String("Type"), 1);
	else
		settings->setValue(QLatin1String("Type"), 2);

	settings->setValue(QLatin1String("Background"), ui.m_leBackground->text());

	settings->setValue(QLatin1String("StartKey"), ui.m_ksStart->keySequence().toString());
	settings->setValue(QLatin1String("StopKey"), ui.m_ksStop->keySequence().toString());

	settings->endGroup();

	settings->beginGroup(QLatin1String("Sound"));
	if (ui.m_rbPcSpeacker->isChecked())
		settings->setValue(QLatin1String("Type"), 1);
	else
		settings->setValue(QLatin1String("Type"), 2);
	settings->endGroup();

	settings->beginGroup(QLatin1String("Brain"));
	settings->setValue(QLatin1String("BrainTimeout"), ui.m_sbBrainMainTime->value());
	settings->setValue(QLatin1String("ErrBrainTimeout1"), ui.m_sbBrainTime1->value());
	settings->setValue(QLatin1String("ErrBrainTimeout2"), ui.m_sbBrainTime2->value());
	settings->setValue(QLatin1String("ErrBrainTimeout3"), ui.m_sbBrainTime3->value());
	settings->setValue(QLatin1String("ErrBrainTimeout4"), ui.m_sbBrainTime4->value());
	settings->setValue(QLatin1String("ErrBrainTimeout5"), ui.m_sbBrainTime5->value());
	settings->setValue(QLatin1String("ErrBrainTimeout6"), ui.m_sbBrainTime6->value());
	settings->setValue(QLatin1String("ErrBrainTimeout7"), ui.m_sbBrainTime7->value());
	settings->setValue(QLatin1String("ErrBrainTimeout" ), ui.m_sbBrainTime ->value());
	settings->endGroup();

	settings->beginGroup(QLatin1String("Sound"));
	settings->setValue(QLatin1String("SvojakTimeout"), ui.m_sbSvoyMainTime->value());
	settings->setValue(QLatin1String("SvojakFalseStart"), ui.m_cbFalseStart->isChecked());
	settings->endGroup();
		
	m_spModel->save();
}