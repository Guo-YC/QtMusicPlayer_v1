#ifndef __KX_HTTP_REQUEST_MANAGER_H_
#define __KX_HTTP_REQUEST_MANAGER_H_

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

class QUrl;
class QJsonDocument;
class KxHttpRequestManager : public QObject
{
	Q_OBJECT

public:
	KxHttpRequestManager(QObject* parent = nullptr);
	~KxHttpRequestManager();
	void sendGetRequest(const QUrl& url);
	void sendGetRequest(const QUrl& url, QString& requestType);
	bool confirmFile(const QString sfile);
	void sendGetRequest(const QUrl& url, QString& requestType, QString path, int index);	
	// 发送 get 请求
	void sendPostRequest(const QUrl& url, const QByteArray& data); // 发送 post 请求
	void requestFinishedHandler(QNetworkReply* reply);	
signals:
	void requestFinished(QNetworkReply* reply);
	void getUserResult(QJsonArray& array);
	void initDataRespone(QJsonDocument& doc);
	void songNameResponse(QJsonDocument& doc);
	void songPicResponse(QByteArray& pic);
	void mp3FileResponse(int index, QString path);
private:
	QNetworkAccessManager m_manager;
	QString m_requestType{};
	QString m_requestPath{};
};

#endif