#include "kmainwindow.h"
#include <QApplication>
#include <QDebug>
#include <QFile>

int main(int argc, char* argv[])
{    
    QApplication a(argc, argv);        
    // ����QSS�ļ�
    //QFile file(":/qss/style.qss");
    //if (file.open(QFile::ReadOnly)) {
    //    QString styleSheet = QLatin1String(file.readAll());
    //    a.setStyleSheet(styleSheet);
    //}

    KMainWindow w;
    QIcon icon(":/image/musicLogo.png");  // �滻Ϊ��ͼ���ʵ��·��
    w.setWindowIcon(icon);
    w.show();
    return a.exec();
}
