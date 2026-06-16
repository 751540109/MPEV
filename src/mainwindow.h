#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSplitter>
#include <QListWidget>
#include <QStackedWidget>
#include <QTreeView>
#include <QTableView>
#include <QTextBrowser>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QMenu>
#include <QAction>
#include <QMessageBox>
#include <QFileDialog>
#include <QSettings>
#include <QTimer>
#include <QComboBox>
#include <QListView>
#include <QStandardItemModel>

#include "emailparser.h"
#include "database.h"
#include "emailmodel.h"
#include "timeline.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void openFile();
    void openFile(const QString &filePath);
    void saveEmail();
    void exportEmail();
    void showSettings();
    void showAbout();
    
    void onFileClicked(const QModelIndex &index);
    void onEmailClicked(const QModelIndex &index);
    void onContactClicked(const QModelIndex &index);
    void onStarredClicked(const QModelIndex &index);
    void onHistoryClicked(const QModelIndex &index);
    
    void onStarChanged(const QModelIndex &index, bool starred);
    void onEmailDeleted(const QModelIndex &index);
    
    void showContextMenu(const QPoint &pos);
    void showFileContextMenu(const QPoint &pos);
    void showEmailContextMenu(const QPoint &pos);
    void showAttachmentContextMenu(const QPoint &pos);
    
    void onSearchTextChanged(const QString &text);
    void onAdvancedSearch();
    
    void onTimelineRequested(const QModelIndexList &indexes);
    void showTimelineWindow();
    
    void onThemeChanged(int theme);
    void onZoomChanged(double zoom);
    
    void onEmailExportRequested(const QModelIndex &index);
    void onAttachmentExportRequested(const QModelIndex &index, int attachmentIndex);

private:
    void setupUI();
    void setupMenuBar();
    void setupToolBar();
    void setupStatusBar();
    void setupConnections();
    void loadSettings();
    void saveSettings();
    void updateRecentFiles();
    void openFileInternal(const QString &filePath);
    
    // UI组件
    QSplitter *mainSplitter;
    QSplitter *leftSplitter;
    QSplitter *rightSplitter;
    
    // 左侧导航区
    QTreeView *fileTreeView;
    QTreeView *contactTreeView;
    QListView *starredListView;
    QListView *historyListView;
    
    // 中间功能区
    QStackedWidget *centerStack;
    QWidget *normalModeWidget;
    QWidget *conversationModeWidget;
    QPushButton *timelineButton;
    
    // 右侧操作区
    QTableView *emailTableView;
    QTextBrowser *emailPreviewBrowser;
    QListView *attachmentListView;
    
    // 顶部控件
    QLineEdit *searchLineEdit;
    QPushButton *advancedSearchButton;
    QComboBox *viewModeComboBox;
    
    // 模型
    EmailModel *emailModel;
    
    // 数据
    QList<Email> emails;
    QList<Contact> contacts;
    QList<EmailHistory> history;
    QList<StarredEmail> starredEmails;
    QList<Conversation> conversations;
    
    // 当前状态
    QString currentFilePath;
    int currentEmailIndex;
    QSettings *settings;
    
    // 时间线窗口
    TimelineWindow *timelineWindow;
    
    // 右键菜单
    QMenu *emailContextMenu;
    QMenu *fileContextMenu;
    QMenu *attachmentContextMenu;
};

#endif // MAINWINDOW_H
