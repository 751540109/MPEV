#include "emailparser.h"
#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QRegularExpression>
#include <QTemporaryFile>
#include <QDir>
#include <QCoreApplication>
#include <QUuid>
#include <QStringDecoder>

EmailParser::EmailParser(QObject *parent)
    : QObject(parent)
{
}

EmailParser::~EmailParser()
{
}

QList<Email> EmailParser::parseFile(const QString &filePath)
{
    QFileInfo fileInfo(filePath);
    
    if (!fileInfo.exists()) {
        return QList<Email>();
    }
    
    if (fileInfo.suffix().toLower() == "eml") {
        return parseEmlFile(filePath);
    } else if (fileInfo.suffix().toLower() == "mbox") {
        return parseMboxFile(filePath);
    }
    
    return QList<Email>();
}

QList<Email> EmailParser::parseEmlFile(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return QList<Email>();
    }
    
    QByteArray content = file.readAll();
    file.close();
    
    return parseEmlContent(content, filePath);
}

QList<Email> EmailParser::parseMboxFile(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return QList<Email>();
    }
    
    QList<Email> emails;
    int index = 0;
    
    while (!file.atEnd()) {
        QByteArray message = readMboxMessage(file);
        if (!message.isEmpty()) {
            QList<Email> parsedEmails = parseEmlContent(message, filePath);
            for (Email &email : parsedEmails) {
                email.id = generateEmailId(filePath, index++);
            }
            emails.append(parsedEmails);
        }
    }
    
    file.close();
    return emails;
}

QByteArray EmailParser::readMboxMessage(QFile &file)
{
    QByteArray message;
    bool foundStart = false;
    
    while (!file.atEnd()) {
        QByteArray line = file.readLine();
        
        // 检查是否是新消息的开始
        if (line.startsWith("From ")) {
            if (foundStart) {
                // 回退一行
                file.seek(file.pos() - line.size());
                break;
            }
            foundStart = true;
        }
        
        if (foundStart) {
            message.append(line);
        }
    }
    
    return message;
}

QList<Email> EmailParser::parseEmlContent(const QByteArray &content, const QString &filePath)
{
    QList<Email> emails;
    
    // 分离头部和正文
    int headerEnd = content.indexOf("\r\n\r\n");
    if (headerEnd == -1) {
        headerEnd = content.indexOf("\n\n");
    }
    
    if (headerEnd == -1) {
        return emails;
    }
    
    QByteArray header = content.left(headerEnd);
    QByteArray body = content.mid(headerEnd + 4);
    
    Email email;
    email.filePath = filePath;
    
    // 解析头部
    parseHeader(header, email);
    
    // 解析正文
    parseBody(body, email.htmlBody.isEmpty() ? "text/plain" : "text/html", email);
    
    // 解析附件
    parseAttachments(body, "multipart/mixed", email);
    
    emails.append(email);
    
    return emails;
}

void EmailParser::parseHeader(const QByteArray &header, Email &email)
{
    QString headerStr = QString::fromUtf8(header);
    QStringList lines = headerStr.split("\r\n", Qt::SkipEmptyParts);
    
    QString currentField;
    QString currentValue;
    
    for (const QString &line : lines) {
        if (line.startsWith(" ") || line.startsWith("\t")) {
            // 继续上一行
            currentValue += line.trimmed();
        } else {
            // 保存上一个字段
            if (!currentField.isEmpty()) {
                if (currentField.compare("From", Qt::CaseInsensitive) == 0) {
                    email.from = currentValue;
                } else if (currentField.compare("To", Qt::CaseInsensitive) == 0) {
                    email.to = currentValue;
                } else if (currentField.compare("Cc", Qt::CaseInsensitive) == 0) {
                    email.cc = currentValue;
                } else if (currentField.compare("Bcc", Qt::CaseInsensitive) == 0) {
                    email.bcc = currentValue;
                } else if (currentField.compare("Subject", Qt::CaseInsensitive) == 0) {
                    email.subject = currentValue;
                } else if (currentField.compare("Date", Qt::CaseInsensitive) == 0) {
                    email.date = parseDate(currentValue);
                }
            }
            
            // 解析新字段
            int colonPos = line.indexOf(':');
            if (colonPos != -1) {
                currentField = line.left(colonPos).trimmed();
                currentValue = line.mid(colonPos + 1).trimmed();
            }
        }
    }
    
    // 保存最后一个字段
    if (!currentField.isEmpty()) {
        if (currentField.compare("From", Qt::CaseInsensitive) == 0) {
            email.from = currentValue;
        } else if (currentField.compare("To", Qt::CaseInsensitive) == 0) {
            email.to = currentValue;
        } else if (currentField.compare("Cc", Qt::CaseInsensitive) == 0) {
            email.cc = currentValue;
        } else if (currentField.compare("Bcc", Qt::CaseInsensitive) == 0) {
            email.bcc = currentValue;
        } else if (currentField.compare("Subject", Qt::CaseInsensitive) == 0) {
            email.subject = currentValue;
        } else if (currentField.compare("Date", Qt::CaseInsensitive) == 0) {
            email.date = parseDate(currentValue);
        }
    }
    
    // 如果主题为空，设置默认主题
    if (email.subject.isEmpty()) {
        email.subject = "无主题";
    }
}

