#include "kmainwindow.h"
#include <QApplication>
#include <QDebug>
#include <QFile>

int main(int argc, char* argv[])
{    
    QApplication a(argc, argv);        
    // 加载QSS文件
    //QFile file(":/qss/style.qss");
    //if (file.open(QFile::ReadOnly)) {
    //    QString styleSheet = QLatin1String(file.readAll());
    //    a.setStyleSheet(styleSheet);
    //}

    KMainWindow w;
    QIcon icon(":/image/musicLogo.png");  // 替换为您图标的实际路径
    w.setWindowIcon(icon);
    w.show();
    return a.exec();
}
