#include "database.h"
#include <QStandardPaths>
#include <QDir>
#include <QSqlRecord>
#include <QDebug>

Database *Database::instance_ = nullptr;

Database::Database(QObject *parent)
    : QObject(parent)
{
}

Database::~Database()
{
    close();
}

Database& Database::instance()
{
    if (!instance_) {
        instance_ = new Database();
    }
    return *instance_;
}

bool Database::init()
{
    QString dataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(dataPath);
    
    QString dbPath = dataPath + "/mpev.db";
    
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(dbPath);
    
    if (!db.open()) {
        qDebug() << "数据库打开失败:" << db.lastError().text();
        return false;
    }
    
    return createTables();
}

void Database::close()
{
    if (db.isOpen()) {
        db.close();
    }
}

bool Database::createTables()
{
    bool success = true;
    success &= createBrowseHistoryTable();
    success &= createStarredEmailsTable();
    success &= createConfigTable();
    return success;
}

bool Database::createBrowseHistoryTable()
{
    QSqlQuery query;
    bool success = query.exec(
        "CREATE TABLE IF NOT EXISTS browse_history ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "file_path TEXT NOT NULL,"
        "open_time DATETIME NOT NULL,"
        "last_browse_time DATETIME,"
        "last_email_id TEXT,"
        "is_closed BOOLEAN DEFAULT FALSE"
        ")"
    );
    
    if (!success) {
        qDebug() << "创建浏览记录表失败:" << query.lastError().text();
    }
    
    return success;
}

bool Database::createStarredEmailsTable()
{
    bool success;
    QSqlQuery query;
    success = query.exec(
        "CREATE TABLE IF NOT EXISTS starred_emails ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "email_id TEXT NOT NULL UNIQUE,"
        "file_path TEXT NOT NULL,"
        "starred_time DATETIME NOT NULL"
        ")"
    );
    
    if (!success) {
        qDebug() << "创建星标邮件表失败:" << query.lastError().text();
    }
    
    return success;
}

bool Database::createConfigTable()
{
    bool success;
    QSqlQuery query;
    success = query.exec(
        "CREATE TABLE IF NOT EXISTS config ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "key TEXT NOT NULL UNIQUE,"
        "value TEXT"
        ")"
    );
    
    if (!success) {
        qDebug() << "创建配置表失败:" << query.lastError().text();
    }
    
    return success;
}

// 浏览记录操作
bool Database::addBrowseHistory(const QString &filePath, const QDateTime &openTime)
{
    QSqlQuery query;
    query.prepare(
        "INSERT INTO browse_history (file_path, open_time, is_closed) "
        "VALUES (:file_path, :open_time, FALSE)"
    );
    query.bindValue(":file_path", filePath);
    query.bindValue(":open_time", openTime);
    
    if (!query.exec()) {
        qDebug() << "添加浏览记录失败:" << query.lastError().text();
        return false;
    }
    
    return true;
}

bool Database::updateBrowseHistory(const QString &filePath, const QDateTime &lastBrowseTime, const QString &lastEmailId)
{
    QSqlQuery query;
    query.prepare(
        "UPDATE browse_history "
        "SET last_browse_time = :last_browse_time, last_email_id = :last_email_id "
        "WHERE file_path = :file_path AND is_closed = FALSE"
    );
    query.bindValue(":last_browse_time", lastBrowseTime);
    query.bindValue(":last_email_id", lastEmailId);
    query.bindValue(":file_path", filePath);
    
    if (!query.exec()) {
        qDebug() << "更新浏览记录失败:" << query.lastError().text();
        return false;
    }
    
    return true;
}

bool Database::closeBrowseHistory(const QString &filePath)
{
    QSqlQuery query;
    query.prepare(
        "UPDATE browse_history "
        "SET is_closed = TRUE "
        "WHERE file_path = :file_path AND is_closed = FALSE"
    );
    query.bindValue(":file_path", filePath);
    
    if (!query.exec()) {
        qDebug() << "关闭浏览记录失败:" << query.lastError().text();
        return false;
    }
    
    return true;
}

