#include "ksongtablemodel.h"
#include "ksonginfo.h"
#include "kxhttprequestmanager.h"

#include <QDebug>
#include <QObject>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QMediaPlaylist>
#include <QMediaPlayer>
#include <QVector>
#include <QString>
#include <string>
#include <QPixmap>
#include <QFile>
#include <QCoreApplication>
#include <QDir>
#include <kxthreadtask.h>
KSongTableModel::KSongTableModel(QObject* parent)
	: QAbstractTableModel(parent)
	, httpRequestManger(new KxHttpRequestManager(parent))
	, m_ptask(new KxThreadTask)
{

	(void)connect(httpRequestManger, &KxHttpRequestManager::requestFinished, httpRequestManger, &KxHttpRequestManager::requestFinishedHandler);
	(void)connect(httpRequestManger, &KxHttpRequestManager::initDataRespone, this, [=](QJsonDocument& doc)
		{
			QJsonArray& array = doc.array();
			QVector<QString> songVector{};
			for (auto& v : array)
			{
				QJsonObject& obj = v.toObject();
				if (obj["type"].toString() == "dir")
				{
					songVector.push_back(obj["path"].toString());
					qDebug() << obj["path"].toString();
					httpRequestManger->sendGetRequest(QUrl("https://gitee.com/MarkYangUp/music/raw/master/" + obj["path"].toString() + "/music.json/"), QString("contents/songname/music.json"));
				}
			}

		});
	// 添加歌曲信息
	(void)connect(httpRequestManger, &KxHttpRequestManager::songNameResponse, this, [=](QJsonDocument& doc)
		{
			QJsonObject& object = doc.object();
			QJsonArray listArray = object.value("list").toArray();
			for (const QJsonValue& value : listArray) {
				QJsonObject itemObject = value.toObject();
				KSongInfo songInfo(itemObject);
				qDebug() << songInfo.toString();
				KSongTableModel::add(songInfo);
			}

		});

	(void)connect(httpRequestManger, &KxHttpRequestManager::songPicResponse, this, &KSongTableModel::songPicResponse);
	(void)connect(httpRequestManger, &KxHttpRequestManager::mp3FileResponse, this, &KSongTableModel::mp3FileResponseHandle);
	(void)connect(&m_task_thread, &QThread::finished, m_ptask, &KxThreadTask::deleteLater);
	(void)connect(this, &KSongTableModel::threadRequest, m_ptask, &KxThreadTask::getRequest);
	(void)connect(m_ptask, &KxThreadTask::threadMp3FileResponse, this, &KSongTableModel::mp3FileResponseHandle);
	//(void)connect(m_ptask, &KxThreadTask::taskFinished, this, &KSongTableModel::onHandleTaskFinished);
}

void KSongTableModel::songPicResponse(QByteArray& pic)
{
	QPixmap pixmap;
	pixmap.loadFromData(pic);
	QPixmap scaledPixmap = pixmap.scaled(400, 400, Qt::KeepAspectRatio);
	emit updatetPixmap(scaledPixmap);

}

void KSongTableModel::mp3FileResponseHandle(int index, QString filePath)
{
	//m_task_thread.quit();
	qDebug() << filePath << " downLoad suc !";
	KSongInfo& songinfo = m_contents[index];

	songinfo.setLocalFileState("File_Downloaded");
	QModelIndex _index = this->index(index, 3); // 假设第一列为有效列
	this->data(_index, Qt::DisplayRole);
	//emit dataChanged(_index, _index);
	emit downLoadSucSignal(index, filePath);
}

KSongTableModel::~KSongTableModel()
{

	m_task_thread.wait();
	m_task_thread.quit();
	delete httpRequestManger;
	delete m_mediaPlayer;
	delete m_ptask;
}

KSongTableModel::KSongTableModel(const KSongTableModel& other)
	:httpRequestManger(new KxHttpRequestManager(other.httpRequestManger))
	, m_contents(other.m_contents)
{
}



void KSongTableModel::add(KSongInfo& song)
{
	beginInsertRows(QModelIndex(), m_contents.size(), m_contents.size());
	m_contents.push_back(song);
	endInsertRows();
	//emit appendSongInPlayList(downloadFileInLocal(song));
	emit updatePlayListSignal(getPlayList());
}

