#include "kmediaplayermanager.h"

#include <QObject>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <QMediaPlayer>
#include <QMediaPlaylist>

KMediaPlayerManager::KMediaPlayerManager(QObject* parent) 
    : QObject(parent)
    , m_mediaPlayer(new QMediaPlayer(this))
    , m_playlist(new QMediaPlaylist(this))
{
    m_mediaPlayer->setVolume(m_volume);
    
    m_mediaPlayer->setPlaylist(m_playlist);    
    // 连接播放状态改变信号
    connect(m_mediaPlayer, &QMediaPlayer::positionChanged, this, &KMediaPlayerManager::onPositionChanged);
    connect(m_mediaPlayer, &QMediaPlayer::durationChanged, this, &KMediaPlayerManager::onDurationChanged);
}



KMediaPlayerManager::~KMediaPlayerManager()
{
	delete m_mediaPlayer;
	delete m_playlist;
}

void KMediaPlayerManager::setCurrentIndex(int index)
{
    pause();
    m_playlist->setCurrentIndex(index);
    qDebug() << "setCurrentIndex(" << index << ")";
    auto p = m_playlist->currentMedia();
    qDebug()<<p.canonicalUrl();
}

void KMediaPlayerManager::setPlayList(QMediaPlaylist* other)
{
    m_playlist = other;
    m_playlist->setCurrentIndex(0);
}


void KMediaPlayerManager::pause()
{
    m_mediaPlayer->pause();
}

bool KMediaPlayerManager::isStoppedState()
{
    showState();
    if (m_mediaPlayer->state() == QMediaPlayer::StoppedState)
    {
        return true;
    }
    return false;
}

bool KMediaPlayerManager::isPlayingState()
{
    showState();
    if (m_mediaPlayer->state() == QMediaPlayer::PlayingState)
    {
        return true;
    }
    return false;
}

void KMediaPlayerManager::showState()
{
    if (m_mediaPlayer->state() == QMediaPlayer::StoppedState)
    {
        qDebug() << "StoppedState";
    }
    else if (m_mediaPlayer->state() == QMediaPlayer::PlayingState)
    {
        qDebug() << "PlayingState";
    }
    else if (m_mediaPlayer->state() == QMediaPlayer::PausedState)
    {
        qDebug() << "PausedState";
    }
}

void KMediaPlayerManager::check()
{
    showState();

    if (m_mediaPlayer->state() == QMediaPlayer::StoppedState || m_mediaPlayer->state() == QMediaPlayer::PausedState)
    {
        resume();
    }
    else
    {
        pause();
    }
}

void KMediaPlayerManager::resume()
{
    m_mediaPlayer->play();
}

void KMediaPlayerManager::resume(int index, QString localPath)
{
    //showPlayList();
    m_playlist->insertMedia(index, QUrl(localPath));    
    m_playlist->removeMedia(index + 1);
    m_playlist->setCurrentIndex(index);    
    m_mediaPlayer->play();
    showPlayList();
}

void KMediaPlayerManager::stop()
{
    m_mediaPlayer->stop();
}

void KMediaPlayerManager::showPlayList()
{
    int curindex = m_playlist->currentIndex();
    for (int i = 0; i < m_playlist->mediaCount(); ++i)
    {
        m_playlist->setCurrentIndex(i);
        qDebug() << "setCurrentIndex(" << i << "):";
        auto p = m_playlist->currentMedia();
        qDebug() << p.canonicalUrl();
    }
    m_playlist->setCurrentIndex(curindex);
}

void KMediaPlayerManager::setPlayList(QString mode)
{
    m_playMode = mode;
}

void KMediaPlayerManager::next()
{
    //showPlayList();
    if (m_playMode == "sortPlay")
    {    
        if (m_playlist->currentIndex() == m_playlist->mediaCount()-1)
            m_playlist->setCurrentIndex(0);
        else
            m_playlist->next();
    }
    else if (m_playMode == "randomPlay")
    {
        std::srand(std::time(nullptr)); // 使用当前时间作为随机数种子
        int randomNumber = rand()%m_playlist->mediaCount();
        m_lastIndex = m_playlist->currentIndex();
        if (randomNumber == m_lastIndex)
            randomNumber = m_lastIndex - 1 < 0 ? m_playlist->mediaCount() - 1 : m_lastIndex - 1;
        
        m_playlist->setCurrentIndex(randomNumber);
        setPosition(0);
        m_mediaPlayer->play();        
    }
    else if (m_playMode == "singlePlay")
    {
        setPosition(0);
        m_mediaPlayer->play();
    }
}

void KMediaPlayerManager::previous()
{    
    if (m_playMode == "sortPlay")
    {
        if (m_playlist->currentIndex() == 0)
            m_playlist->setCurrentIndex(m_playlist->mediaCount() - 1);
        else
            m_playlist->previous();
    }
    else if (m_playMode == "randomMode")
    {

        std::srand(std::time(nullptr)); // 使用当前时间作为随机数种子
        int randomNumber = rand() % m_playlist->mediaCount();
        m_lastIndex = m_playlist->currentIndex();
        if (randomNumber == m_lastIndex)
            randomNumber = m_lastIndex - 1 < 0 ? m_playlist->mediaCount() - 1 : m_lastIndex - 1;

        m_playlist->setCurrentIndex(randomNumber);
        setPosition(0);
        m_mediaPlayer->play();
    }
    else if (m_playMode == "singlePlay")
    {
        setPosition(0);
        m_mediaPlayer->play();
    }
}

void KMediaPlayerManager::setVolume(int volume)
{
    m_mediaPlayer->setVolume(volume);
}

void KMediaPlayerManager::setPosition(int volume)
{
    m_mediaPlayer->setPosition(volume);
}

void KMediaPlayerManager::setPlayMode(QString mode)
{
    m_playMode = mode;
}

int KMediaPlayerManager::switchVolume()
{
    if (m_mediaPlayer->volume() != 0)
    {
        m_volume = m_mediaPlayer->volume();
        setVolume(0);
        return 0;
    }
    else
    {
        setVolume(m_volume);
        return m_volume;
    }
    
}

void KMediaPlayerManager::updatePlayList(QVector<QString> playList)
{
    m_playlist->clear();
    qDebug() << "playList size:" << playList.size();
    for (int i=0 ;i < playList.size();++i)
    {
        m_playlist->addMedia(QUrl(playList[i]));
    }    
}

int KMediaPlayerManager::getCurrentIndex()
{
    return m_playlist->currentIndex();	
}

QString KMediaPlayerManager::getPlayMode()
{
    return m_playMode;
}

void KMediaPlayerManager::updateProgress(qint64 position, qint64 duration)
{
    emit progressChanged(position, duration);
}

void KMediaPlayerManager::onPositionChanged(qint64 position)
{
    updateProgress(position, m_mediaPlayer->duration());
}

void KMediaPlayerManager::onDurationChanged(qint64 duration)
{
    updateProgress(m_mediaPlayer->position(), duration);
}