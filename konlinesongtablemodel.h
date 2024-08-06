#ifndef __K_ONLINESONG_H_
#define __K_ONLINESONG_H_

#include "ksongtablemodel.h"
class KSongTableModel;
class KOnlineSongTableModel : public KSongTableModel
{
    Q_OBJECT
public:
    KOnlineSongTableModel(QObject* parent = nullptr);
    KOnlineSongTableModel(const KOnlineSongTableModel& other);
    KOnlineSongTableModel& operator=(const KOnlineSongTableModel& other);
    KOnlineSongTableModel(KOnlineSongTableModel&& other);
    KOnlineSongTableModel& operator=(KOnlineSongTableModel&& other);
    ~KOnlineSongTableModel();
};

#endif // __K_ONLINESONG_H_