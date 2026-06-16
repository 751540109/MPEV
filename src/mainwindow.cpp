#include "mainwindow.h"
#include "settingsdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QToolBar>
#include <QMenuBar>
#include <QStatusBar>
#include <QApplication>
#include <QClipboard>
#include <QMessageBox>
#include <QFileDialog>
#include <QSettings>
#include <QTimer>
#include <QDir>
#include <QStandardPaths>
#include <QDesktopServices>
#include <QUrl>
#include <QCloseEvent>
#include <QSplitter>
#include <QHeaderView>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , currentEmailIndex(-1)
    , timelineWindow(nullptr)
{
    setupUI();
    setupMenuBar();
    setupToolBar();
    setupStatusBar();
    setupConnections();
    loadSettings();
    
    setWindowTitle("MPEV - 邮件管理软件");
    setMinimumSize(1200, 800);
    
    // 初始化邮件模型
    emailModel = new EmailModel(this);
    emailTableView->setModel(emailModel);
    
    // 设置右键菜单
    emailTableView->setContextMenuPolicy(Qt::CustomContextMenu);
    fileTreeView->setContextMenuPolicy(Qt::CustomContextMenu);
    attachmentListView->setContextMenuPolicy(Qt::CustomContextMenu);
    
    // 初始化时间线窗口
    timelineWindow = new TimelineWindow(this);
    
    // 初始化右键菜单
    emailContextMenu = new QMenu(this);
    fileContextMenu = new QMenu(this);
    attachmentContextMenu = new QMenu(this);
    
    // 添加邮件右键菜单项
    emailContextMenu->addAction("打开邮件", this, [this]() {
        QModelIndex index = emailTableView->currentIndex();
        if (index.isValid()) {
            onEmailClicked(index);
        }
    });
    
    emailContextMenu->addAction("添加星标", this, [this]() {
        QModelIndex index = emailTableView->currentIndex();
        if (index.isValid()) {
            onStarChanged(index, true);
        }
    });
    
    emailContextMenu->addAction("取消星标", this, [this]() {
        QModelIndex index = emailTableView->currentIndex();
        if (index.isValid()) {
            onStarChanged(index, false);
        }
    });
    
    emailContextMenu->addSeparator();
    
    emailContextMenu->addAction("导出邮件", this, [this]() {
        QModelIndex index = emailTableView->currentIndex();
        if (index.isValid()) {
            onEmailExportRequested(index);
        }
    });
    
    emailContextMenu->addAction("导出附件", this, [this]() {
        QModelIndex index = emailTableView->currentIndex();
        if (index.isValid()) {
            // 导出所有附件
            for (int i = 0; i < emails[index.row()].attachments.size(); ++i) {
                onAttachmentExportRequested(index, i);
            }
        }
    });
    
    emailContextMenu->addSeparator();
    
    emailContextMenu->addAction("移动到分类", this, [this]() {
        // TODO: 实现移动到分类功能
    });
    
    emailContextMenu->addAction("删除邮件", this, [this]() {
        QModelIndex index = emailTableView->currentIndex();
        if (index.isValid()) {
            onEmailDeleted(index);
        }
    });
    
    emailContextMenu->addSeparator();
    
    emailContextMenu->addAction("查看邮件详情", this, [this]() {
        QModelIndex index = emailTableView->currentIndex();
        if (index.isValid()) {
            onEmailClicked(index);
        }
    });
    
    emailContextMenu->addAction("添加到时间线", this, [this]() {
        QModelIndexList indexes = emailTableView->selectionModel()->selectedIndexes();
        if (!indexes.isEmpty()) {
            onTimelineRequested(indexes);
        }
    });
    
    // 添加文件右键菜单项
    fileContextMenu->addAction("关闭文件", this, [this]() {
        // TODO: 实现关闭文件功能
    });
    
    fileContextMenu->addAction("重新加载", this, [this]() {
        // TODO: 实现重新加载功能
    });
    
    fileContextMenu->addSeparator();
    
    fileContextMenu->addAction("导出所有邮件", this, [this]() {
        // TODO: 实现导出所有邮件功能
    });
    
    fileContextMenu->addAction("删除浏览记录", this, [this]() {
        // TODO: 实现删除浏览记录功能
    });
    
    // 添加附件右键菜单项
    attachmentContextMenu->addAction("预览附件", this, [this]() {
        QModelIndex index = attachmentListView->currentIndex();
        if (index.isValid() && currentEmailIndex >= 0) {
            // 使用系统默认程序打开附件
            QString filePath = emails[currentEmailIndex].attachments[index.row()].tempPath;
            QDesktopServices::openUrl(QUrl::fromLocalFile(filePath));
        }
    });
    
    attachmentContextMenu->addAction("保存附件", this, [this]() {
        QModelIndex index = attachmentListView->currentIndex();
        if (index.isValid() && currentEmailIndex >= 0) {
            onAttachmentExportRequested(emailTableView->currentIndex(), index.row());
        }
    });
    
    attachmentContextMenu->addAction("复制附件路径", this, [this]() {
        QModelIndex index = attachmentListView->currentIndex();
        if (index.isValid() && currentEmailIndex >= 0) {
            QString filePath = emails[currentEmailIndex].attachments[index.row()].tempPath;
            QApplication::clipboard()->setText(filePath);
        }
    });
}

