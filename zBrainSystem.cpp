#include "zBrainSystem.h"

#include <QSettings>
#include <QDir>
#include <QShortcut>
#include <QSignalMapper>
#include <QThread>
#include <QTimer>
#include <QDateTime>
#include <QMediaPlayer>
#include <QMediaPlaylist>

#include "zDlgSettings.h"
#include "zModels.h"

#ifdef WIN32
#include <Windows.h>
#elif WIN64
#include <Windows.h>
#else
void Beep(int, int) { qApp->beep(); }
#endif

const QString MSG_H_READY			=QString::fromLocal8Bit("Система готова");
const QString MSG_H_STARTED			=QString::fromLocal8Bit("Время запущено");
const QString MSG_STARTED			=QString::fromLocal8Bit("Ожидаю нажатия кнопки");
const QString MSG_H_FALSESTART		=QString::fromLocal8Bit("ФАЛЬСТАРТ");
const QString MSG_FALSESTART		=QString::fromLocal8Bit("%1 кнопка");
const QString MSG_H_BUTTON_PRESSED	=QString::fromLocal8Bit("Нажата кнопка");
const QString MSG_H_TIMEOUT			=QString::fromLocal8Bit("Время вышло");
const QString MSG_TIMEOUT			=QString::fromLocal8Bit("Ни одна из кнопок не была нажата");

QString s_exeLocation;

QSettings *g_pSettings = NULL;

zBrainSystem * zBrainSystem::m_pMainWindow = NULL;

zBrainSystem::zBrainSystem(QWidget *parent)
	: QMainWindow(parent)
	, m_dCurTime(0)
	, m_nCountdown(0)
	, m_bFullscreen(false)
	, m_nFalseStartCount(0)
{
	m_pMainWindow = this;

	// Define the base location - Should be ended with a slash!
	s_exeLocation = QCoreApplication::arguments()[0];
	int i = s_exeLocation.lastIndexOf(QDir::separator());
	if (i == -1)
		s_exeLocation = "";
	else
		s_exeLocation = s_exeLocation.left(i + 1);

	QCoreApplication::setOrganizationName(QLatin1String("zBrainSystem"));
	QCoreApplication::setApplicationName(QLatin1String("zBrainSystem"));
	QCoreApplication::setApplicationVersion(QLatin1String("0.0.1"));

	g_pSettings = new QSettings(s_exeLocation + tr("zBrainSystem.ini"), QSettings::IniFormat, this);

	m_pTimer = new QTimer(this);
	connect(m_pTimer, SIGNAL(timeout()), this, SLOT(onTimer()));

	m_pPlayer = new QMediaPlayer(this);
	m_pPlaylist = new QMediaPlaylist(m_pPlayer);
	m_pPlayer->setPlaylist(m_pPlaylist);
	m_pPlaylist->setPlaybackMode(QMediaPlaylist::PlaybackMode::CurrentItemOnce);
	m_pPlayer->setVolume(100);

	loadSettings();

	ui.setupUi(this);
	
	ui.m_fBgColor->setPixmap(m_Settings.m_strBackground);

	m_pSignalMapper = new QSignalMapper(this);

	if (!registerButtons())
		exit(0);

	connect(m_pSignalMapper, SIGNAL(mapped(int)), this, SLOT(onCommandButton(int)));
	onReset();

	playSound(zSound::Stub);

	ui.m_lHeader->hide();

	m_pEscShortcut = new QShortcut(Qt::Key_Escape, this);
	m_pStartShortcut = new QShortcut(QKeySequence::fromString(m_Settings.m_strStartKey), this);
	m_pStopShortcut = new QShortcut(QKeySequence::fromString(m_Settings.m_strStopKey), this);

	
	connect(m_pEscShortcut, SIGNAL(activated()), this, SLOT(onNormal()));
	connect(m_pStartShortcut, SIGNAL(activated()), this, SLOT(onStart()));
	connect(m_pStopShortcut, SIGNAL(activated()), this, SLOT(onReset()));

	m_pTimer->start(100);

}
zBrainSystem::~zBrainSystem()
{
	qDeleteAll(m_lstKeys);
	m_lstKeys.clear();
}

QSettings * zBrainSystem::settings()
{
	return g_pSettings;
}