QList<EmailHistory> Database::getBrowseHistory()
{
    QList<EmailHistory> historyList;
    
    QSqlQuery query;
    query.prepare(
        "SELECT file_path, open_time, last_browse_time, last_email_id, is_closed "
        "FROM browse_history "
        "ORDER BY last_browse_time DESC"
    );
    
    if (query.exec()) {
        while (query.next()) {
            EmailHistory history;
            history.filePath = query.value("file_path").toString();
            history.openTime = query.value("open_time").toDateTime();
            history.lastBrowseTime = query.value("last_browse_time").toDateTime();
            history.lastEmailId = query.value("last_email_id").toString();
            history.isClosed = query.value("is_closed").toBool();
            historyList.append(history);
        }
    }
    
    return historyList;
}

bool Database::deleteBrowseHistory(const QString &filePath)
{
    QSqlQuery query;
    query.prepare("DELETE FROM browse_history WHERE file_path = :file_path");
    query.bindValue(":file_path", filePath);
    
    if (!query.exec()) {
        qDebug() << "删除浏览记录失败:" << query.lastError().text();
        return false;
    }
    
    return true;
}

bool Database::clearBrowseHistory()
{
    QSqlQuery query;
    if (!query.exec("DELETE FROM browse_history")) {
        qDebug() << "清空浏览记录失败:" << query.lastError().text();
        return false;
    }
    
    return true;
}

// 星标邮件操作
bool Database::addStarredEmail(const QString &emailId, const QString &filePath, const QDateTime &starredTime)
{
    QSqlQuery query;
    query.prepare(
        "INSERT OR REPLACE INTO starred_emails (email_id, file_path, starred_time) "
        "VALUES (:email_id, :file_path, :starred_time)"
    );
    query.bindValue(":email_id", emailId);
    query.bindValue(":file_path", filePath);
    query.bindValue(":starred_time", starredTime);
    
    if (!query.exec()) {
        qDebug() << "添加星标邮件失败:" << query.lastError().text();
        return false;
    }
    
    return true;
}

bool Database::removeStarredEmail(const QString &emailId)
{
    QSqlQuery query;
    query.prepare("DELETE FROM starred_emails WHERE email_id = :email_id");
    query.bindValue(":email_id", emailId);
    
    if (!query.exec()) {
        qDebug() << "移除星标邮件失败:" << query.lastError().text();
        return false;
    }
    
    return true;
}

bool Database::isStarredEmail(const QString &emailId)
{
    QSqlQuery query;
    query.prepare("SELECT COUNT(*) FROM starred_emails WHERE email_id = :email_id");
    query.bindValue(":email_id", emailId);
    
    if (query.exec() && query.next()) {
        return query.value(0).toInt() > 0;
    }
    
    return false;
}

QList<StarredEmail> Database::getStarredEmails()
{
    QList<StarredEmail> starredList;
    
    QSqlQuery query;
    query.prepare(
        "SELECT email_id, file_path, starred_time "
        "FROM starred_emails "
        "ORDER BY starred_time DESC"
    );
    
    if (query.exec()) {
        while (query.next()) {
            StarredEmail starred;
            starred.emailId = query.value("email_id").toString();
            starred.filePath = query.value("file_path").toString();
            starred.starredTime = query.value("starred_time").toDateTime();
            starredList.append(starred);
        }
    }
    
    return starredList;
}

// 配置操作
QVariant Database::getConfig(const QString &key, const QVariant &defaultValue)
{
    QSqlQuery query;
    query.prepare("SELECT value FROM config WHERE key = :key");
    query.bindValue(":key", key);
    
    if (query.exec() && query.next()) {
        return query.value("value");
    }
    
    return defaultValue;
}

bool Database::setConfig(const QString &key, const QVariant &value)
{
    QSqlQuery query;
    query.prepare(
        "INSERT OR REPLACE INTO config (key, value) "
        "VALUES (:key, :value)"
    );
    query.bindValue(":key", key);
    query.bindValue(":value", value.toString());
    
    if (!query.exec()) {
        qDebug() << "设置配置失败:" << query.lastError().text();
        return false;
    }
    
    return true;
}

bool Database::clearConfig()
{
    QSqlQuery query;
    if (!query.exec("DELETE FROM config")) {
        qDebug() << "清空配置失败:" << query.lastError().text();
        return false;
    }
    
    return true;
}