MainWindow::~MainWindow()
{
    saveSettings();
}

void MainWindow::setupUI()
{
    // 主分割器
    mainSplitter = new QSplitter(Qt::Horizontal, this);
    
    // 左侧分割器
    leftSplitter = new QSplitter(Qt::Vertical, this);
    
    // 文件树视图
    fileTreeView = new QTreeView(this);
    fileTreeView->setHeaderHidden(true);
    fileTreeView->setMinimumWidth(200);
    
    // 联系人树视图
    contactTreeView = new QTreeView(this);
    contactTreeView->setHeaderHidden(true);
    contactTreeView->setMinimumWidth(200);
    
    // 星标列表视图
    starredListView = new QListView(this);
    starredListView->setMinimumWidth(200);
    
    // 历史记录列表视图
    historyListView = new QListView(this);
    historyListView->setMinimumWidth(200);
    
    // 将视图添加到左侧分割器
    leftSplitter->addWidget(fileTreeView);
    leftSplitter->addWidget(contactTreeView);
    leftSplitter->addWidget(starredListView);
    leftSplitter->addWidget(historyListView);
    
    // 右侧分割器
    rightSplitter = new QSplitter(Qt::Vertical, this);
    
    // 邮件表格视图
    emailTableView = new QTableView(this);
    emailTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    emailTableView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    emailTableView->horizontalHeader()->setStretchLastSection(true);
    emailTableView->verticalHeader()->hide();
    emailTableView->setMinimumHeight(300);
    
    // 邮件预览浏览器
    emailPreviewBrowser = new QTextBrowser(this);
    emailPreviewBrowser->setOpenExternalLinks(true);
    emailPreviewBrowser->setMinimumHeight(200);
    
    // 附件列表视图
    attachmentListView = new QListView(this);
    attachmentListView->setMinimumHeight(100);
    
    // 将视图添加到右侧分割器
    rightSplitter->addWidget(emailTableView);
    rightSplitter->addWidget(emailPreviewBrowser);
    rightSplitter->addWidget(attachmentListView);
    
    // 中间功能区
    centerStack = new QStackedWidget(this);
    
    // 普通模式视图
    normalModeWidget = new QWidget(this);
    QVBoxLayout *normalLayout = new QVBoxLayout(normalModeWidget);
    timelineButton = new QPushButton("时间线", normalModeWidget);
    timelineButton->setFixedWidth(100);
    normalLayout->addWidget(timelineButton);
    normalLayout->addStretch();
    
    // 对话模式视图
    conversationModeWidget = new QWidget(this);
    QVBoxLayout *conversationLayout = new QVBoxLayout(conversationModeWidget);
    conversationLayout->addStretch();
    
    centerStack->addWidget(normalModeWidget);
    centerStack->addWidget(conversationModeWidget);
    
    // 添加到主分割器
    mainSplitter->addWidget(leftSplitter);
    mainSplitter->addWidget(centerStack);
    mainSplitter->addWidget(rightSplitter);
    
    // 设置分割器比例
    mainSplitter->setSizes({200, 100, 900});
    leftSplitter->setSizes({200, 200, 200, 200});
    rightSplitter->setSizes({300, 200, 100});
    
    setCentralWidget(mainSplitter);
}

