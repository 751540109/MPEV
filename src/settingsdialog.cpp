#include "settingsdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QMessageBox>
#include <QSettings>
#include <QDir>
#include <QFileDialog>
#include <QStandardPaths>

SettingsDialog::SettingsDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUI();
    loadSettings();
    
    setWindowTitle("设置");
    setMinimumSize(500, 400);
    resize(600, 500);
}

SettingsDialog::~SettingsDialog()
{
}

void SettingsDialog::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    // 选项卡
    tabWidget = new QTabWidget(this);
    
    // 界面设置选项卡
    appearanceTab = new QWidget(this);
    QFormLayout *appearanceLayout = new QFormLayout(appearanceTab);
    
    themeComboBox = new QComboBox(appearanceTab);
    themeComboBox->addItem("浅色");
    themeComboBox->addItem("深色");
    themeComboBox->addItem("跟随系统");
    appearanceLayout->addRow("主题:", themeComboBox);
    
    zoomSlider = new QSlider(Qt::Horizontal, appearanceTab);
    zoomSlider->setRange(50, 200);
    zoomSlider->setValue(100);
    zoomLabel = new QLabel("100%", appearanceTab);
    
    QHBoxLayout *zoomLayout = new QHBoxLayout();
    zoomLayout->addWidget(zoomSlider);
    zoomLayout->addWidget(zoomLabel);
    appearanceLayout->addRow("缩放比例:", zoomLayout);
    
    fontComboBox = new QFontComboBox(appearanceTab);
    appearanceLayout->addRow("字体:", fontComboBox);
    
    fontSizeSpinBox = new QSpinBox(appearanceTab);
    fontSizeSpinBox->setRange(8, 24);
    fontSizeSpinBox->setValue(10);
    appearanceLayout->addRow("字体大小:", fontSizeSpinBox);
    
    tabWidget->addTab(appearanceTab, "界面");
    
    // 文件设置选项卡
    fileTab = new QWidget(this);
    QFormLayout *fileLayout = new QFormLayout(fileTab);
    
    QHBoxLayout *defaultPathLayout = new QHBoxLayout();
    defaultPathEdit = new QLineEdit(fileTab);
    browseDefaultPathButton = new QPushButton("浏览...", fileTab);
    defaultPathLayout->addWidget(defaultPathEdit);
    defaultPathLayout->addWidget(browseDefaultPathButton);
    fileLayout->addRow("默认打开路径:", defaultPathLayout);
    
    QHBoxLayout *exportPathLayout = new QHBoxLayout();
    exportPathEdit = new QLineEdit(fileTab);
    browseExportPathButton = new QPushButton("浏览...", fileTab);
    exportPathLayout->addWidget(exportPathEdit);
    exportPathLayout->addWidget(browseExportPathButton);
    fileLayout->addRow("导出文件路径:", exportPathLayout);
    
    autoSaveHistoryCheckBox = new QCheckBox("自动保存浏览记录", fileTab);
    autoSaveHistoryCheckBox->setChecked(true);
    fileLayout->addRow(autoSaveHistoryCheckBox);
    
    associateFilesCheckBox = new QCheckBox("关联EML/MBOX文件", fileTab);
    fileLayout->addRow(associateFilesCheckBox);
    
    tabWidget->addTab(fileTab, "文件");
    
    // 解析设置选项卡
    parseTab = new QWidget(this);
    QFormLayout *parseLayout = new QFormLayout(parseTab);
    
    encodingComboBox = new QComboBox(parseTab);
    encodingComboBox->addItem("UTF-8");
    encodingComboBox->addItem("GB2312");
    encodingComboBox->addItem("BIG5");
    encodingComboBox->addItem("自动检测");
    parseLayout->addRow("默认编码:", encodingComboBox);
    
    autoFixEncodingCheckBox = new QCheckBox("自动修复乱码", parseTab);
    autoFixEncodingCheckBox->setChecked(true);
    parseLayout->addRow(autoFixEncodingCheckBox);
    
    cacheSizeSpinBox = new QSpinBox(parseTab);
    cacheSizeSpinBox->setRange(1, 100);
    cacheSizeSpinBox->setValue(10);
    cacheSizeSpinBox->setSuffix(" MB");
    parseLayout->addRow("MBOX缓存大小:", cacheSizeSpinBox);
    
    tabWidget->addTab(parseTab, "解析");
    
    // 其他设置选项卡
    otherTab = new QWidget(this);
    QFormLayout *otherLayout = new QFormLayout(otherTab);
    
    showTimelineCheckBox = new QCheckBox("显示时间线", otherTab);
    showTimelineCheckBox->setChecked(true);
    otherLayout->addRow(showTimelineCheckBox);
    
    defaultConversationModeCheckBox = new QCheckBox("默认显示对话模式", otherTab);
    otherLayout->addRow(defaultConversationModeCheckBox);
    
    searchHistorySpinBox = new QSpinBox(otherTab);
    searchHistorySpinBox->setRange(10, 100);
    searchHistorySpinBox->setValue(50);
    otherLayout->addRow("搜索历史保存数量:", searchHistorySpinBox);
    
    autoStartCheckBox = new QCheckBox("开机自启动", otherTab);
    otherLayout->addRow(autoStartCheckBox);
    
    tabWidget->addTab(otherTab, "其他");
    
    mainLayout->addWidget(tabWidget);
    
    // 按钮
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    
    resetButton = new QPushButton("恢复默认", this);
    okButton = new QPushButton("确定", this);
    cancelButton = new QPushButton("取消", this);
    
    buttonLayout->addWidget(resetButton);
    buttonLayout->addStretch();
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);
    
    mainLayout->addLayout(buttonLayout);
    
    // 连接信号和槽
    connect(okButton, &QPushButton::clicked, this, &SettingsDialog::onAccepted);
    connect(cancelButton, &QPushButton::clicked, this, &SettingsDialog::onRejected);
    connect(resetButton, &QPushButton::clicked, this, &SettingsDialog::onResetDefaults);
    connect(browseDefaultPathButton, &QPushButton::clicked, this, &SettingsDialog::onBrowseDefaultPath);
    connect(browseExportPathButton, &QPushButton::clicked, this, &SettingsDialog::onBrowseExportPath);
    connect(zoomSlider, &QSlider::valueChanged, this, [this](int value) {
        zoomLabel->setText(QString("%1%").arg(value));
    });
}