QString KSongTableModel::getMusicNameByIndex(int index)
{
	if (index < 0 || index >= m_contents.size())
		return "  ";
	KSongInfo& songinfo = m_contents[index];
	return songinfo.getMusicName();
}

int KSongTableModel::getDurationByIndex(int index)
{
	if (index < 0 || index >= m_contents.size())
		return 1;
	KSongInfo& songinfo = m_contents[index];
	return songinfo.getDuration();
}

void KSongTableModel::getPicByIndex(int index)
{
	if (index < 0 || index >= m_contents.size())
		return;
	KSongInfo& songinfo = m_contents[index];
	QString str = QString("https://gitee.com/MarkYangUp/music/raw/master") + songinfo.getPath() + QString("/img.jpg");
	httpRequestManger->sendGetRequest(QUrl(str), QString("music/img.jpg"));
	//(void)connect(httpRequestManger, &KxHttpRequestManager::songPicResponse, this, [=](QByteArray& pic)
	//	{
	//		QPixmap pixmap;
	//		pixmap.loadFromData(pic);
	//		QPixmap scaledPixmap = pixmap.scaled(400,400, Qt::KeepAspectRatio);
	//		emit updatetPixmap(scaledPixmap);

	//	});
}

void KSongTableModel::getMp3ByIndex(int index)
{
	if (index < 0 || index >= m_contents.size())
		return;
	KSongInfo& songinfo = m_contents[index];

	if (songinfo.getLocalFilePath().length() != 0) // 已下载
		emit downLoadSucSignal(index, songinfo.getLocalFilePath());


	QString pos = QString("https://gitee.com/MarkYangUp/music/raw/master") + songinfo.getPath() + QString("/music.mp3");
	QString pos_words = QString("https://gitee.com/MarkYangUp/music/raw/master") + songinfo.getPath() + QString("/lyric.txt");
	QString pos_img = QString("https://gitee.com/MarkYangUp/music/raw/master") + songinfo.getPath() + QString("/img.jpg");
	QString basePath = QCoreApplication::applicationDirPath(); // 获取程序运行的当前工作目录
	// basePath D:/KingSoft_Project/qt_project/build/bin/Debug/
	// 构建文件的完整路径

	QString localPath = QString("/music") + songinfo.getPath() + QString("/music.mp3");
	QString filePath = basePath + localPath;
	songinfo.setLocalFilePath(filePath);

	QString words_localPath = QString("/music") + songinfo.getPath() + QString("/lyric.txt");
	QString words_filePath = basePath + words_localPath;
	songinfo.setLocalLyricFilePath(words_filePath);

	QString img_localPath = QString("/music") + songinfo.getPath() + QString("/img.jpg");
	QString img_filePath = basePath + img_localPath;
	songinfo.setLocalPicFilePath(img_filePath);

	songinfo.setLocalFileState("File_Downloading");
	updateData();
	QModelIndex _index = this->index(index, 3); // 假设第一列为有效列		
	this->data(_index, Qt::DisplayRole);
	//KxThreadTask* ptask = new  KxThreadTask;


	m_ptask->moveToThread(&m_task_thread);
	m_task_thread.start();
	//emit getTest();
	emit threadRequest(QUrl(pos), QString("songname/music.mp3"), filePath, index);
	emit threadRequest(QUrl(pos_words), QString("songname/lyric.txt"), words_filePath, index);
	emit threadRequest(QUrl(pos_img), QString("songname/img.jpg"), img_filePath, index);
	//httpRequestManger->sendGetRequest(QUrl(pos), QString("songname/music.mp3"), filePath, index);
	//httpRequestManger->sendGetRequest(QUrl(pos_words), QString("songname/lyric.txt"), words_filePath, index);
	//httpRequestManger->sendGetRequest(QUrl(pos_img), QString("songname/img.jpg"), img_filePath, index);


}

void KSongTableModel::onHandleTaskFinished()
{
	m_task_thread.quit();
	//QMessageBox::information(this, "information", "task finished");
}

QString KSongTableModel::getLocalMp3PathByIndex(int index)
{
	if (index < 0 || index >= m_contents.size())
		return "";
	KSongInfo& songinfo = m_contents[index];
	QDir dir;
	if (songinfo.getLocalFilePath().length() > 0 && dir.exists(songinfo.getLocalFilePath()))
		return songinfo.getLocalFilePath();
	else
		return "";
}