zBrainSystem * zBrainSystem::instance()
{
	return m_pMainWindow;
}
void zBrainSystem::onSettings()
{
	zDlgSettings dlg(this);
	
	if (dlg.exec() == QDialog::Accepted)
	{
		loadSettings();
		registerButtons();
		ui.m_fBgColor->setPixmap(m_Settings.m_strBackground);

		if (!m_Settings.m_strStartKey.isEmpty())
			m_pStartShortcut->setKey(QKeySequence::fromString(m_Settings.m_strStartKey));

		if (!m_Settings.m_strStopKey.isEmpty())
			m_pStopShortcut->setKey(QKeySequence::fromString(m_Settings.m_strStopKey));

	}
}

void zBrainSystem::onNormal()
{
	ui.menuBar->show();
	ui.mainToolBar->show();
	showNormal();

	m_bFullscreen = false;
}

void zBrainSystem::onFullscreen()
{
	if (!m_bFullscreen)
	{
		showFullScreen();
		
		ui.menuBar->hide();
		ui.mainToolBar->hide();
					
		m_bFullscreen = true;
	}
	else
		onNormal();
}
void zBrainSystem::loadSettings()
{
	g_pSettings->beginGroup(QLatin1String("zBrainSystem"));
	if (g_pSettings->value(QLatin1String("Type"), 1).toInt() == 1)
		m_Settings.m_nGameType = zGameType::Brain;
	else
		m_Settings.m_nGameType = zGameType::Svoja;

	m_Settings.m_strBackground = g_pSettings->value(QLatin1String("Background"), "").toString();

	m_Settings.m_strStartKey = g_pSettings->value(tr("StartKey"), "").toString();
	m_Settings.m_strStopKey  = g_pSettings->value(tr("StopKey"), "").toString();

	g_pSettings->endGroup();

	g_pSettings->beginGroup(QLatin1String("Sound"));
	if (g_pSettings->value(QLatin1String("Type"), 1).toInt() == 1)
		m_Settings.m_nSoundType = zSoundType::PcSpeaker;
	else
		m_Settings.m_nSoundType = zSoundType::SoundCard;
	g_pSettings->endGroup();

	g_pSettings->beginGroup(QLatin1String("Brain"));
	m_Settings.m_nBrainTimeout = g_pSettings->value(QLatin1String("BrainTimeout"), 60).toInt();
	m_Settings.m_nErrBrainTimeout1 = g_pSettings->value(QLatin1String("ErrBrainTimeout1"), 20).toInt();
	m_Settings.m_nErrBrainTimeout2 = g_pSettings->value(QLatin1String("ErrBrainTimeout2"), 10).toInt();
	m_Settings.m_nErrBrainTimeout3 = g_pSettings->value(QLatin1String("ErrBrainTimeout3"), 5).toInt();
	m_Settings.m_nErrBrainTimeout4 = g_pSettings->value(QLatin1String("ErrBrainTimeout4"), 5).toInt();
	m_Settings.m_nErrBrainTimeout5 = g_pSettings->value(QLatin1String("ErrBrainTimeout5"), 5).toInt();
	m_Settings.m_nErrBrainTimeout6 = g_pSettings->value(QLatin1String("ErrBrainTimeout6"), 5).toInt();
	m_Settings.m_nErrBrainTimeout7 = g_pSettings->value(QLatin1String("ErrBrainTimeout7"), 5).toInt();
	m_Settings.m_nErrBrainTimeout  = g_pSettings->value(QLatin1String("ErrBrainTimeout"), 5).toInt();
	g_pSettings->endGroup();

	g_pSettings->beginGroup(QLatin1String("Sound"));
	m_Settings.m_nSvojakTimeout = g_pSettings->value(QLatin1String("SvojakTimeout"), 60).toInt();
	m_Settings.m_bSvojakFalseStart = g_pSettings->value(QLatin1String("SvojakFalseStart"), false).toBool();

	m_pPlaylist->clear();

	m_pPlaylist->addMedia(QUrl::fromLocalFile(g_pSettings->value(QLatin1String("Stub"), (s_exeLocation + "stub.wav")).toString()));
	m_pPlaylist->addMedia(QUrl::fromLocalFile(g_pSettings->value(QLatin1String("Start"), (s_exeLocation + "start.wav")).toString()));
	m_pPlaylist->addMedia(QUrl::fromLocalFile(g_pSettings->value(QLatin1String("FalseStart"), (s_exeLocation + "falsestart.wav")).toString()));
	m_pPlaylist->addMedia(QUrl::fromLocalFile(g_pSettings->value(QLatin1String("Button"), (s_exeLocation + "button.wav")).toString()));
	m_pPlaylist->addMedia(QUrl::fromLocalFile(g_pSettings->value(QLatin1String("Timeout"), (s_exeLocation + "timeout.wav")).toString()));

	g_pSettings->endGroup();

	qDeleteAll(m_lstCommands);
	m_lstCommands.clear();

	g_pSettings->beginGroup(QLatin1String("Commands"));

	int count = g_pSettings->value(QLatin1String("count"), 0).toInt();

	for (int i = 0; i < count; i++)
	{
		zCommandItem cmd;
		cmd.m_iId = i;
		cmd.m_strName = g_pSettings->value(tr("Name_%1").arg(i), "").toString();
		cmd.m_Color = g_pSettings->value(tr("Color_%1").arg(i), 0).toUInt();
		cmd.m_ksButton = QKeySequence::fromString(g_pSettings->value(tr("Button_%1").arg(i), "").toString());

		m_lstCommands.append(new zCommandItem(cmd));
	}

	g_pSettings->endGroup();
}

