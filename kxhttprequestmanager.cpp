#include "kxhttprequestmanager.h"

#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QCoreApplication>
#include <QDir>
KxHttpRequestManager::KxHttpRequestManager(QObject* parent)
	: QObject(parent)
{
	(void)connect(&m_manager, &QNetworkAccessManager::finished, this, &KxHttpRequestManager::requestFinished);
}

KxHttpRequestManager::~KxHttpRequestManager()
{
}
// ���� get ����
void KxHttpRequestManager::sendGetRequest(const QUrl& url)
{
	QNetworkRequest request(url);	
	m_manager.get(request);
}

void KxHttpRequestManager::sendGetRequest(const QUrl& url, QString& requestType)
{
	QNetworkRequest request(url);
	m_requestType = requestType;

	//QNetworkAccessManager manager;
	QNetworkReply* reply = m_manager.get(request);

}

// ���� post ����
void KxHttpRequestManager::sendPostRequest(const QUrl& url, const QByteArray& data)
{
	QNetworkRequest request(url);
	request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
	m_manager.post(request, data);
}


bool KxHttpRequestManager::confirmFile(const QString sfile)
{
	QString sAppPath = QCoreApplication::applicationDirPath();
	QString sFilePath = sfile;
	QString sPath = sFilePath.left(sFilePath.lastIndexOf('/'));

	QDir dir;
	if (!dir.exists(sPath))
	{
		bool res = dir.mkpath(sPath);
		if (res) {
			qDebug() << "����Ŀ¼�ɹ�";
		}
		else {
			qDebug() << "����Ŀ¼ʧ�ܣ�������Ϣ: ";
		}
	}
	else {
		/*qDebug() << "Ŀ¼�Ѵ���";*/
	}

	return true;
}

void KxHttpRequestManager::sendGetRequest(const QUrl& url, QString& requestType, QString path, int index)
{
	qDebug() << "sendGetRequest path:" << path;
	QNetworkRequest request(url);
	
	//request.setRawHeader("requestType", requestType.toUtf8());
	m_requestType = requestType;
	QString requestTypeCopy = requestType;  // ���� requestType
	QString pathCopy = path;  // ���� path

	QNetworkAccessManager manager;
	QNetworkReply* reply = manager.get(request);

	QEventLoop eventLoop;
	(void)connect(&manager, &QNetworkAccessManager::finished, &eventLoop, &QEventLoop::quit);
	eventLoop.exec();
	 
	if (reply->error() == QNetworkReply::NoError) {
		QByteArray byte = reply->readAll();
		reply->deleteLater();  // �ͷ���Դ  


		bool comfire = confirmFile(pathCopy);

		qDebug() << pathCopy << "mp3FileResponse()";
		QFile file(pathCopy);
		if (file.open(QIODevice::WriteOnly))
		{
			// �������л�ȡ�ļ����ݵķ����������ü�������ʾ
			//QByteArray data = "Dummy MP3 Data";
			file.write(byte);
			file.close();
		}
		else
		{
			//qDebug() << path + QString::fromLocal8Bit("�ļ���ʧ�ܣ�");
		}
		if (m_requestType == "songname/music.mp3")emit mp3FileResponse(index, path);
	}
	else
	{
		qDebug() << "Error : " << reply->errorString();
	}

}




void KxHttpRequestManager::requestFinishedHandler(QNetworkReply* reply)
{
	if (reply->error() == QNetworkReply::NoError)
	{
		QByteArray responseData = reply->readAll();// ��ȡӦ������

		QJsonDocument doc = QJsonDocument::fromJson(responseData);

		if ( m_requestType == "music/contents")
		{
			emit initDataRespone(doc);
		}
		else if (m_requestType == "contents/songname/music.json")
		{
			emit songNameResponse(doc);
		}
		else if (m_requestType == "music/img.jpg")
		{
			emit songPicResponse(responseData);
		}
	}
	else
	{
		qDebug() << "Error : " << reply->errorString();
	}
}