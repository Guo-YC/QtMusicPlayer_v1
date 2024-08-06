#ifndef __K_MEDIAPLAYERMANAGER_H_
#define __K_MEDIAPLAYERMANAGER_H_

#include <QObject>

class QMediaPlayer;
class QMediaPlaylist;
class KMediaPlayerManager : public QObject
{
	Q_OBJECT
signals:
	void progressChanged(qint64 position, qint64 duration);
public:
	KMediaPlayerManager(QObject* parent = nullptr);
	~KMediaPlayerManager();
	void setCurrentIndex(int index);
	void setPlayList(QMediaPlaylist* other);	
	void pause();
	bool isStoppedState();
	bool isPlayingState();
	void showState();
	void check();
	void resume();
	void resume(int index, QString localPath);
	void stop();
	void showPlayList();
	void setPlayList(QString mode);
	void next();
	void previous();
	void setVolume(int volume);
	void setPosition(int volume);
	void setPlayMode(QString mode);
	int switchVolume();
	void updatePlayList(QVector<QString> playList);
	int getCurrentIndex();
	QString getPlayMode();
	void onPositionChanged(qint64 position);
	void onDurationChanged(qint64 duration);
	void updateProgress(qint64 position, qint64 duration);
private:
	QMediaPlayer* m_mediaPlayer;
	QMediaPlaylist* m_playlist;
	QString m_playMode{};	
	int m_volume = 20;
	int m_lastIndex = 0;
};

#endif __K_MEDIAPLAYERMANAGER_H_