void MainWindow::setupMenuBar()
{
    // 文件菜单
    QMenu *fileMenu = menuBar()->addMenu("文件(&F)");
    fileMenu->addAction("打开文件(&O)", this, QOverload<>::of(&MainWindow::openFile), QKeySequence::Open);
    fileMenu->addAction("保存邮件(&S)", this, &MainWindow::saveEmail, QKeySequence::Save);
    fileMenu->addAction("导出邮件(&E)", this, &MainWindow::exportEmail);
    fileMenu->addSeparator();
    fileMenu->addAction("设置(&T)", this, &MainWindow::showSettings);
    fileMenu->addSeparator();
    fileMenu->addAction("退出(&X)", qApp, &QApplication::quit, QKeySequence::Quit);
    
    // 编辑菜单
    QMenu *editMenu = menuBar()->addMenu("编辑(&E)");
    editMenu->addAction("搜索(&F)", this, [this]() {
        searchLineEdit->setFocus();
    }, QKeySequence::Find);
    editMenu->addAction("高级搜索(&A)", this, &MainWindow::onAdvancedSearch);
    
    // 视图菜单
    QMenu *viewMenu = menuBar()->addMenu("视图(&V)");
    viewMenu->addAction("普通模式", this, [this]() {
        viewModeComboBox->setCurrentIndex(0);
    });
    viewMenu->addAction("对话模式", this, [this]() {
        viewModeComboBox->setCurrentIndex(1);
    });
    viewMenu->addSeparator();
    viewMenu->addAction("时间线", this, &MainWindow::showTimelineWindow);
    
    // 帮助菜单
    QMenu *helpMenu = menuBar()->addMenu("帮助(&H)");
    helpMenu->addAction("关于(&A)", this, &MainWindow::showAbout);
}

void MainWindow::setupToolBar()
{
    QToolBar *toolBar = addToolBar("工具栏");
    toolBar->setMovable(false);
    
    // 搜索框
    searchLineEdit = new QLineEdit(this);
    searchLineEdit->setPlaceholderText("搜索邮件...");
    searchLineEdit->setMaximumWidth(300);
    toolBar->addWidget(searchLineEdit);
    
    // 高级搜索按钮
    advancedSearchButton = new QPushButton("高级搜索", this);
    toolBar->addWidget(advancedSearchButton);
    
    toolBar->addSeparator();
    
    // 视图模式选择
    viewModeComboBox = new QComboBox(this);
    viewModeComboBox->addItem("普通模式");
    viewModeComboBox->addItem("对话模式");
    viewModeComboBox->setMaximumWidth(100);
    toolBar->addWidget(viewModeComboBox);
}

void MainWindow::setupStatusBar()
{
    statusBar()->showMessage("就绪");
}

void MainWindow::setupConnections()
{
    // 连接信号和槽
    connect(fileTreeView, &QTreeView::clicked, this, &MainWindow::onFileClicked);
    connect(contactTreeView, &QTreeView::clicked, this, &MainWindow::onContactClicked);
    connect(starredListView, &QListView::clicked, this, &MainWindow::onStarredClicked);
    connect(historyListView, &QListView::clicked, this, &MainWindow::onHistoryClicked);
    
    connect(emailTableView, &QTableView::clicked, this, &MainWindow::onEmailClicked);
    
    connect(searchLineEdit, &QLineEdit::textChanged, this, &MainWindow::onSearchTextChanged);
    connect(advancedSearchButton, &QPushButton::clicked, this, &MainWindow::onAdvancedSearch);
    
    connect(viewModeComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int index) {
        centerStack->setCurrentIndex(index);
    });
    
    connect(timelineButton, &QPushButton::clicked, this, &MainWindow::showTimelineWindow);
    
    // 右键菜单连接
    connect(emailTableView, &QTableView::customContextMenuRequested, this, &MainWindow::showEmailContextMenu);
    connect(fileTreeView, &QTreeView::customContextMenuRequested, this, &MainWindow::showFileContextMenu);
    connect(attachmentListView, &QListView::customContextMenuRequested, this, &MainWindow::showAttachmentContextMenu);
}

void MainWindow::loadSettings()
{
    settings = new QSettings("MPEV", "MPEV", this);
    
    // 加载窗口几何信息
    restoreGeometry(settings->value("geometry").toByteArray());
    restoreState(settings->value("windowState").toByteArray());
    
    // 加载最近打开的文件
    updateRecentFiles();
}

void MainWindow::saveSettings()
{
    settings->setValue("geometry", saveGeometry());
    settings->setValue("windowState", saveState());
}

void MainWindow::updateRecentFiles()
{
    QStringList recentFiles = settings->value("recentFiles").toStringList();
    // TODO: 更新最近文件列表
}