bool zBrainSystem::registerButtons()
{
	m_pSignalMapper->removeMappings(nullptr);

	qDeleteAll(m_lstKeys);
	m_lstKeys.clear();

	for (int i = 0; i < m_lstCommands.count(); i++)
	{
		zCommandItem * cmd = m_lstCommands.at(i);
		QShortcut *shortcut = new QShortcut(cmd->m_ksButton, this);
		m_lstKeys.append(shortcut);
	//	shortcut->setContext(Qt::ApplicationShortcut);
		connect(shortcut, SIGNAL(activated()), m_pSignalMapper, SLOT(map()));
		m_pSignalMapper->setMapping(shortcut, i);
	}
	
	return true;

}
void zBrainSystem::onCommandButton(int ind)
{
	zCommandItem * cmd = m_lstCommands.at(ind);

	if (m_nState == zGameState::Finished)
		return;

	if (m_nCountdown > 0)
		return;
	else
		m_nCountdown = 10 * 2;

	if (m_setPressedBtns.find(ind) != m_setPressedBtns.end())
		return;
	
	m_setPressedBtns.insert(ind);

	

	if (m_Settings.m_nGameType == zGameType::Brain)
	{
		if (m_nState == zGameState::Started)
		{
			pressed(cmd);
			m_nState = zGameState::Paused;
		}
		else if (m_nState == zGameState::Ready || m_nState == zGameState::Paused)
		{		
			ui.m_lHeader->setText(QString::fromLocal8Bit("%1").arg(MSG_H_FALSESTART));
			ui.m_lHeader->show();
			ui.m_lCommandName->setText(QString::fromLocal8Bit("%1").arg(cmd->m_strName));

			m_nFalseStartCount++;

			playSound(zSound::FalseStart);
			m_nState = zGameState::Paused;
		}

		switch (m_setPressedBtns.count() - m_nFalseStartCount)
		{
		case 0:
			break;
		case 1:
			setTime(m_Settings.m_nBrainTimeout - m_Settings.m_nErrBrainTimeout1);
			break;
		case 2:
			setTime(m_Settings.m_nBrainTimeout - m_Settings.m_nErrBrainTimeout2);
			break;
		case 3:
			setTime(m_Settings.m_nBrainTimeout - m_Settings.m_nErrBrainTimeout3);
			break;
		case 4:
			setTime(m_Settings.m_nBrainTimeout - m_Settings.m_nErrBrainTimeout4);
			break;
		case 5:
			setTime(m_Settings.m_nBrainTimeout - m_Settings.m_nErrBrainTimeout5);
			break;
		case 6:
			setTime(m_Settings.m_nBrainTimeout - m_Settings.m_nErrBrainTimeout6);
			break;
		case 7:
			setTime(m_Settings.m_nBrainTimeout - m_Settings.m_nErrBrainTimeout7);
			break;
		default:
			setTime(m_Settings.m_nBrainTimeout - m_Settings.m_nErrBrainTimeout);
			break;
		}

		if (m_setPressedBtns.count() >= m_lstCommands.count())
			setTime(m_Settings.m_nBrainTimeout);

	}
	else
	{
		if (m_nState == zGameState::Started || (!m_Settings.m_bSvojakFalseStart))
		{
			pressed(cmd);
			m_nState = zGameState::Paused;
		}
	}
}
void zBrainSystem::pressed(const zCommandItem * cmd)
{
	ui.m_lCommandName->setText(QString::fromLocal8Bit("%1").arg(cmd->m_strName));
/*	QPixmap p(24, 24);
	p.fill(cmd->m_Color);
	ui.m_lColor->setPixmap(p);
*/
	playSound(zSound::BtnPressed); 

}
void zBrainSystem::onStart()
{
	if (m_nCountdown > 0)
		return;

	if (m_nState == zGameState::Finished || m_nState == zGameState::Started) return;

	playSound(zSound::Start);
	
	m_nCountdown = 0;

	ui.m_lHeader->hide();
	ui.m_lCommandName->setText(MSG_H_STARTED);
	m_nState = zGameState::Started;

	m_i64StartTime = QDateTime::currentMSecsSinceEpoch();	
}
void zBrainSystem::onReset()
{
	m_nCountdown = 0;
	m_nFalseStartCount = 0;

	ui.m_lHeader->hide();

	m_nState = zGameState::Ready;

	ui.m_lCommandName->setText(MSG_H_READY);
	
	m_setPressedBtns.clear();

	m_pPlayer->stop();

	setTime(0);
}

