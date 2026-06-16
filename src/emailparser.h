#ifndef EMAILPARSER_H
#define EMAILPARSER_H

#include <QObject>
#include <QList>
#include <QDateTime>
#include <QByteArray>
#include <QString>
#include <QStringList>
#include <QFile>
#include <QFileInfo>

struct Attachment {
    QString name;
    qint64 size;
    QString tempPath;
    QString mimeType;
};

struct Email {
    QString id;
    QString from;
    QString to;
    QString cc;
    QString bcc;
    QString subject;
    QDateTime date;
    QString textBody;
    QString htmlBody;
    QList<Attachment> attachments;
    QString filePath;
    bool isStarred;
    QString conversationId;
    
    Email() : isStarred(false) {}
};

struct Contact {
    QString name;
    QString email;
    int emailCount;
    
    Contact() : emailCount(0) {}
};

struct EmailHistory {
    QString filePath;
    QDateTime openTime;
    QDateTime lastBrowseTime;
    QString lastEmailId;
    bool isClosed;
    
    EmailHistory() : isClosed(false) {}
};

struct StarredEmail {
    QString emailId;
    QString filePath;
    QDateTime starredTime;
};

struct Conversation {
    QString id;
    QString subject;
    QList<Email> emails;
    QDateTime lastEmailTime;
};

class EmailParser : public QObject
{
    Q_OBJECT

public:
    explicit EmailParser(QObject *parent = nullptr);
    ~EmailParser();
    
    QList<Email> parseFile(const QString &filePath);
    QList<Email> parseEmlFile(const QString &filePath);
    QList<Email> parseMboxFile(const QString &filePath);
    
    static QString generateEmailId(const QString &filePath, int index);
    static QString extractEmailFromAddress(const QString &address);
    static QString extractNameFromAddress(const QString &address);
    static QDateTime parseDate(const QString &dateString);
    static QString decodeQuotedPrintable(const QByteArray &data);
    static QString decodeBase64(const QByteArray &data);
    static QByteArray decodeContentTransferEncoding(const QByteArray &data, const QString &encoding);

private:
    QList<Email> parseEmlContent(const QByteArray &content, const QString &filePath);
    void parseHeader(const QByteArray &header, Email &email);
    void parseBody(const QByteArray &body, const QString &contentType, Email &email);
    void parseAttachments(const QByteArray &body, const QString &contentType, Email &email);
    QByteArray readMboxMessage(QFile &file);
    
    static QString fixEncoding(const QString &text, const QString &charset);
};

#endif // EMAILPARSER_H
