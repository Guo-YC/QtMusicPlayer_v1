#include "konlinesongtablemodel.h"
#include "ksongtablemodel.h"
#include "ksonginfo.h"
#include <QObject>
#include <QUrl>
#include <QJsonDocument>
#include "kxhttprequestmanager.h"
KOnlineSongTableModel::KOnlineSongTableModel(QObject* parent)		
	:KSongTableModel(parent)
{
	(void)connect(httpRequestManger, &KxHttpRequestManager::requestFinished, httpRequestManger, &KxHttpRequestManager::requestFinishedHandler);
	
}

KOnlineSongTableModel::~KOnlineSongTableModel()
{

}

KOnlineSongTableModel::KOnlineSongTableModel(const KOnlineSongTableModel& other)
	:KSongTableModel(other)
{

}

KOnlineSongTableModel& KOnlineSongTableModel::operator=(const KOnlineSongTableModel& other)
{
	if (this != &other) 
	{		
		m_contents = other.m_contents;
		httpRequestManger = other.httpRequestManger;
	}
	return *this;
}


KOnlineSongTableModel::KOnlineSongTableModel(KOnlineSongTableModel&& other)
	:KSongTableModel(other)
{

}

KOnlineSongTableModel& KOnlineSongTableModel::operator=(KOnlineSongTableModel&& other)
{
	if (this != &other) 
	{
		// 移动成员变量的代码
		m_contents = std::move(other.m_contents);
		httpRequestManger = other.httpRequestManger;
		other.httpRequestManger = nullptr;
	}
	return *this;
}