void SettingsDialog::loadSettings()
{
    QSettings settings("MPEV", "MPEV");
    
    // 加载界面设置
    int theme = settings.value("theme", 0).toInt();
    themeComboBox->setCurrentIndex(theme);
    
    int zoom = settings.value("zoom", 100).toInt();
    zoomSlider->setValue(zoom);
    zoomLabel->setText(QString("%1%").arg(zoom));
    
    QString fontFamily = settings.value("fontFamily", "Segoe UI").toString();
    fontComboBox->setCurrentFont(QFont(fontFamily));
    
    int fontSize = settings.value("fontSize", 10).toInt();
    fontSizeSpinBox->setValue(fontSize);
    
    // 加载文件设置
    QString defaultPath = settings.value("defaultPath", QDir::homePath()).toString();
    defaultPathEdit->setText(defaultPath);
    
    QString exportPath = settings.value("exportPath", QDir::homePath()).toString();
    exportPathEdit->setText(exportPath);
    
    bool autoSaveHistory = settings.value("autoSaveHistory", true).toBool();
    autoSaveHistoryCheckBox->setChecked(autoSaveHistory);
    
    bool associateFiles = settings.value("associateFiles", false).toBool();
    associateFilesCheckBox->setChecked(associateFiles);
    
    // 加载解析设置
    int encoding = settings.value("encoding", 0).toInt();
    encodingComboBox->setCurrentIndex(encoding);
    
    bool autoFixEncoding = settings.value("autoFixEncoding", true).toBool();
    autoFixEncodingCheckBox->setChecked(autoFixEncoding);
    
    int cacheSize = settings.value("cacheSize", 10).toInt();
    cacheSizeSpinBox->setValue(cacheSize);
    
    // 加载其他设置
    bool showTimeline = settings.value("showTimeline", true).toBool();
    showTimelineCheckBox->setChecked(showTimeline);
    
    bool defaultConversationMode = settings.value("defaultConversationMode", false).toBool();
    defaultConversationModeCheckBox->setChecked(defaultConversationMode);
    
    int searchHistory = settings.value("searchHistory", 50).toInt();
    searchHistorySpinBox->setValue(searchHistory);
    
    bool autoStart = settings.value("autoStart", false).toBool();
    autoStartCheckBox->setChecked(autoStart);
}

