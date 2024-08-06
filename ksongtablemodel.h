#ifndef __K_STUTABLEMODEL_H_
#define __K_STUTABLEMODEL_H_
#include <QAbstractTableModel>
#include <QJsonArray>
#include <QJsonObject>
#include <QThread>
class KSongInfo;
class QMediaPlayer;
class KxHttpRequestManager;
class QMediaPlayList;
class KxThreadTask;
class KSongTableModel : public QAbstractTableModel
{
	Q_OBJECT

signals:
	void updatePlayListSignal(QVector<QString> vec);
	void updatetPixmap(QPixmap& scaledPixmap);
	void appendSongInPlayList(QString& localPath);
	void downLoadSucSignal(int index, QString localPath);
	void threadRequest(QUrl url, QString requestType, QString path, int index);
	void getTest();
public:
	KSongTableModel(QObject* parent);
	void songPicResponse(QByteArray& pic);
	void mp3FileResponseHandle(int index, QString filePath);
	virtual ~KSongTableModel();
	KSongTableModel(const KSongTableModel& other);
	KSongTableModel& operator=(const KSongTableModel& other);
	KSongTableModel& operator=(KSongTableModel&& other);
	KSongTableModel(KSongTableModel&& other);
	virtual void add(KSongInfo& song);
	QString getMusicNameByIndex(int index);
	int getDurationByIndex(int index);
	void getPicByIndex(int index);
	void getMp3ByIndex(int index);
	void onHandleTaskFinished();
	QString getLocalMp3PathByIndex(int index);
	QString getLocalLyricPathByIndex(int index);
	void initDataSource();
	virtual QVariant
		headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
	// ����ģ�͵�����
	virtual int
		rowCount(const QModelIndex& parent = QModelIndex()) const override;
	// ����ģ�͵�����
	virtual int
		columnCount(const QModelIndex& parent = QModelIndex()) const override;
	// ����ģ�����������ݽ�ɫ,����ģ�͵�����
	virtual QVariant
		data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
	//// ָ����Ԫ������һЩ��־
	void updateData();
	QVector<QString> getPlayList();
	bool confirmFile(const QString sfile);
	QString& downloadFileInLocal(KSongInfo& info);
protected:
	QVector<KSongInfo> m_contents{};
	KxHttpRequestManager* httpRequestManger = nullptr;
	QMediaPlayer* m_mediaPlayer = nullptr;
	QThread m_task_thread;
	KxThreadTask* m_ptask = nullptr;
};
#endif