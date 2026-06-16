#include "contextmenu.h"
#include <QMenu>
#include <QAction>

ContextMenuManager::ContextMenuManager(QObject *parent)
    : QObject(parent)
{
}

ContextMenuManager::~ContextMenuManager()
{
}

QMenu* ContextMenuManager::createEmailContextMenu(const QModelIndex &index)
{
    QMenu *menu = new QMenu();
    setupEmailContextMenu(menu, index);
    return menu;
}

QMenu* ContextMenuManager::createFileContextMenu(const QModelIndex &index)
{
    QMenu *menu = new QMenu();
    setupFileContextMenu(menu, index);
    return menu;
}

QMenu* ContextMenuManager::createAttachmentContextMenu(const QModelIndex &index)
{
    QMenu *menu = new QMenu();
    setupAttachmentContextMenu(menu, index);
    return menu;
}

QMenu* ContextMenuManager::createTimelineContextMenu(const QModelIndex &index)
{
    QMenu *menu = new QMenu();
    setupTimelineContextMenu(menu, index);
    return menu;
}

void ContextMenuManager::setupEmailContextMenu(QMenu *menu, const QModelIndex &index)
{
    // 打开邮件
    QAction *openAction = menu->addAction("打开邮件");
    connect(openAction, &QAction::triggered, this, [this, index]() {
        emit openEmail(index);
    });
    
    menu->addSeparator();
    
    // 添加星标
    QAction *starAction = menu->addAction("添加星标");
    connect(starAction, &QAction::triggered, this, [this, index]() {
        emit starEmail(index);
    });
    
    // 取消星标
    QAction *unstarAction = menu->addAction("取消星标");
    connect(unstarAction, &QAction::triggered, this, [this, index]() {
        emit unstarEmail(index);
    });
    
    menu->addSeparator();
    
    // 导出邮件
    QAction *exportAction = menu->addAction("导出邮件");
    connect(exportAction, &QAction::triggered, this, [this, index]() {
        emit exportEmail(index);
    });
    
    // 导出附件
    QAction *exportAttachmentsAction = menu->addAction("导出附件");
    connect(exportAttachmentsAction, &QAction::triggered, this, [this, index]() {
        emit exportAttachments(index);
    });
    
    menu->addSeparator();
    
    // 移动到分类
    QAction *moveAction = menu->addAction("移动到分类");
    connect(moveAction, &QAction::triggered, this, [this, index]() {
        emit moveToCategory(index);
    });
    
    // 删除邮件
    QAction *deleteAction = menu->addAction("删除邮件");
    connect(deleteAction, &QAction::triggered, this, [this, index]() {
        emit deleteEmail(index);
    });
    
    menu->addSeparator();
    
    // 查看邮件详情
    QAction *detailsAction = menu->addAction("查看邮件详情");
    connect(detailsAction, &QAction::triggered, this, [this, index]() {
        emit viewEmailDetails(index);
    });
    
    // 添加到时间线
    QAction *timelineAction = menu->addAction("添加到时间线");
    connect(timelineAction, &QAction::triggered, this, [this, index]() {
        emit addToTimeline(index);
    });
}

void ContextMenuManager::setupFileContextMenu(QMenu *menu, const QModelIndex &index)
{
    // 关闭文件
    QAction *closeAction = menu->addAction("关闭文件");
    connect(closeAction, &QAction::triggered, this, [this, index]() {
        emit closeFile(index);
    });
    
    // 重新加载
    QAction *reloadAction = menu->addAction("重新加载");
    connect(reloadAction, &QAction::triggered, this, [this, index]() {
        emit reloadFile(index);
    });
    
    menu->addSeparator();
    
    // 导出所有邮件
    QAction *exportAllAction = menu->addAction("导出所有邮件");
    connect(exportAllAction, &QAction::triggered, this, [this, index]() {
        emit exportAllEmails(index);
    });
    
    // 删除浏览记录
    QAction *deleteHistoryAction = menu->addAction("删除浏览记录");
    connect(deleteHistoryAction, &QAction::triggered, this, [this, index]() {
        emit deleteHistory(index);
    });
}

void ContextMenuManager::setupAttachmentContextMenu(QMenu *menu, const QModelIndex &index)
{
    // 预览附件
    QAction *previewAction = menu->addAction("预览附件");
    connect(previewAction, &QAction::triggered, this, [this, index]() {
        emit previewAttachment(index, 0);
    });
    
    // 保存附件
    QAction *saveAction = menu->addAction("保存附件");
    connect(saveAction, &QAction::triggered, this, [this, index]() {
        emit saveAttachment(index, 0);
    });
    
    // 复制附件路径
    QAction *copyPathAction = menu->addAction("复制附件路径");
    connect(copyPathAction, &QAction::triggered, this, [this, index]() {
        emit copyAttachmentPath(index, 0);
    });
}

void ContextMenuManager::setupTimelineContextMenu(QMenu *menu, const QModelIndex &index)
{
    // 筛选该时间邮件
    QAction *filterAction = menu->addAction("筛选该时间邮件");
    connect(filterAction, &QAction::triggered, this, [this, index]() {
        emit filterByTime(index);
    });
    
    // 查看该时间星标邮件
    QAction *viewStarredAction = menu->addAction("查看该时间星标邮件");
    connect(viewStarredAction, &QAction::triggered, this, [this, index]() {
        emit viewStarredByTime(index);
    });
    
    menu->addSeparator();
    
    // 移除该时间邮件（从时间线中）
    QAction *removeAction = menu->addAction("移除该时间邮件（从时间线中）");
    connect(removeAction, &QAction::triggered, this, [this, index]() {
        emit removeFromTimeline(index);
    });
}