void zBrainSystem::setTime(double time)
{
	double tmp = 0;
	if (m_Settings.m_nGameType == zGameType::Brain)
	{
		tmp = m_Settings.m_nBrainTimeout;
	}
	else
	{
		tmp = m_Settings.m_nSvojakTimeout;
	}

	ui.m_pbTime->setMaximum(tmp);

	m_dCurTime = time;
	ui.m_pbTime->setValue(m_dCurTime);
	
	ui.m_lTime->setText(QString("%1").arg(m_dCurTime, 0, 'f', 1));
	ui.m_lRemaningTime->setText(QString("%1").arg(tmp - m_dCurTime, 0, 'f', 1));

}

void zBrainSystem::playSound(zSound nSound)
{
	if (m_Settings.m_nSoundType == zSoundType::PcSpeaker)
	{
		switch (nSound)
		{
		case zSound::Start:
		{
			Beep(2280, 500);
			break;
		}
		case zSound::FalseStart:
		{
			Beep(2380, 200);
			QThread::msleep(20);
			Beep(2380, 200);
			QThread::msleep(20);
			Beep(2380, 200);
			break;
		}
		case zSound::BtnPressed:
		{
			Beep(2530, 1000);
			break;
		}
		case zSound::Timeout:
		{
			Beep(2430, 1000);
			break;
		}
		}
	}
	else
	{
		m_pPlaylist->setCurrentIndex((int)nSound);
		m_pPlayer->play();
	}
}

void zBrainSystem::onTimer()
{
	if (m_nCountdown>0) 
		m_nCountdown--;

	if (m_nState == zGameState::Started)
	{
		double dms = QDateTime::currentMSecsSinceEpoch() - m_i64StartTime;
		m_i64StartTime = QDateTime::currentMSecsSinceEpoch();

		setTime(m_dCurTime + dms/1000);

		if (m_Settings.m_nGameType == zGameType::Brain)
		{
			if (m_dCurTime >= m_Settings.m_nBrainTimeout)
			{
				m_nState = zGameState::Finished;
				ui.m_lCommandName->setText(MSG_H_TIMEOUT);
				playSound(zSound::Timeout);
				setTime(m_Settings.m_nBrainTimeout);
			}
		}
		else 
		{
			if (m_dCurTime >= m_Settings.m_nSvojakTimeout)
			{
				m_nState = zGameState::Finished;
				ui.m_lCommandName->setText(MSG_H_TIMEOUT);
				playSound(zSound::Timeout);
				setTime(m_Settings.m_nSvojakTimeout);
			}
		}
	}
}