void MainWindow::openFile()
{
    QString filePath = QFileDialog::getOpenFileName(
        this,
        "打开邮件文件",
        QDir::homePath(),
        "邮件文件 (*.eml *.mbox);;EML文件 (*.eml);;MBOX文件 (*.mbox);;所有文件 (*.*)"
    );
    
    if (!filePath.isEmpty()) {
        openFileInternal(filePath);
    }
}

void MainWindow::openFile(const QString &filePath)
{
    openFileInternal(filePath);
}

void MainWindow::openFileInternal(const QString &filePath)
{
    statusBar()->showMessage("正在打开文件: " + filePath);
    
    // 解析文件
    EmailParser parser;
    QList<Email> parsedEmails = parser.parseFile(filePath);
    
    if (parsedEmails.isEmpty()) {
        QMessageBox::warning(this, "警告", "无法解析文件或文件为空");
        statusBar()->showMessage("就绪");
        return;
    }
    
    // 更新邮件列表
    emails = parsedEmails;
    emailModel->setEmails(emails);
    
    // 更新当前文件路径
    currentFilePath = filePath;
    
    // 更新浏览记录
    Database::instance().addBrowseHistory(filePath, QDateTime::currentDateTime());
    
    // 更新最近文件列表
    QStringList recentFiles = settings->value("recentFiles").toStringList();
    recentFiles.removeAll(filePath);
    recentFiles.prepend(filePath);
    while (recentFiles.size() > 10) {
        recentFiles.takeLast();
    }
    settings->setValue("recentFiles", recentFiles);
    
    statusBar()->showMessage("已加载 " + QString::number(emails.size()) + " 封邮件");
}

void MainWindow::saveEmail()
{
    if (currentEmailIndex < 0 || currentEmailIndex >= emails.size()) {
        QMessageBox::warning(this, "警告", "请先选择一封邮件");
        return;
    }
    
    QString filePath = QFileDialog::getSaveFileName(
        this,
        "保存邮件",
        QDir::homePath() + "/" + emails[currentEmailIndex].subject + ".eml",
        "EML文件 (*.eml)"
    );
    
    if (!filePath.isEmpty()) {
        // TODO: 实现保存邮件功能
        QMessageBox::information(this, "提示", "邮件已保存");
    }
}

void MainWindow::exportEmail()
{
    // TODO: 实现导出邮件功能
}

void MainWindow::showSettings()
{
    SettingsDialog dialog(this);
    dialog.exec();
}

void MainWindow::showAbout()
{
    QMessageBox::about(
        this,
        "关于 MPEV",
        "MPEV - 邮件管理软件\n\n"
        "版本: 1.0.0\n"
        "一款免费、易用、现代化的邮件管理工具\n"
        "支持EML和MBOX格式邮件的解析、预览和管理\n\n"
        "© 2026 MPEV"
    );
}

void MainWindow::onFileClicked(const QModelIndex &index)
{
    // TODO: 实现文件点击处理
}

void MainWindow::onEmailClicked(const QModelIndex &index)
{
    if (!index.isValid() || index.row() >= emails.size()) {
        return;
    }
    
    currentEmailIndex = index.row();
    Email &email = emails[currentEmailIndex];
    
    // 更新邮件预览
    QString previewContent;
    if (!email.htmlBody.isEmpty()) {
        previewContent = email.htmlBody;
    } else {
        previewContent = "<pre>" + email.textBody + "</pre>";
    }
    
    emailPreviewBrowser->setHtml(previewContent);
    
    // 更新附件列表
    QStringList attachmentNames;
    for (const Attachment &attachment : email.attachments) {
        attachmentNames << attachment.name;
    }
    
    QStandardItemModel *attachmentModel = new QStandardItemModel(this);
    for (const QString &name : attachmentNames) {
        attachmentModel->appendRow(new QStandardItem(name));
    }
    attachmentListView->setModel(attachmentModel);
    
    // 更新浏览记录
    Database::instance().updateBrowseHistory(
        currentFilePath,
        QDateTime::currentDateTime(),
        email.id
    );
    
    statusBar()->showMessage("已选择邮件: " + email.subject);
}

void MainWindow::onContactClicked(const QModelIndex &index)
{
    // TODO: 实现联系人点击处理
}

void MainWindow::onStarredClicked(const QModelIndex &index)
{
    // TODO: 实现星标点击处理
}