QString KSongTableModel::getLocalLyricPathByIndex(int index)
{
	if (index < 0 || index >= m_contents.size())
		return "";
	KSongInfo& songinfo = m_contents[index];
	QDir dir;
	if (songinfo.getLocalLyricFilePath().length() > 0 && dir.exists(songinfo.getLocalLyricFilePath()))
		return songinfo.getLocalLyricFilePath();
	else
		return "";
}



QVariant KSongTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role != Qt::DisplayRole)
		return QVariant();

	if (orientation == Qt::Horizontal)
	{
		switch (section)
		{
		case 0:
			return QString::fromLocal8Bit("歌名");
		case 1:
			return QString::fromLocal8Bit("专辑");
		case 2:
			return QString::fromLocal8Bit("时长");
		case 3:
			return QString::fromLocal8Bit("歌曲缓存");
		default:
			break;

		}
	}
	return QVariant();
}

int KSongTableModel::rowCount(const QModelIndex& parent) const
{

	return parent.isValid() ? 0 : m_contents.size();
}
int KSongTableModel::columnCount(const QModelIndex& parent) const
{
	return parent.isValid() ? 0 : 4;
}

QVariant KSongTableModel::data(const QModelIndex& index, int role) const
{
	if (!index.isValid())
		return QVariant();

	if (index.row() >= m_contents.size() || index.row() < 0)
		return QVariant();
	if (role == Qt::DisplayRole || role == Qt::EditRole)
	{
		const KSongInfo& song = m_contents[index.row()];
		switch (index.column())
		{
		case 0:
			return song.getMusicName();
		case 1:
			return song.getAlbumName();
		case 2:
			return song.showDuration();
		case 3:
			return song.getLocalFileState();
		default:
			break;
		}

	}
	return QVariant();
}


std::string urlEncode(const std::string& str) 
{
	std::string encoded;
	for (char c : str) 
	{
		if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') 
		{
			encoded += c;
		}
		else 
		{
			encoded += '%';
			char hex[3];
			sprintf(hex, "%02X", static_cast<unsigned char>(c));
			encoded += hex;
		}
	}
	return encoded;
}

void KSongTableModel::initDataSource()
{

	httpRequestManger->sendGetRequest(QUrl(QString("https://gitee.com/MarkYangUp/music/raw/master/%E5%AD%A4%E5%8B%87%E8%80%85/music.json/")), QString("contents/songname/music.json"));
	httpRequestManger->sendGetRequest(QUrl(QString("https://gitee.com/MarkYangUp/music/raw/master/%E9%A3%8E%E9%9B%A8%E6%97%A0%E9%98%BB/music.json/")), QString("contents/songname/music.json"));
	//httpRequestManger->sendGetRequest(QUrl(QString("https://gitee.com/MarkYangUp/music/raw/master/%E9%A3%8E%E9%9B%A8%E6%97%A0%E9%98%BB/%E5%91%A8%E5%8D%8E%E5%81%A5/music.json/")), QString("contents/songname/music.json"));
}

void KSongTableModel::updateData()
{
	beginResetModel();
	//setContent(data);
	endResetModel();
}

QVector<QString> KSongTableModel::getPlayList()
{
	QVector<QString> vec;
	for (auto& p : m_contents)
	{
		QString pos = QString("https://gitee.com/MarkYangUp/music/raw/master") + p.getPath() + QString("/music.mp3");
		qDebug() << pos;
		vec.push_back(pos);
	}

	return vec;
}


bool KSongTableModel::confirmFile(const QString sfile)
{
	QString sAppPath = QCoreApplication::applicationDirPath();
	QString sFilePath = sfile;
	QString sPath = sFilePath.left(sFilePath.lastIndexOf('/'));

	// 文件是否存在
	   // QString sPath = QString::fromLocal8Bit("D:/KingSoft_Project/qt_project/build/bin/Debug/music/风雨无阻/周华健");
	QDir dir;
	if (!dir.exists(sPath))
	{
		bool res = dir.mkpath(sPath);
		if (res) {
			qDebug() << "创建目录成功";
		}
		else {
			qDebug() << "创建目录失败，错误信息: ";
		}
	}
	else {
		/*qDebug() << "目录已存在";*/
	}

	return true;
}


