#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDateTime>
#include <QVariant>
#include <QList>
#include <QString>

#include "emailparser.h"

class Database : public QObject
{
    Q_OBJECT

public:
    static Database& instance();
    
    bool init();
    void close();
    
    // 浏览记录
    bool addBrowseHistory(const QString &filePath, const QDateTime &openTime);
    bool updateBrowseHistory(const QString &filePath, const QDateTime &lastBrowseTime, const QString &lastEmailId);
    bool closeBrowseHistory(const QString &filePath);
    QList<EmailHistory> getBrowseHistory();
    bool deleteBrowseHistory(const QString &filePath);
    bool clearBrowseHistory();
    
    // 星标邮件
    bool addStarredEmail(const QString &emailId, const QString &filePath, const QDateTime &starredTime);
    bool removeStarredEmail(const QString &emailId);
    bool isStarredEmail(const QString &emailId);
    QList<StarredEmail> getStarredEmails();
    
    // 配置
    QVariant getConfig(const QString &key, const QVariant &defaultValue = QVariant());
    bool setConfig(const QString &key, const QVariant &value);
    bool clearConfig();
    
private:
    explicit Database(QObject *parent = nullptr);
    ~Database();
    
    bool createTables();
    bool createBrowseHistoryTable();
    bool createStarredEmailsTable();
    bool createConfigTable();
    
    QSqlDatabase db;
    static Database *instance_;
};

#endif // DATABASE_H
