#include "klyricsmanager.h"
#include <QDebug>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
KLyricsManager::KLyricsManager(QObject* parent)
    : QObject(parent) {}


std::vector<QString> KLyricsManager::splitString(const QString& str) 
{
    std::vector<QString> result;
    int start = 0;
    int end = str.indexOf("\\n");

    while (end != -1) 
    {
        result.push_back(str.mid(start, end - start));
        start = end + 2;  // 因为是两个字符"\n"，所以要加 2
        end = str.indexOf("\\n", start);
    }

    result.push_back(str.mid(start));

    return result;
}
void KLyricsManager::loadLyrics(const QString& filePath) 
{
    m_lyricsMap.clear();
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) 
    {
        //qDebug() << "Failed to open file:" << filePath;
        return;
    }
   
    while (!file.atEnd()) 
    {
        QString line = file.readLine().trimmed();
        std::vector<QString> vec = splitString(line);
        //qDebug() << "Failed to open file:" << line;
        
        for (auto& str : vec)
        {
            QRegularExpression regex(R"(\[(\d{2}):(\d{2})\.(\d{2})\](.*)\\r)");
            QRegularExpressionMatch match = regex.match(str);
            if (match.hasMatch()) {
                QString s1 = match.captured(1);
                QString s2 = match.captured(2);
                QString s3 = match.captured(3);
                QString s4 = match.captured(4);
                qint64 minutes = match.captured(1).toInt();
                qint64 seconds = match.captured(2).toInt();
                qint64 milliseconds = match.captured(3).toInt() * 10;
                qint64 time = (minutes * 60 + seconds) * 1000 + milliseconds;
                QString lyric = match.captured(4);
                m_lyricsMap.insert(time, lyric); // 使用 insert 代替下标操作符
                //qDebug() << time << lyric;
                //std::cout << "Time: " << ime << ", Lyric: " << lyric.toStdString() << std::endl;
            }
            else {
                //qDebug() << "No match for line:" << line;
            }
        }

    }
    file.close();
    
}

QString KLyricsManager::getLyrics(qint64 position) const 
{
    qint64 closestTime = 0;
    for (auto it =m_lyricsMap.begin(); it != m_lyricsMap.end(); ++it) 
    {
        qint64 time = it.key();
        if (position >= it.key()) 
        {
            closestTime = it.key();
        }
        else 
        {
            break;
        }
    }
    return m_lyricsMap.value(closestTime);
}
