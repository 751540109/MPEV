#include <QApplication>
#include <QStyleFactory>
#include <QDir>
#include <QStandardPaths>
#include "mainwindow.h"
#include "database.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("MPEV");
    app.setOrganizationName("MPEV");
    app.setApplicationVersion("1.0.0");
    
    // 设置应用样式
    app.setStyle(QStyleFactory::create("Fusion"));
    
    // 初始化数据库
    Database::instance().init();
    
    // 创建并显示主窗口
    MainWindow window;
    window.show();
    
    return app.exec();
}