void MainWindow::onHistoryClicked(const QModelIndex &index)
{
    // TODO: 实现历史记录点击处理
}

void MainWindow::onStarChanged(const QModelIndex &index, bool starred)
{
    if (!index.isValid() || index.row() >= emails.size()) {
        return;
    }
    
    Email &email = emails[index.row()];
    email.isStarred = starred;
    
    if (starred) {
        Database::instance().addStarredEmail(
            email.id,
            currentFilePath,
            QDateTime::currentDateTime()
        );
    } else {
        Database::instance().removeStarredEmail(email.id);
    }
    
    // 更新邮件模型
    emailModel->updateEmail(index.row(), email);
}

void MainWindow::onEmailDeleted(const QModelIndex &index)
{
    if (!index.isValid() || index.row() >= emails.size()) {
        return;
    }
    
    QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        "确认删除",
        "确定要删除这封邮件吗？",
        QMessageBox::Yes | QMessageBox::No
    );
    
    if (reply == QMessageBox::Yes) {
        // TODO: 实现邮件删除功能
        emails.removeAt(index.row());
        emailModel->removeEmail(index.row());
    }
}

void MainWindow::showContextMenu(const QPoint &pos)
{
    emailContextMenu->exec(emailTableView->viewport()->mapToGlobal(pos));
}

void MainWindow::showFileContextMenu(const QPoint &pos)
{
    fileContextMenu->exec(fileTreeView->viewport()->mapToGlobal(pos));
}

void MainWindow::showEmailContextMenu(const QPoint &pos)
{
    QModelIndex index = emailTableView->indexAt(pos);
    if (index.isValid()) {
        emailTableView->setCurrentIndex(index);
        emailContextMenu->exec(emailTableView->viewport()->mapToGlobal(pos));
    }
}

void MainWindow::showAttachmentContextMenu(const QPoint &pos)
{
    QModelIndex index = attachmentListView->indexAt(pos);
    if (index.isValid()) {
        attachmentListView->setCurrentIndex(index);
        attachmentContextMenu->exec(attachmentListView->viewport()->mapToGlobal(pos));
    }
}

void MainWindow::onSearchTextChanged(const QString &text)
{
    // TODO: 实现实时搜索功能
    statusBar()->showMessage("搜索: " + text);
}

void MainWindow::onAdvancedSearch()
{
    // TODO: 实现高级搜索功能
    QMessageBox::information(this, "提示", "高级搜索功能开发中...");
}

void MainWindow::onTimelineRequested(const QModelIndexList &indexes)
{
    QList<Email> timelineEmails;
    for (const QModelIndex &index : indexes) {
        if (index.isValid() && index.row() < emails.size()) {
            timelineEmails << emails[index.row()];
        }
    }
    
    timelineWindow->setEmails(timelineEmails);
    timelineWindow->show();
}

void MainWindow::showTimelineWindow()
{
    timelineWindow->setEmails(emails);
    timelineWindow->show();
}

void MainWindow::onThemeChanged(int theme)
{
    // TODO: 实现主题切换功能
}

void MainWindow::onZoomChanged(double zoom)
{
    // TODO: 实现缩放功能
}

void MainWindow::onEmailExportRequested(const QModelIndex &index)
{
    if (!index.isValid() || index.row() >= emails.size()) {
        return;
    }
    
    QString filePath = QFileDialog::getSaveFileName(
        this,
        "导出邮件",
        QDir::homePath() + "/" + emails[index.row()].subject + ".eml",
        "EML文件 (*.eml)"
    );
    
    if (!filePath.isEmpty()) {
        // TODO: 实现邮件导出功能
        QMessageBox::information(this, "提示", "邮件已导出");
    }
}

void MainWindow::onAttachmentExportRequested(const QModelIndex &index, int attachmentIndex)
{
    if (!index.isValid() || index.row() >= emails.size()) {
        return;
    }
    
    Email &email = emails[index.row()];
    if (attachmentIndex >= email.attachments.size()) {
        return;
    }
    
    Attachment &attachment = email.attachments[attachmentIndex];
    
    QString filePath = QFileDialog::getSaveFileName(
        this,
        "保存附件",
        QDir::homePath() + "/" + attachment.name,
        "所有文件 (*.*)"
    );
    
    if (!filePath.isEmpty()) {
        // TODO: 实现附件保存功能
        QMessageBox::information(this, "提示", "附件已保存");
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    saveSettings();
    event->accept();
}
