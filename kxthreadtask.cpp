#include"kxthreadtask.h"
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QCoreApplication>
#include <QObject>
#include <QDir>
#include <QThread>
#include <kxhttprequestmanager.h>
KxThreadTask::KxThreadTask(QObject* parent)
    : QObject(parent)
{
	qDebug() << "Thread id : " << QThread::currentThreadId;
	(void)connect(&m_manager, &QNetworkAccessManager::finished, this, &KxThreadTask::requestFinished);
}


KxThreadTask::~KxThreadTask()
{
	qDebug() << "Thread id : " << QThread::currentThreadId << "deleteLater";
}




bool KxThreadTask::confirmFile(const QString sfile)
{
	QString sAppPath = QCoreApplication::applicationDirPath();
	QString sFilePath = sfile;
	QString sPath = sFilePath.left(sFilePath.lastIndexOf('/'));

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

	return true;
}

void KxThreadTask::getRequest(QUrl url, QString requestType, QString path, int index)
{

	qDebug() << "getRequest：" << requestType;
	QNetworkRequest request(url);
	QMap<QString, QString> map;
	requestType = requestType + "&" + path + "&" + QString::number(index);


	request.setAttribute(QNetworkRequest::User, requestType);

	//m_requestType = requestType;
	QString requestTypeCopy = requestType;  // 复制 requestType
	QString pathCopy = path;  // 复制 path

	//QNetworkAccessManager manager;
	m_manager.get(request);
	
}

void KxThreadTask::requestFinished(QNetworkReply* reply)
{
	if (reply->error() == QNetworkReply::NoError) {

		QString attr = reply->request().attribute(QNetworkRequest::User).toString();
		QStringList parts = attr.split('&');
		QString requestType = parts[0];
		QString pathCopy = parts[1];
		int index = parts[2].toInt();



		QByteArray byte = reply->readAll();
		reply->deleteLater();  // 释放资源  


		bool comfire = confirmFile(pathCopy);

		if (requestType == "songname/music.mp3") emit threadMp3FileResponse(index, pathCopy);

		qDebug() << pathCopy << "mp3FileResponse()";
		QFile file(pathCopy);
		if (file.open(QIODevice::WriteOnly))
		{
			// 假设您有获取文件数据的方法，这里用假数据演示
			//QByteArray data = "Dummy MP3 Data";
			file.write(byte);
			file.close();
		}
		
	}
	else
	{
		qDebug() << "Error : " << reply->errorString();
	}

}

void KxThreadTask::getTest()
{
	qDebug() << "test";
}