void SettingsDialog::saveSettings()
{
    QSettings settings("MPEV", "MPEV");
    
    // 保存界面设置
    settings.setValue("theme", themeComboBox->currentIndex());
    settings.setValue("zoom", zoomSlider->value());
    settings.setValue("fontFamily", fontComboBox->currentFont().family());
    settings.setValue("fontSize", fontSizeSpinBox->value());
    
    // 保存文件设置
    settings.setValue("defaultPath", defaultPathEdit->text());
    settings.setValue("exportPath", exportPathEdit->text());
    settings.setValue("autoSaveHistory", autoSaveHistoryCheckBox->isChecked());
    settings.setValue("associateFiles", associateFilesCheckBox->isChecked());
    
    // 保存解析设置
    settings.setValue("encoding", encodingComboBox->currentIndex());
    settings.setValue("autoFixEncoding", autoFixEncodingCheckBox->isChecked());
    settings.setValue("cacheSize", cacheSizeSpinBox->value());
    
    // 保存其他设置
    settings.setValue("showTimeline", showTimelineCheckBox->isChecked());
    settings.setValue("defaultConversationMode", defaultConversationModeCheckBox->isChecked());
    settings.setValue("searchHistory", searchHistorySpinBox->value());
    settings.setValue("autoStart", autoStartCheckBox->isChecked());
}

void SettingsDialog::resetToDefaults()
{
    themeComboBox->setCurrentIndex(0);
    zoomSlider->setValue(100);
    zoomLabel->setText("100%");
    fontComboBox->setCurrentFont(QFont("Segoe UI"));
    fontSizeSpinBox->setValue(10);
    
    defaultPathEdit->setText(QDir::homePath());
    exportPathEdit->setText(QDir::homePath());
    autoSaveHistoryCheckBox->setChecked(true);
    associateFilesCheckBox->setChecked(false);
    
    encodingComboBox->setCurrentIndex(0);
    autoFixEncodingCheckBox->setChecked(true);
    cacheSizeSpinBox->setValue(10);
    
    showTimelineCheckBox->setChecked(true);
    defaultConversationModeCheckBox->setChecked(false);
    searchHistorySpinBox->setValue(50);
    autoStartCheckBox->setChecked(false);
}

void SettingsDialog::onAccepted()
{
    saveSettings();
    accept();
}

void SettingsDialog::onRejected()
{
    reject();
}

void SettingsDialog::onResetDefaults()
{
    QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        "确认重置",
        "确定要恢复默认设置吗？",
        QMessageBox::Yes | QMessageBox::No
    );
    
    if (reply == QMessageBox::Yes) {
        resetToDefaults();
    }
}

void SettingsDialog::onBrowseDefaultPath()
{
    QString path = QFileDialog::getExistingDirectory(
        this,
        "选择默认打开路径",
        defaultPathEdit->text()
    );
    
    if (!path.isEmpty()) {
        defaultPathEdit->setText(path);
    }
}

void SettingsDialog::onBrowseExportPath()
{
    QString path = QFileDialog::getExistingDirectory(
        this,
        "选择导出文件路径",
        exportPathEdit->text()
    );
    
    if (!path.isEmpty()) {
        exportPathEdit->setText(path);
    }
}
