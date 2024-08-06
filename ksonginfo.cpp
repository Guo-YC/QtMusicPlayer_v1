#include "ksonginfo.h"
#include <QJsonObject>


KSongInfo::KSongInfo(QJsonObject& info)
    :m_musicName(info["musicName"].toString())
    , m_albumName(info["albumName"].toString())
    , m_duration(info["duration"].toInt())
    , m_path(info["path"].toString())
    , m_mp3(info["mp3"].toString())
    , m_lyric(info["lyric"].toString())
    , m_img(info["img"].toString())
{
}

KSongInfo::KSongInfo(const QString& musicName, const QString& albumName, const QString& path, const QString& mp3, const QString& lyric, const QString& img, int duration)
    : m_musicName(musicName), m_albumName(albumName), m_duration(duration), m_path(path), m_mp3(mp3), m_lyric(lyric), m_img(img)
{
}

KSongInfo::~KSongInfo() 
{

}

KSongInfo::KSongInfo(const KSongInfo& info)
    :m_musicName(info.m_musicName)
    , m_albumName(info.m_albumName)
    , m_duration(info.m_duration)
    , m_path(info.m_path)
    , m_mp3(info.m_mp3)
    , m_lyric(info.m_lyric)
    , m_img(info.m_img)
{
}

KSongInfo::KSongInfo(KSongInfo&& info)
    : m_musicName(std::move(info.m_musicName))
    , m_albumName(std::move(info.m_albumName))
    , m_duration(std::move(info.m_duration))
    , m_path(std::move(info.m_path))
    , m_mp3(std::move(info.m_mp3))
    , m_lyric(std::move(info.m_lyric))
    , m_img(std::move(info.m_img))
{

}

KSongInfo& KSongInfo::operator=(const KSongInfo& info)
{
    if (this != &info) {
        m_musicName = info.m_musicName;
        m_albumName = info.m_albumName;
        m_duration = info.m_duration;
        m_path = info.m_path;
        m_mp3 = info.m_mp3;
        m_lyric = info.m_lyric;
        m_img = info.m_img;
    }
    return *this;
}

KSongInfo& KSongInfo::operator=(KSongInfo&& info)
{
    if (this != &info) {
        m_musicName = std::move(info.m_musicName);
        m_albumName = std::move(info.m_albumName);
        m_duration = std::move(info.m_duration);
        m_path = std::move(info.m_path);
        m_mp3 = std::move(info.m_mp3);
        m_lyric = std::move(info.m_lyric);
        m_img = std::move(info.m_img);
    }
    return *this;
}

void KSongInfo::setMusicName(const QString& name) 
{
    m_musicName = name;
}

void KSongInfo::setAlbumName(const QString& albumName) 
{
    m_albumName = albumName;
}

void KSongInfo::setDuration(const int& duration) 
{
    m_duration = duration;
}

void KSongInfo::setPath(const QString& path) 
{
    m_path = path;
}


void KSongInfo::setMp3(const QString& mp3) 
{
    m_mp3 = mp3;
}

void KSongInfo::setLyric(const QString& lyric) 
{
    m_lyric = lyric;
}

void KSongInfo::setImg(const QString& img) 
{
    m_img = img;
}

void KSongInfo::setLocalFilePath(const QString& path)
{
    m_localFilePath = path;
}

void KSongInfo::setLocalLyricFilePath(const QString& path)
{
    m_localLyricFilePath = path;
}

void KSongInfo::setLocalPicFilePath(const QString& path)
{
    m_localPicFilePath = path;
}

void KSongInfo::setLocalFileState(const QString& path)
{
    m_localFileState = path;
}

QString KSongInfo::getMusicName() const 
{
    return m_musicName;
}

QString KSongInfo::getAlbumName() const 
{
    return m_albumName;
}

int KSongInfo::getDuration() const 
{
    return m_duration;
}

QString KSongInfo::showDuration() const
{
    int seconds = m_duration;
    int minutes = seconds / 60;
    seconds %= 60;

    QString durationString;
    durationString = QString("%1:%2")
        .arg(minutes, 2, 10, QChar('0'))
        .arg(seconds, 2, 10, QChar('0'));
    return durationString;
}

QString KSongInfo::getPath() const 
{
    return m_path;
}

QString KSongInfo::getMp3() const 
{
    return m_mp3;
}

QString KSongInfo::getLyric() const 
{
    return m_lyric;
}

QString KSongInfo::getImg() const 
{
    return m_img;
}

QString KSongInfo::getLocalFilePath() const
{
    return m_localFilePath;
}

QString KSongInfo::getLocalLyricFilePath() const
{
    return m_localLyricFilePath;
}

QString KSongInfo::getLocalPicFilePath() const
{
    return m_localPicFilePath;
}

QString KSongInfo::getLocalFileState() const
{
    if (m_localFileState == "File_No_Find")
    {
        return QString::fromLocal8Bit("暂未缓存");
    }
    else if (m_localFileState == "File_Downloading")
    {
        return QString::fromLocal8Bit("缓存中...");
    }
    else if (m_localFileState == "File_Downloaded")
    {
        return QString::fromLocal8Bit("已缓存");
    }
    return m_localFileState;
}

QString KSongInfo::toString() const
{
    QString str{};
    str = getMusicName() + " " + getAlbumName() + " " + QString::number(getDuration()) + " " + getPath() + " " + getMp3() + " " + getLyric() + " " + getImg();
    return str;
}
