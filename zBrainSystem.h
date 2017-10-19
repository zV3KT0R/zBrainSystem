#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_zBrainSystem.h"

class QSettings;

enum zGameType 
{
	InvalidGameType = 0,
	Brain		= 1,
	Svoja		= 2
};
enum zSoundType
{
	InvalidSoundType = 0,
	PcSpeaker	= 1,
	SoundCard	= 2
};

enum zGameState 
{
	Ready = 1,
	Started,
	Paused,
	Finished
};

enum zSound
{
	Stub =	0,
	Start,	
	FalseStart,
	BtnPressed,
	Timeout
};

/*


#define GAME_BRAIN 0
#define GAME_SVOJAK 1



*/
struct zSettings
{
	
	zGameType	m_nGameType;			
	zSoundType	m_nSoundType;		

	int m_nBrainTimeout;
	int m_nErrBrainTimeout1;
	int m_nErrBrainTimeout2;
	int m_nErrBrainTimeout3;
	int m_nErrBrainTimeout4;
	int m_nErrBrainTimeout5;
	int m_nErrBrainTimeout6;
	int m_nErrBrainTimeout7;
	int m_nErrBrainTimeout;

	int  m_nSvojakTimeout;
	bool m_bSvojakFalseStart;

	QString m_strBackground;
	QString m_strStartKey;
	QString m_strStopKey;

	zSettings()
	{
		m_nGameType = zGameType::Brain;
		m_nSoundType = zSoundType::PcSpeaker;
		m_nBrainTimeout = 60;
		m_nErrBrainTimeout1 = 20;
		m_nErrBrainTimeout2 = 10;
		m_nErrBrainTimeout3 = 5;
		m_nErrBrainTimeout4 = 5;
		m_nErrBrainTimeout5 = 5;
		m_nErrBrainTimeout6 = 5;
		m_nErrBrainTimeout7 = 5;
		m_nErrBrainTimeout  = 0;

		m_nSvojakTimeout = 10;
		m_bSvojakFalseStart = false;
	}
};

#define zMainWnd zBrainSystem::instance()

class zCommandItem;
class QSignalMapper;
class QTimer;
class QMediaPlayer;
class QMediaPlaylist;
class QShortcut;
class zBrainSystem : public QMainWindow
{
	Q_OBJECT

public:
	static QSettings	* settings();
	static zBrainSystem	* instance();

	zBrainSystem(QWidget *parent = Q_NULLPTR);
	virtual ~zBrainSystem();

private slots:
	void onSettings();
	void onStart();
	void onReset();
	void onCommandButton(int);
	void playSound(zSound nSound);
	void onTimer();
	void onFullscreen();
	void onNormal();
private:
	Ui::zBrainSystemClass ui;

	QShortcut *m_pEscShortcut;
	QShortcut *m_pStartShortcut;
	QShortcut *m_pStopShortcut;

	void loadSettings();
	bool registerButtons();
	void setTime(double time);
	void pressed(const zCommandItem * cmd);

	static zBrainSystem *  m_pMainWindow;
	zSettings m_Settings;

	QSignalMapper	*		m_pSignalMapper;
	QTimer			*		m_pTimer;
	QMediaPlayer	*		m_pPlayer;
	QMediaPlaylist	*		m_pPlaylist;

	QList<zCommandItem * >	m_lstCommands;
	QList< QShortcut* >		m_lstKeys;

	zGameState				m_nState;
	double					m_dCurTime;

	int						m_nCountdown;

	QSet<int>				m_setPressedBtns;

	qint64					m_i64StartTime;

	bool					m_bFullscreen;
	int						m_nFalseStartCount;
};
