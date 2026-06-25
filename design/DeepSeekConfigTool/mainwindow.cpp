#include "mainwindow.h"
#include "deepseekapi.h"
#include "docxhandler.h"

#include <QDebug>
#include <QLineEdit>
#include <QTextEdit>
#include <QPushButton>
#include <QListWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDateTime>
#include <QFont>
#include <QFrame>
#include <QDirIterator>
#include <QFileDialog>
#include <QProgressBar>
#include <QMessageBox>
#include <QFileInfo>

namespace {
const char kDefaultApiUrl[] = "https://api.deepseek.com";

static QFont monoFont()
{
    QFont f("JetBrains Mono", 10);
    f.setStyleHint(QFont::Monospace);
    return f;
}

QString inputStyle(const QString &accentColor = "#89b4fa")
{
    return QString(
        "QLineEdit, QTextEdit { background: #1e1e2e; border: 1px solid #313244;"
        " border-radius: 4px; padding: 6px 8px; color: #cdd6f4; }"
        "QLineEdit:focus, QTextEdit:focus { border-color: %1; }")
        .arg(accentColor);
}

QString secondaryButtonStyle()
{
    return
        "QPushButton { background: #313244; color: #cdd6f4;"
        " border: 1px solid #45455a; border-radius: 4px;"
        " padding: 6px 14px; }"
        "QPushButton:hover { background: #45455a; }";
}
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_api(new DeepSeekAPI(this))
    , m_docx(new DocxHandler)
    , m_currentIndex(0)
    , m_successCount(0)
    , m_failCount(0)
    , m_batchRunning(false)
{
    setWindowTitle("DeepSeek 实验报告评语工具");
    resize(820, 720);
    setupUI();

    connect(m_api, &DeepSeekAPI::commentReady,
            this, &MainWindow::onApiResponse);
    connect(m_api, &DeepSeekAPI::errorOccurred,
            this, &MainWindow::onApiError);
}

void MainWindow::setupUI()
{
    auto *central = new QWidget(this);
    setCentralWidget(central);

    auto *mainLayout = new QVBoxLayout(central);
    mainLayout->setSpacing(8);
    mainLayout->setContentsMargins(12, 12, 12, 12);

    // 标题
    auto *title = new QLabel("DeepSeek 实验报告评语工具");
    title->setFont(QFont("Microsoft YaHei", 15, QFont::DemiBold));
    title->setStyleSheet("color: #cdd6f4;");
    mainLayout->addWidget(title);

    auto *subtitle = new QLabel("批量读取实验报告 → AI 生成评语 → 自动写入文档");
    subtitle->setStyleSheet("color: #a6adc8;");
    mainLayout->addWidget(subtitle);
    mainLayout->addSpacing(4);

    auto addRow = [&](const QString &labelText, QWidget *field) {
        auto *row = new QHBoxLayout;
        row->setSpacing(8);
        auto *lb = new QLabel(labelText);
        lb->setFixedWidth(80);
        lb->setFont(monoFont());
        lb->setStyleSheet("color: #a6adc8;");
        row->addWidget(lb);
        row->addWidget(field);
        mainLayout->addLayout(row);
        mainLayout->addSpacing(2);
    };

    auto makeInput = [&]() {
        auto *e = new QLineEdit;
        e->setFont(monoFont());
        e->setStyleSheet(inputStyle());
        return e;
    };

    // API 设置区域
    m_apiUrlEdit = makeInput();
    m_apiUrlEdit->setPlaceholderText(kDefaultApiUrl);
    m_apiUrlEdit->setText(kDefaultApiUrl);
    addRow("API 地址", m_apiUrlEdit);

    m_apiKeyEdit = makeInput();
    m_apiKeyEdit->setEchoMode(QLineEdit::Password);
    m_apiKeyEdit->setPlaceholderText("sk-xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    m_apiKeyEdit->setText("sk-a8d51729256449a2b5d0a51b1a76d761");
    addRow("API Key", m_apiKeyEdit);

    // 分隔线
    {
        auto *line = new QFrame;
        line->setFrameShape(QFrame::HLine);
        line->setStyleSheet("QFrame { color: #313244; max-height: 1px; }");
        mainLayout->addWidget(line);
        mainLayout->addSpacing(4);
    }

    // 文件夹选择
    {
        auto *row = new QHBoxLayout;
        row->setSpacing(6);
        m_folderEdit = new QLineEdit;
        m_folderEdit->setReadOnly(true);
        m_folderEdit->setPlaceholderText("选择包含 .docx 实验报告的文件夹...");
        m_folderEdit->setFont(monoFont());
        m_folderEdit->setStyleSheet(
            "QLineEdit { background: #1e1e2e; border: 1px solid #313244;"
            " border-radius: 4px; padding: 6px 8px; color: #fab387; }");
        m_browseBtn = new QPushButton("选择文件夹");
        m_browseBtn->setFont(monoFont());
        m_browseBtn->setStyleSheet(secondaryButtonStyle());
        row->addWidget(m_folderEdit, 1);
        row->addWidget(m_browseBtn);
        mainLayout->addLayout(row);
    }

    // 文件列表
    m_fileList = new QListWidget;
    m_fileList->setFont(monoFont());
    m_fileList->setMinimumHeight(80);
    m_fileList->setStyleSheet(
        "QListWidget { background: #1e1e2e; border: 1px solid #313244;"
        " border-radius: 4px; padding: 4px 0; }"
        "QListWidget::item { padding: 2px 8px; color: #a6e3a1; }"
        "QListWidget::item:selected { background: #313244; }");
    mainLayout->addWidget(m_fileList, 2);

    // 系统提示词
    {
        auto *row = new QHBoxLayout;
        row->setSpacing(8);

        auto *promptLabel = new QLabel("系统提示词");
        promptLabel->setFont(monoFont());
        promptLabel->setStyleSheet("color: #a6adc8;");
        row->addWidget(promptLabel);

        m_systemPromptEdit = new QTextEdit;
        m_systemPromptEdit->setFont(monoFont());
        m_systemPromptEdit->setPlaceholderText(
            "例：你是一位严谨的数据结构课程教师，请根据学生实验报告内容和成绩，"
            "撰写一段简明的评语（50-100字）。指出优点和不足。");
        m_systemPromptEdit->setMaximumHeight(60);
        m_systemPromptEdit->setStyleSheet(inputStyle());
        row->addWidget(m_systemPromptEdit, 1);

        mainLayout->addLayout(row);
    }

    // 进度条
    m_progressBar = new QProgressBar;
    m_progressBar->setFont(monoFont());
    m_progressBar->setRange(0, 100);
    m_progressBar->setValue(0);
    m_progressBar->setStyleSheet(
        "QProgressBar { background: #1e1e2e; border: 1px solid #313244;"
        " border-radius: 4px; text-align: center; color: #cdd6f4;"
        " padding: 2px; height: 18px; }"
        "QProgressBar::chunk { background: #89b4fa; border-radius: 3px; }");
    mainLayout->addWidget(m_progressBar);

    // 操作按钮
    {
        auto *row = new QHBoxLayout;
        row->addStretch();
        m_resetBtn = new QPushButton("重置");
        m_resetBtn->setFont(monoFont());
        m_resetBtn->setStyleSheet(secondaryButtonStyle());
        m_sendBtn = new QPushButton("开始批量处理");
        m_sendBtn->setFont(monoFont());
        m_sendBtn->setDefault(true);
        m_sendBtn->setStyleSheet(
            "QPushButton { background: #89b4fa; color: #1e1e2e;"
            " border: none; border-radius: 4px;"
            " padding: 8px 24px; font-weight: bold; font-size: 13px; }"
            "QPushButton:hover { background: #74c7ec; }"
            "QPushButton:disabled { background: #45455a; color: #6c7086; }");
        row->addWidget(m_resetBtn);
        row->addSpacing(8);
        row->addWidget(m_sendBtn);
        mainLayout->addLayout(row);
    }

    // 日志
    m_logEdit = new QTextEdit;
    m_logEdit->setReadOnly(true);
    m_logEdit->setPlaceholderText("运行日志...");
    m_logEdit->setFont(monoFont());
    m_logEdit->setMinimumHeight(100);
    m_logEdit->setStyleSheet(
        "QTextEdit { background: #11111b; border: 1px solid #313244;"
        " border-radius: 4px; padding: 6px 8px; color: #a6adc8; }");
    mainLayout->addWidget(m_logEdit, 2);

    // 信号连接
    connect(m_browseBtn, &QPushButton::clicked, this, &MainWindow::onBrowseFolder);
    connect(m_sendBtn,   &QPushButton::clicked, this, &MainWindow::onSend);
    connect(m_resetBtn,  &QPushButton::clicked, this, &MainWindow::onReset);

    // 默认提示词
    m_systemPromptEdit->setPlainText(
        "你是一位严谨的数据结构课程教师。请根据学生实验报告内容和成绩，"
        "撰写一段简明的评语（50-100字）。指出学生的优点和存在的不足，"
        "语言要客观、有建设性。不要使用「优点：」「不足：」「总体评价：」等"
        "小标题，直接写正文。不要提及具体分数。");
    logMessage("就绪：请填写 API Key，选择实验报告文件夹，然后点「开始批量处理」");
}

void MainWindow::onBrowseFolder()
{
    QString dir = QFileDialog::getExistingDirectory(this, "选择实验报告文件夹");
    if (dir.isEmpty())
        return;

    m_folderEdit->setText(dir);
    m_fileList->clear();

    QStringList files;
    QDirIterator it(dir, {"*.docx"}, QDir::Files, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        it.next();
        files << it.filePath();  // 保存完整路径
    }
    files.sort();

    for (const QString &f : files) {
        m_fileList->addItem("  " + QFileInfo(f).fileName());
    }

    logMessage(QString("找到 %1 个 .docx 文件").arg(files.size()));
}

void MainWindow::onSend()
{
    QString apiUrl = m_apiUrlEdit->text().trimmed();
    QString apiKey = m_apiKeyEdit->text().trimmed();
    if (apiUrl.isEmpty() || apiKey.isEmpty()) {
        logMessage("请先填写 API 地址和 API Key");
        return;
    }

    if (m_folderEdit->text().trimmed().isEmpty()) {
        logMessage("请先选择实验报告文件夹");
        return;
    }

    if (m_fileList->count() == 0) {
        logMessage("文件夹中没有 .docx 文件");
        return;
    }

    // 收集所有文件路径
    m_pendingFiles.clear();
    QString baseDir = m_folderEdit->text().trimmed();
    QDirIterator it(baseDir, {"*.docx"}, QDir::Files, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        it.next();
        m_pendingFiles << it.filePath();
    }
    m_pendingFiles.sort();

    if (m_pendingFiles.isEmpty()) {
        logMessage("没有找到 .docx 文件");
        return;
    }

    // 初始化批量处理状态
    m_api->setApiUrl(apiUrl);
    m_api->setApiKey(apiKey);
    m_currentIndex = 0;
    m_successCount = 0;
    m_failCount = 0;
    m_batchRunning = true;
    m_sendBtn->setEnabled(false);
    m_browseBtn->setEnabled(false);
    m_progressBar->setValue(0);
    m_progressBar->setRange(0, m_pendingFiles.size());

    logMessage(QString("开始批量处理 %1 个文件...").arg(m_pendingFiles.size()));
    startNextFile();
}

void MainWindow::startNextFile()
{
    if (m_currentIndex >= m_pendingFiles.size()) {
        finishBatch();
        return;
    }

    QString filePath = m_pendingFiles.at(m_currentIndex);
    QString fileName = QFileInfo(filePath).fileName();
    m_progressBar->setValue(m_currentIndex);
    logMessage(QString("[%1/%2] 正在处理: %3")
                   .arg(m_currentIndex + 1)
                   .arg(m_pendingFiles.size())
                   .arg(fileName));

    // 1. 从 .docx 提取文本
    bool ok = false;
    QString studentText = m_docx->extractText(filePath, &ok);
    if (!ok) {
        logMessage(QString("  ⚠ 读取失败: %1").arg(m_docx->lastError()));
        m_failCount++;
        m_currentIndex++;
        startNextFile();  // 跳过，处理下一个
        return;
    }

    if (studentText.trimmed().isEmpty()) {
        logMessage("  ⚠ 文档内容为空，跳过");
        m_failCount++;
        m_currentIndex++;
        startNextFile();
        return;
    }
    logMessage(QString("  ✓ 提取文本 %1 字：%2...")
                   .arg(studentText.length())
                   .arg(studentText.left(80)));

    // 2. 从文档中提取成绩（仅记录，不传给 AI）
    bool scoreOk = false;
    double score = m_docx->extractScore(filePath, &scoreOk);
    if (scoreOk)
        logMessage(QString("  ✓ 读取成绩: %1 分").arg(score, 0, 'f', 1));
    else
        logMessage("  ⚠ 未找到成绩");

    QString systemPrompt = m_systemPromptEdit->toPlainText().trimmed();
    if (systemPrompt.isEmpty()) {
        systemPrompt = "你是一位严谨的数据结构课程教师，请根据学生实验报告内容和成绩，"
                       "撰写一段简明的评语（50-100字）。说出优点和不足。"
                       "不要使用「优点：」「不足：」「总体评价：」等小标题，直接写正文。"
                       "不要提及具体分数。";
    }

    m_api->generateComment(systemPrompt, studentText);
    // AI 返回后通过 onApiResponse / onApiError 回调继续
}

void MainWindow::onApiResponse(const QString &reply)
{
    if (!m_batchRunning) {
        m_sendBtn->setEnabled(true);
        return;
    }

    QString filePath = m_pendingFiles.at(m_currentIndex);
    QString fileName = QFileInfo(filePath).fileName();
    logMessage(QString("  ✓ AI 回复: %1 ...").arg(reply.left(60)));

    // 3. 清理前缀，将评语写回 .docx
    QString clean = reply.trimmed();
    if (clean.startsWith("**评语：**"))
        clean = clean.mid(QString("**评语：**").size()).trimmed();
    else if (clean.startsWith("评语："))
        clean = clean.mid(QString("评语：").size()).trimmed();
    bool ok = m_docx->writeComment(filePath, clean);
    if (!ok) {
        logMessage(QString("  ⚠ 写入评语失败: %1").arg(m_docx->lastError()));
        m_failCount++;
    } else {
        logMessage(QString("  ✓ 评语已写入: %1").arg(fileName));
        m_successCount++;
    }

    // 继续处理下一个
    m_currentIndex++;
    startNextFile();
}

void MainWindow::onApiError(const QString &error)
{
    if (!m_batchRunning) {
        m_sendBtn->setEnabled(true);
        return;
    }

    QString filePath = m_pendingFiles.at(m_currentIndex);
    QString fileName = QFileInfo(filePath).fileName();
    logMessage(QString("  ⚠ AI 请求失败 [%1]: %2").arg(fileName, error));

    m_failCount++;
    m_currentIndex++;
    startNextFile();  // 跳过，继续下一个
}

void MainWindow::finishBatch()
{
    m_batchRunning = false;
    m_sendBtn->setEnabled(true);
    m_browseBtn->setEnabled(true);
    m_progressBar->setValue(m_pendingFiles.size());

    logMessage("");
    logMessage(QString("═══ 批量处理完成 ═══"));
    logMessage(QString("总计: %1 个文件 | 成功: %2 | 失败: %3")
                   .arg(m_pendingFiles.size())
                   .arg(m_successCount)
                   .arg(m_failCount));
    logMessage("所有评语已直接写入对应 .docx 文件中的「教师评语」位置。");
}

void MainWindow::onReset()
{
    m_batchRunning = false;
    m_currentIndex = 0;
    m_successCount = 0;
    m_failCount = 0;
    m_pendingFiles.clear();

    m_apiUrlEdit->setText(kDefaultApiUrl);
    m_apiKeyEdit->clear();
    m_folderEdit->clear();
    m_systemPromptEdit->clear();
    m_logEdit->clear();
    m_fileList->clear();
    m_progressBar->setValue(0);
    m_sendBtn->setEnabled(true);
    m_browseBtn->setEnabled(true);

    logMessage("已重置");
}

void MainWindow::logMessage(const QString &msg)
{
    QString ts = QDateTime::currentDateTime().toString("hh:mm:ss");
    m_logEdit->append(QString("[%1] %2").arg(ts, msg));
    qDebug().noquote() << QString("[%1] %2").arg(ts, msg);
}