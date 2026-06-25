#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStringList>

class QLineEdit;
class QTextEdit;
class QPushButton;
class QListWidget;
class QProgressBar;
class DeepSeekAPI;
class DocxHandler;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override = default;

private slots:
    void onBrowseFolder();
    void onSend();             // 开始批量处理
    void onReset();
    void onApiResponse(const QString &reply);
    void onApiError(const QString &error);

private:
    void setupUI();
    void logMessage(const QString &msg);
    void startNextFile();      // 处理下一个文件
    void finishBatch();        // 完成所有文件处理

    // UI
    QLineEdit      *m_apiUrlEdit = nullptr;
    QLineEdit      *m_apiKeyEdit = nullptr;
    QLineEdit      *m_folderEdit = nullptr;
    QPushButton    *m_browseBtn = nullptr;
    QListWidget    *m_fileList = nullptr;
    QTextEdit      *m_systemPromptEdit = nullptr;  // 系统提示词
    QPushButton    *m_sendBtn = nullptr;
    QPushButton    *m_resetBtn = nullptr;
    QProgressBar   *m_progressBar = nullptr;
    QTextEdit      *m_logEdit = nullptr;

    // backend
    DeepSeekAPI  *m_api = nullptr;
    DocxHandler  *m_docx = nullptr;

    // 批量处理状态
    QStringList    m_pendingFiles;     // 待处理的文件完整路径列表
    int            m_currentIndex;     // 当前正在处理的文件索引
    int            m_successCount;     // 成功处理数
    int            m_failCount;        // 失败处理数
    bool           m_batchRunning;     // 是否正在批量处理中
};

#endif // MAINWINDOW_H