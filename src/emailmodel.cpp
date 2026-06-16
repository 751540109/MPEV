#include "emailmodel.h"
#include <QIcon>
#include <QColor>
#include <QFont>

EmailModel::EmailModel(QObject *parent)
    : QAbstractTableModel(parent)
{
}

EmailModel::~EmailModel()
{
}

int EmailModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return emails.size();
}

int EmailModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return ColumnCount;
}

QVariant EmailModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= emails.size()) {
        return QVariant();
    }
    
    const Email &email = emails[index.row()];
    
    if (role == Qt::DisplayRole) {
        switch (index.column()) {
            case ColumnStar:
                return email.isStarred ? "★" : "☆";
            case ColumnSubject:
                return email.subject;
            case ColumnFrom:
                return email.from;
            case ColumnTo:
                return email.to;
            case ColumnDate:
                return email.date.toString("yyyy-MM-dd HH:mm");
            case ColumnAttachment:
                return email.attachments.isEmpty() ? "" : QString::number(email.attachments.size()) + "个附件";
            default:
                return QVariant();
        }
    } else if (role == Qt::ForegroundRole) {
        if (email.isStarred) {
            return QColor(Qt::yellow);
        }
    } else if (role == Qt::FontRole) {
        if (email.isStarred) {
            QFont font;
            font.setBold(true);
            return font;
        }
    } else if (role == Qt::TextAlignmentRole) {
        if (index.column() == ColumnStar || index.column() == ColumnAttachment) {
            return QVariant(Qt::AlignCenter);
        }
    } else if (role == Qt::ToolTipRole) {
        if (index.column() == ColumnStar) {
            return email.isStarred ? "取消星标" : "添加星标";
        }
    }
    
    return QVariant();
}

QVariant EmailModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole) {
        return QVariant();
    }
    
    switch (section) {
        case ColumnStar:
            return "星标";
        case ColumnSubject:
            return "主题";
        case ColumnFrom:
            return "发件人";
        case ColumnTo:
            return "收件人";
        case ColumnDate:
            return "时间";
        case ColumnAttachment:
            return "附件";
        default:
            return QVariant();
    }
}

void EmailModel::setEmails(const QList<Email> &newEmails)
{
    beginResetModel();
    emails = newEmails;
    endResetModel();
}

void EmailModel::updateEmail(int row, const Email &email)
{
    if (row >= 0 && row < emails.size()) {
        emails[row] = email;
        QModelIndex topLeft = index(row, 0);
        QModelIndex bottomRight = index(row, ColumnCount - 1);
        emit dataChanged(topLeft, bottomRight);
    }
}

void EmailModel::removeEmail(int row)
{
    if (row >= 0 && row < emails.size()) {
        beginRemoveRows(QModelIndex(), row, row);
        emails.removeAt(row);
        endRemoveRows();
    }
}

void EmailModel::addEmail(const Email &email)
{
    beginInsertRows(QModelIndex(), emails.size(), emails.size());
    emails.append(email);
    endInsertRows();
}

Email EmailModel::getEmail(int row) const
{
    if (row >= 0 && row < emails.size()) {
        return emails[row];
    }
    return Email();
}

QList<Email> EmailModel::getEmails() const
{
    return emails;
}
