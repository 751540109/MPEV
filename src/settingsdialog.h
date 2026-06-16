#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QTabWidget>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QComboBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QLineEdit>
#include <QPushButton>
#include <QGroupBox>
#include <QSlider>
#include <QFontComboBox>

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = nullptr);
    ~SettingsDialog();

private slots:
    void onAccepted();
    void onRejected();
    void onResetDefaults();
    void onBrowseDefaultPath();
    void onBrowseExportPath();

private:
    void setupUI();
    void loadSettings();
    void saveSettings();
    void resetToDefaults();
    
    // UI组件
    QTabWidget *tabWidget;
    
    // 界面设置
    QWidget *appearanceTab;
    QComboBox *themeComboBox;
    QSlider *zoomSlider;
    QLabel *zoomLabel;
    QFontComboBox *fontComboBox;
    QSpinBox *fontSizeSpinBox;
    
    // 文件设置
    QWidget *fileTab;
    QLineEdit *defaultPathEdit;
    QPushButton *browseDefaultPathButton;
    QLineEdit *exportPathEdit;
    QPushButton *browseExportPathButton;
    QCheckBox *autoSaveHistoryCheckBox;
    QCheckBox *associateFilesCheckBox;
    
    // 解析设置
    QWidget *parseTab;
    QComboBox *encodingComboBox;
    QCheckBox *autoFixEncodingCheckBox;
    QSpinBox *cacheSizeSpinBox;
    
    // 其他设置
    QWidget *otherTab;
    QCheckBox *showTimelineCheckBox;
    QCheckBox *defaultConversationModeCheckBox;
    QSpinBox *searchHistorySpinBox;
    QCheckBox *autoStartCheckBox;
    
    // 按钮
    QPushButton *okButton;
    QPushButton *cancelButton;
    QPushButton *resetButton;
};

#endif // SETTINGSDIALOG_H
