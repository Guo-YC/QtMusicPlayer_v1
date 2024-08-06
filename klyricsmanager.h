#ifndef KLYRICSMANAGER_H
#define KLYRICSMANAGER_H

#include <QObject>
#include <QMap>
#include <QString>
#include <QFile>
#include <QRegularExpression>

class KLyricsManager : public QObject 
{
    Q_OBJECT

public:
    explicit KLyricsManager(QObject* parent = nullptr);    
    std::vector<QString> splitString(const QString& str);
    void loadLyrics(const QString& filePath);
    QString getLyrics(qint64 position) const;

private:
    QMap<qint64, QString> m_lyricsMap;
};

#endif // KLYRICSMANAGER_H
