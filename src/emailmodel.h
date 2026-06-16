#ifndef EMAILMODEL_H
#define EMAILMODEL_H

#include <QAbstractTableModel>
#include <QList>
#include <QModelIndex>
#include <QVariant>
#include <QColor>
#include <QFont>
#include <QIcon>

#include "emailparser.h"

class EmailModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit EmailModel(QObject *parent = nullptr);
    ~EmailModel();
    
    // 表格模型接口
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    
    // 数据操作
    void setEmails(const QList<Email> &emails);
    void updateEmail(int row, const Email &email);
    void removeEmail(int row);
    void addEmail(const Email &email);
    
    // 获取邮件
    Email getEmail(int row) const;
    QList<Email> getEmails() const;
    
    // 列枚举
    enum Column {
        ColumnStar = 0,
        ColumnSubject,
        ColumnFrom,
        ColumnTo,
        ColumnDate,
        ColumnAttachment,
        ColumnCount
    };

signals:
    void starChanged(const QModelIndex &index, bool starred);
    void emailDeleted(const QModelIndex &index);

private:
    QList<Email> emails;
};

#endif // EMAILMODEL_H
