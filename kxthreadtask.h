#ifndef __KX_THREAD_TASK__
#define __KX_THREAD_TASK__

#include <QObject>
#include <QNetworkAccessManager>
class QNetworkAccessManager;
class QNetworkReply;
class KxThreadTask : public QObject
{
    Q_OBJECT

public:
    KxThreadTask(QObject* parent = Q_NULLPTR);
    ~KxThreadTask();    
	bool confirmFile(const QString sfile);	
    void getRequest(QUrl url, QString requestType, QString path, int index);
    void requestFinished(QNetworkReply* reply);
    void getTest();
signals:
    void progressUpdated(int progress);//ͨ��ui�߳��������
    void taskFinished();// ֪ͨ ui �߳��������
    void threadMp3FileResponse(int index, QString path);
private:
    QNetworkAccessManager m_manager;    
};

#endif