#ifndef __KMAINWINDOW_H_
#define __KMAINWINDOW_H_

#include <QMainWindow>
#include <QTimer>
#include <QSystemTrayIcon>
QT_BEGIN_NAMESPACE
namespace Ui { class KMainWindow; }
QT_END_NAMESPACE

class QModelIndex;
class KSongTableModel;
class KMediaPlayerManager;
class QSystemTrayIcon;
class KLyricsManager;
class KMainWindow : public QMainWindow
{
    Q_OBJECT

signals:
    void songChanged(int index);
public:
    KMainWindow(QWidget* parent = nullptr);
    ~KMainWindow();
    void initTableView();
    void initMediaPlayManager();
    void updatePlayListHandle(QVector<QString> playList);
    void downloadSong(int index, bool);
    void showCurrentSongName(int index);
    void playLocalMp3ByIndex(int index);
    void switchPushButton(QString state);
    void initLyricsManager();
    void updateLyrics(qint64 position);   

private slots:
    void onProgressChanged(qint64 position, qint64 duration);  
    void updatePixmapHandle(QPixmap& Pixmap);
    void rotateNoRotation();
    void rotate(double rotation);
    void toggleRotation();
    void positionChangedHandle();
    void voiceChangedHandle(int value);
    void voiceSwitchHandle();
    void playModeSwitchHandle();
    void switchState();
    void next();
    void previous();
    void onRowDoubleClicked(const QModelIndex& index);
    void trayIconActivated(QSystemTrayIcon::ActivationReason reason);
protected:
    void closeEvent(QCloseEvent* event) override;

private:
    Ui::KMainWindow* ui;
    KSongTableModel* m_songOnlineModel = nullptr;
    KMediaPlayerManager* m_mediaPlayerManager = nullptr;
    KLyricsManager* m_lyricsManager;
    QPixmap m_pixmap;
    double m_rotation = 0;
    QTimer m_timer{};
    QSystemTrayIcon* trayIcon;
};
#endif // __KMAINWINDOW_H_
