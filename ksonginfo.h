#ifndef __K_SONGINFO_H__
#define __K_SONGINFO_H__

#include <QString>
#include <QJsonObject>
class KSongInfo
{
public:
    KSongInfo(QJsonObject& obj);
    KSongInfo(const QString& musicName, const QString& albumName, const QString& path, const QString& mp3, const QString& lyric, const QString& img, int duration);
    KSongInfo(const KSongInfo& info);
    KSongInfo(KSongInfo&& info);
    KSongInfo& operator=(const KSongInfo& info);
    KSongInfo& operator=(KSongInfo&& info);
    ~KSongInfo();

    void setMusicName(const QString& name);
    void setAlbumName(const QString& albumName);
    void setDuration(const int& duration);
    void setPath(const QString& path);
    void setMp3(const QString& mp3);
    void setLyric(const QString& lyric);
    void setImg(const QString& img);
    void setLocalFilePath(const QString& path);
    void setLocalLyricFilePath(const QString& path);
    void setLocalPicFilePath(const QString& path);
    void setLocalFileState(const QString& path);
    QString getMusicName() const;
    QString getAlbumName() const;
    int getDuration() const;
    QString showDuration() const;
    QString getPath() const;
    QString getMp3() const;
    QString getLyric() const;
    QString getImg() const;
    QString getLocalFilePath() const;
    QString getLocalLyricFilePath() const;
    QString getLocalPicFilePath() const;
    QString getLocalFileState() const;
    QString toString() const;
       
private:
    QString m_musicName{};
    QString m_albumName{};
    int m_duration{};
    QString m_path{};
    QString m_mp3{};
    QString m_lyric{};
    QString m_img{};
    QString m_localFilePath{};
    QString m_localFileState{ "File_No_Find" };
    QString m_localLyricFilePath{};
    QString m_localPicFilePath{};
};

#endif // __K_SONGINFO_H__
