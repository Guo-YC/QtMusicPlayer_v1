@startuml
class KMainWindow {
    - Ui::KMainWindow* ui
    - KSongTableModel* m_songOnlineModel
    - KMediaPlayerManager* m_mediaPlayerManager
    - KLyricsManager* m_lyricsManager
    - QPixmap m_pixmap
    - double m_rotation
    - QTimer m_timer
    --
    + KMainWindow(QWidget *parent = nullptr)
    + ~KMainWindow()
    + void initTableView()
    + void initMediaPlayManager()
    + void updatePlayListHandle(QVector<QString> playList)
    + void downloadSong(int index, bool)
    + void showCurrentSongName(int index)
    + void playLocalMp3ByIndex(int index)
    + void switchPushButton(QString state)
    + void initLyricsManager()
    + void updateLyrics(qint64 position)
    --
    + void onProgressChanged(qint64 position, qint64 duration)
    + void updatePixmapHandle(QPixmap& Pixmap)
    + void rotateNoRotation()
    + void rotate(double rotation)
    + void toggleRotation()
    + void positionChangedHandle()
    + void voiceChangedHandle(int value)
    + void voiceSwitchHandle()
    + void playModeSwitchHandle()
    + void switchState()
    + void next()
    + void previous()
    + void onRowDoubleClicked(const QModelIndex& index)
    --

}

note right of KMainWindow
  KMainWindow 是应用程序
  的主窗口类，它负责初始化
  和管理界面的核心部件，
  包括歌曲表格、媒体播放
  器、歌词显示等。
end note

class KMediaPlayerManager {
    - QMediaPlayer* m_mediaPlayer
    - QMediaPlaylist* m_playlist
    - QString m_playMode
    - int m_volume
    - int m_lastIndex
    --
    + KMediaPlayerManager(QObject* parent = nullptr)
    + ~KMediaPlayerManager()
    + void setCurrentIndex(int index)
    + void setPlayList(QMediaPlaylist* other)
    + void pause()
    + bool isStoppedState()
    + bool isPlayingState()
    + void showState()
    + void check()
    + void resume()
    + void resume(int index, QString localPath)
    + void stop()
    + void showPlayList()
    + void setPlayList(QString mode)
    + void next()
    + void previous()
    + void setVolume(int volume)
    + void setPosition(int volume)
    + void setPlayMode(QString mode)
    + int switchVolume()
    + void updatePlayList(QVector<QString> playList)
    + int getCurrentIndex()
    + QString getPlayMode()
    + void onPositionChanged(qint64 position)
    + void onDurationChanged(qint64 duration)
    + void updateProgress(qint64 position, qint64 duration)
    --
    - signal progressChanged(qint64 position, qint64 duration)
}

note top of KMediaPlayerManager
  KMediaPlayerManager 是
  音乐播放的控件类，
  它负责初始化和管理歌
  曲的各种操作，包括暂停、
  播放、切换，音量控制、进度控制等。
end note

class KLyricsManager {
    - QMap<qint64, QString> m_lyricsMap
    --
    + KLyricsManager(QObject* parent = nullptr)
    + std::vector<QString> splitString(const QString& str)
    + void loadLyrics(const QString& filePath)
    + QString getLyrics(qint64 position) const
}

note top of KLyricsManager
  KLyricsManager 是歌词的管理类，
  它负责解析歌词文件和获取实时歌词，
  包括预处理歌词数据、返回当前歌曲播放歌词等。
end note


class KSongTableModel {
    - QVector<KSongInfo> m_contents
    - KxHttpRequestManager* httpRequestManger
    - QMediaPlayer* m_mediaPlayer
    - QThread m_task_thread
    - KxThreadTask* m_ptask
    --
    + KSongTableModel(QObject* parent)
    + ~KSongTableModel()
    + void add(KSongInfo& song)
    + QString getMusicNameByIndex(int index)
    + int getDurationByIndex(int index)
    + void getPicByIndex(int index)
    + void getMp3ByIndex(int index)
    + void onHandleTaskFinished()
    + QString getLocalMp3PathByIndex(int index)
    + QString getLocalLyricPathByIndex(int index)
    + void initDataSource()
    + QVariant headerData(int section, 
Qt::Orientation orientation, int role = Qt::DisplayRole) const
    + int rowCount(const QModelIndex& parent = QModelIndex()) const
    + int columnCount(const QModelIndex& parent = QModelIndex()) const
    + QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const
    + void updateData()
    + QVector<QString> getPlayList()
    + bool confirmFile(const QString sfile)
    + QString& downloadFileInLocal(KSongInfo& info)
    --
    - signal updatePlayListSignal(QVector<QString> vec)
    - signal updatetPixmap(QPixmap& scaledPixmap)
    - signal appendSongInPlayList(QString& localPath)
    - signal downLoadSucSignal(int index, QString localPath)
    - signal threadRequest(QUrl url, QString requestType, QString path, int index)
}

