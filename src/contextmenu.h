#ifndef CONTEXTMENU_H
#define CONTEXTMENU_H

#include <QObject>
#include <QMenu>
#include <QAction>
#include <QModelIndex>
#include <QPoint>

class ContextMenuManager : public QObject
{
    Q_OBJECT

public:
    explicit ContextMenuManager(QObject *parent = nullptr);
    ~ContextMenuManager();
    
    // 邮件右键菜单
    QMenu* createEmailContextMenu(const QModelIndex &index);
    
    // 文件右键菜单
    QMenu* createFileContextMenu(const QModelIndex &index);
    
    // 附件右键菜单
    QMenu* createAttachmentContextMenu(const QModelIndex &index);
    
    // 时间线右键菜单
    QMenu* createTimelineContextMenu(const QModelIndex &index);

signals:
    // 邮件操作信号
    void openEmail(const QModelIndex &index);
    void starEmail(const QModelIndex &index);
    void unstarEmail(const QModelIndex &index);
    void exportEmail(const QModelIndex &index);
    void exportAttachments(const QModelIndex &index);
    void moveToCategory(const QModelIndex &index);
    void deleteEmail(const QModelIndex &index);
    void viewEmailDetails(const QModelIndex &index);
    void addToTimeline(const QModelIndex &index);
    
    // 文件操作信号
    void closeFile(const QModelIndex &index);
    void reloadFile(const QModelIndex &index);
    void exportAllEmails(const QModelIndex &index);
    void deleteHistory(const QModelIndex &index);
    
    // 附件操作信号
    void previewAttachment(const QModelIndex &index, int attachmentIndex);
    void saveAttachment(const QModelIndex &index, int attachmentIndex);
    void copyAttachmentPath(const QModelIndex &index, int attachmentIndex);
    
    // 时间线操作信号
    void filterByTime(const QModelIndex &index);
    void viewStarredByTime(const QModelIndex &index);
    void removeFromTimeline(const QModelIndex &index);

private:
    void setupEmailContextMenu(QMenu *menu, const QModelIndex &index);
    void setupFileContextMenu(QMenu *menu, const QModelIndex &index);
    void setupAttachmentContextMenu(QMenu *menu, const QModelIndex &index);
    void setupTimelineContextMenu(QMenu *menu, const QModelIndex &index);
};

#endif // CONTEXTMENU_H