void EmailParser::parseBody(const QByteArray &body, const QString &contentType, Email &email)
{
    if (contentType.contains("text/html")) {
        email.htmlBody = QString::fromUtf8(body);
    } else {
        email.textBody = QString::fromUtf8(body);
    }
}

void EmailParser::parseAttachments(const QByteArray &body, const QString &contentType, Email &email)
{
    // 简化的附件解析
    // 实际实现需要更复杂的MIME解析
    Q_UNUSED(body);
    Q_UNUSED(contentType);
    Q_UNUSED(email);
}

QString EmailParser::generateEmailId(const QString &filePath, int index)
{
    QDateTime now = QDateTime::currentDateTime();
    return QString("%1_%2_%3")
        .arg(filePath)
        .arg(index)
        .arg(now.toMSecsSinceEpoch());
}

QString EmailParser::extractEmailFromAddress(const QString &address)
{
    QRegularExpression rx("<([^>]+)>");
    QRegularExpressionMatch match = rx.match(address);
    if (match.hasMatch()) {
        return match.captured(1);
    }
    return address.trimmed();
}

QString EmailParser::extractNameFromAddress(const QString &address)
{
    QRegularExpression rx("^([^<]+)<");
    QRegularExpressionMatch match = rx.match(address);
    if (match.hasMatch()) {
        return match.captured(1).trimmed();
    }
    return QString();
}

QDateTime EmailParser::parseDate(const QString &dateString)
{
    // 简化的日期解析
    QDateTime dateTime = QDateTime::fromString(dateString, "ddd, dd MMM yyyy HH:mm:ss");
    if (dateTime.isValid()) {
        return dateTime;
    }
    
    dateTime = QDateTime::fromString(dateString, "dd MMM yyyy HH:mm:ss");
    if (dateTime.isValid()) {
        return dateTime;
    }
    
    return QDateTime::currentDateTime();
}

QString EmailParser::decodeQuotedPrintable(const QByteArray &data)
{
    QString result;
    int i = 0;
    
    while (i < data.size()) {
        if (data[i] == '=') {
            if (i + 2 < data.size()) {
                bool ok;
                int hex = data.mid(i + 1, 2).toInt(&ok, 16);
                if (ok) {
                    result += QChar(hex);
                    i += 3;
                    continue;
                }
            }
            result += '=';
        } else if (data[i] == '_') {
            result += ' ';
        } else {
            result += QChar(data[i]);
        }
        i++;
    }
    
    return result;
}

QString EmailParser::decodeBase64(const QByteArray &data)
{
    QByteArray decoded = QByteArray::fromBase64(data);
    return QString::fromUtf8(decoded);
}

QByteArray EmailParser::decodeContentTransferEncoding(const QByteArray &data, const QString &encoding)
{
    if (encoding.compare("quoted-printable", Qt::CaseInsensitive) == 0) {
        return decodeQuotedPrintable(data).toUtf8();
    } else if (encoding.compare("base64", Qt::CaseInsensitive) == 0) {
        return QByteArray::fromBase64(data);
    }
    
    return data;
}

QString EmailParser::fixEncoding(const QString &text, const QString &charset)
{
    if (charset.isEmpty() || charset.compare("utf-8", Qt::CaseInsensitive) == 0) {
        return text;
    }
    
    // 尝试使用指定编码重新解码
    QByteArray data = text.toUtf8();
    QStringDecoder decoder(charset.toUtf8().constData());
    if (decoder.isValid()) {
        return decoder(data);
    }
    
    return text;
}