note top of KSongTableModel 
  KSongTableModel 是歌单信息管理类，
  它负责管理歌单歌曲的各种信息，
  包括管理时长、歌名、等信息，对数据进行各种操作。
end note


KOnlineSongTableModel --|> KSongTableModel

class KSongInfo {
    - QString m_musicName
    - QString m_albumName
    - int m_duration
    - QString m_path
    - QString m_mp3
    - QString m_lyric
    - QString m_img
    - QString m_localFilePath
    - QString m_localFileState
    - QString m_localLyricFilePath
    - QString m_localPicFilePath
    --
    + KSongInfo(QJsonObject& obj)
    + KSongInfo(const KSongInfo& info)
    + KSongInfo(KSongInfo&& info)
    + KSongInfo& operator=(const KSongInfo& info)
    + KSongInfo& operator=(KSongInfo&& info)
    + ~KSongInfo()
    + void setMusicName(const QString& name)
    + void setAlbumName(const QString& albumName)
    + void setDuration(const int& duration)
    + void setPath(const QString& path)
    + void setMp3(const QString& mp3)
    + void setLyric(const QString& lyric)
    + void setImg(const QString& img)
    + void setLocalFilePath(const QString& path)
    + void setLocalLyricFilePath(const QString& path)
    + void setLocalPicFilePath(const QString& path)
    + void setLocalFileState(const QString& path)
    + QString getMusicName() const
    + QString getAlbumName() const
    + int getDuration() const
    + QString showDuration() const
    + QString getPath() const
    + QString getMp3() const
    + QString getLyric() const
    + QString getImg() const
    + QString getLocalFilePath() const
    + QString getLocalLyricFilePath() const
    + QString getLocalPicFilePath() const
    + QString getLocalFileState() const
    + QString toString() const
}

note top of KSongInfo 
  KSongInfo 是歌单信息存储类，
  它负责获取和管理歌曲的各种信息，
  包括管理时长、歌名、专辑、资源路径等信息。
end note


class KxHttpRequestManager {
    - QNetworkAccessManager m_manager
    - QString m_requestType
    - QString m_requestPath
    --
    + KxHttpRequestManager(QObject* parent = nullptr)
    + ~KxHttpRequestManager()
    + void sendGetRequest(const QUrl& url)
    + void sendGetRequest(const QUrl& url, QString& requestType)
    + bool confirmFile(const QString sfile)
    + void sendPostRequest(const QUrl& url, const QByteArray& data)
    + void requestFinishedHandler(QNetworkReply* reply)
    --
    - signal requestFinished(QNetworkReply* reply)
    - signal getUserResult(QJsonArray& array)
    - signal initDataRespone(QJsonDocument& doc)
    - signal songNameResponse(QJsonDocument& doc)
    - signal songPicResponse(QByteArray& pic)
}

note top of KxHttpRequestManager 
  KxHttpRequestManager  是http请求管理类，
  它负责处理各种http请求，并返回响应结果。
end note


class KxThreadTask {
    - QNetworkAccessManager m_manager
    --
    + KxThreadTask(QObject* parent = nullptr)
    + ~KxThreadTask()
    + bool confirmFile(const QString sfile)
    + void getRequest(QUrl url, QString requestType, QString path, int index)
    + void requestFinished(QNetworkReply* reply)
    --
    - signal progressUpdated(int progress)
    - signal threadMp3FileResponse(int index, QString path)
}


note top of KxThreadTask 
  KxThreadTask 是一个自定义线程类，
  它负责通过子线程的方式请求http，
  处理http响应。
end note

KMainWindow *-- KSongTableModel
KMainWindow *-- KMediaPlayerManager
KMainWindow *-- KLyricsManager

KMediaPlayerManager *-- QMediaPlayer
KMediaPlayerManager *-- QMediaPlaylist

KSongTableModel *-- KSongInfo
KSongTableModel *-- KxHttpRequestManager
KSongTableModel *-- QMediaPlayer
KSongTableModel *-- QThread
KSongTableModel *-- KxThreadTask

KxHttpRequestManager *-- QNetworkAccessManager

KxThreadTask *-- QNetworkAccessManager
@enduml