#include "docxhandler.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QProcess>
#include <QTextStream>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QRandomGenerator>
#include <QDebug>
#include <QRegularExpression>

namespace {
const char kDocxTempPrefix[] = "deepseek_docx_";
const char kWordNS[]        = "http://schemas.openxmlformats.org/wordprocessingml/2006/main";
}

DocxHandler::DocxHandler() = default;

// ═══════════════════════════════════════════════
//  内部：解压 → 读 document.xml 返回字符串
// ═══════════════════════════════════════════════
QString DocxHandler::readDocxXml(const QString &docxPath, bool *ok, QString *tempDirOut)
{
    QString tempDir = unzipToTemp(docxPath, ok);
    if (!*ok) return {};

    QString docPath = tempDir + "/word/document.xml";
    QFile file(docPath);
    if (!file.open(QIODevice::ReadOnly)) {
        m_lastError = QString("无法打开 %1").arg(docPath);
        *ok = false;
        removeDir(tempDir);
        return {};
    }

    QString xml = QString::fromUtf8(file.readAll());
    file.close();

    if (tempDirOut)
        *tempDirOut = tempDir;
    else
        removeDir(tempDir);

    return xml;
}

// ═══════════════════════════════════════════════
//  提取文本
// ═══════════════════════════════════════════════
QString DocxHandler::extractText(const QString &docxPath, bool *ok)
{
    bool dummy;
    if (!ok) ok = &dummy;

    QString tempDir;
    QString xml = readDocxXml(docxPath, ok, &tempDir);
    if (!*ok) return {};

    // 直接用 <w:t> 标签提取文本，比 QXmlStreamReader 更健壮
    QStringList texts;
    int pos = 0;
    static const QString tOpen  = QStringLiteral("<w:t");
    static const QString tClose = QStringLiteral("</w:t>");

    while (true) {
        int start = xml.indexOf(tOpen, pos);
        if (start < 0) break;

        // 跳过 <w:t 属性...>
        int gt = xml.indexOf('>', start);
        if (gt < 0) break;
        int contentStart = gt + 1;

        int end = xml.indexOf(tClose, contentStart);
        if (end < 0) break;

        QString text = xml.mid(contentStart, end - contentStart);
        text = text.trimmed();
        if (!text.isEmpty() && text != QStringLiteral("教师评语"))
            texts << text;

        pos = end + tClose.size();
    }

    if (!tempDir.isEmpty()) removeDir(tempDir);
    *ok = true;
    return texts.join("\n");
}

// ═══════════════════════════════════════════════
//  提取成绩
// ═══════════════════════════════════════════════
double DocxHandler::extractScore(const QString &docxPath, bool *ok)
{
    bool dummy;
    if (!ok) ok = &dummy;

    QString tempDir;
    QString xml = readDocxXml(docxPath, ok, &tempDir);
    if (!*ok) return -1.0;

    // 提取所有段落文本，找到 "实验（践）成绩" 标签
    QStringList paragraphs;
    QXmlStreamReader reader(xml);

    while (!reader.atEnd() && !reader.hasError()) {
        if (reader.readNext() == QXmlStreamReader::StartElement
            && reader.name() == QStringLiteral("p")
            && reader.namespaceUri() == QLatin1String(kWordNS)) {

            QString text = reader.readElementText(QXmlStreamReader::IncludeChildElements);
            paragraphs << text.trimmed();
        }
    }

    if (!tempDir.isEmpty()) removeDir(tempDir);

    // 查找 "实验（践）成绩" 标签
    // 可能的形式：实验（践）成绩、实验成绩、实验(践)成绩
    static const QRegularExpression scoreLabelRe(
        QStringLiteral("实验[（(]?践?[）)]?\\s*成绩"));
    int scoreLabelIdx = -1;
    for (int i = 0; i < paragraphs.size(); ++i) {
        if (scoreLabelRe.match(paragraphs[i]).hasMatch()) {
            scoreLabelIdx = i;
            break;
        }
    }

    if (scoreLabelIdx < 0) {
        m_lastError = QString("未找到 \"实验（践）成绩\" 标签");
        *ok = false;
        return -1.0;
    }

    // 从该段落之后查找第一个包含数字的段落
    for (int i = scoreLabelIdx + 1; i < paragraphs.size(); ++i) {
        QString text = paragraphs[i].trimmed();
        if (text.isEmpty()) continue;
        // 用正则提取数字（含小数）
        QRegularExpression numRe(R"((\d+(?:\.\d+)?))");
        auto match = numRe.match(text);
        if (match.hasMatch()) {
            bool convOk = false;
            double score = match.captured(1).toDouble(&convOk);
            if (convOk && score >= 0 && score <= 150) {
                *ok = true;
                return score;
            }
        }
    }

    m_lastError = QString("未在文档中找到成绩分数");
    *ok = false;
    return -1.0;
}

// ═══════════════════════════════════════════════
//  写入评语
// ═══════════════════════════════════════════════
bool DocxHandler::writeComment(const QString &docxPath,
                                const QString &comment,
                                QString *processedPath)
{
    bool ok = false;
    QString tempDir = unzipToTemp(docxPath, &ok);
    if (!ok) return false;

    QString docPath = tempDir + "/word/document.xml";
    QFile file(docPath);
    if (!file.open(QIODevice::ReadOnly)) {
        m_lastError = QString("无法打开 %1 读取").arg(docPath);
        removeDir(tempDir);
        return false;
    }

    QString xml = QString::fromUtf8(file.readAll());
    file.close();

    // 定位 "教师评语"
    int labelIdx = xml.indexOf(QStringLiteral("教师评语"));
    if (labelIdx < 0) {
        m_lastError = QString("文档中未找到 \"教师评语\" 标签");
        removeDir(tempDir);
        return false;
    }

    // --- 1. 清理 <w:t> 中 "教师评语" 之后的旧文本 ---
    int tTagStart = xml.lastIndexOf(QStringLiteral("<w:t"), labelIdx);
    if (tTagStart < 0) {
        m_lastError = QString("文档结构异常：找不到 <w:t>");
        removeDir(tempDir);
        return false;
    }
    int tContentStart = xml.indexOf('>', tTagStart) + 1;
    int tEnd = xml.indexOf(QStringLiteral("</w:t>"), labelIdx);
    if (tEnd < 0) {
        m_lastError = QString("文档结构异常：找不到 </w:t>");
        removeDir(tempDir);
        return false;
    }
    {
        QString tc = xml.mid(tContentStart, tEnd - tContentStart);
        int idx = tc.indexOf(QStringLiteral("教师评语"));
        if (idx >= 0) {
            int keep = idx + QStringLiteral("教师评语").size();
            while (keep < tc.size()) {
                QChar c = tc[keep];
                if (c.unicode() == 0xFF1A || c == ':' || c == ' ' || c == '\t')
                    ++keep;
                else
                    break;
            }
            QString trimmed = tc.left(keep);
            xml.replace(tContentStart, tEnd - tContentStart, trimmed);
            int diff = trimmed.size() - (tEnd - tContentStart);
            tEnd += diff;
        }
    }

    // --- 2. 删除该段落内 "教师评语" run 之后的所有旧 run ---
    int runEnd = xml.indexOf(QStringLiteral("</w:r>"), labelIdx);
    if (runEnd < 0) {
        m_lastError = QString("文档结构异常：找不到 </w:r>");
        removeDir(tempDir);
        return false;
    }
    int paraEnd = xml.indexOf(QStringLiteral("</w:p>"), runEnd);
    int insertPos = runEnd + 6;
    if (paraEnd > insertPos)
        xml.remove(insertPos, paraEnd - insertPos);

    // --- 3. 插入新评语 run ---
    QString runXml = buildCommentRuns(comment);
    xml.insert(insertPos, runXml);

    // --- 4. 删除同一单元格内后续的占位段落 ---
    {
        int pp = xml.indexOf(QStringLiteral("</w:p>"), insertPos);
        if (pp >= 0) {
            pp += 6;
            int cellStart = -1;
            {
                int pos = labelIdx;
                while (pos >= 0) {
                    int tc = xml.lastIndexOf(QStringLiteral("<w:tc"), pos);
                    if (tc < 0) break;
                    int tcEnd = xml.indexOf(QStringLiteral("</w:tc>"), tc);
                    if (tcEnd > 0 && tcEnd > labelIdx) {
                        cellStart = tc;
                        break;
                    }
                    pos = tc - 1;
                }
            }
            if (cellStart >= 0) {
                int cellEnd = xml.indexOf(QStringLiteral("</w:tc>"), pp);
                if (cellEnd > pp)
                    xml.remove(pp, cellEnd - pp);
            }
        }
    }

    // 写回
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        m_lastError = QString("无法打开 %1 写入").arg(docPath);
        removeDir(tempDir);
        return false;
    }
    file.write(xml.toUtf8());
    file.close();

    if (!repackFromTemp(tempDir, docxPath)) {
        removeDir(tempDir);
        return false;
    }

    removeDir(tempDir);
    if (processedPath) *processedPath = docxPath;
    return true;
}

// ═══════════════════════════════════════════════
//  内部：构造评语的 XML run（追加在教师评语所在 run 后面）
// ═══════════════════════════════════════════════
QString DocxHandler::buildCommentRuns(const QString &comment)
{
    QStringList lines = comment.split('\n');
    if (lines.isEmpty()) lines << comment;

    QString result;
    QXmlStreamWriter w(&result);
    w.setAutoFormatting(false);
    w.writeNamespace(kWordNS, QStringLiteral("w"));

    auto writeAttrSz = [&](const QString &tag, const QString &val) {
        w.writeStartElement(kWordNS, tag);
        w.writeAttribute(QStringLiteral("w:val"), val);
        w.writeEndElement();
    };

    for (int i = 0; i < lines.size(); ++i) {
        w.writeStartElement(kWordNS, QStringLiteral("r"));
        w.writeStartElement(kWordNS, QStringLiteral("rPr"));
        writeAttrSz(QStringLiteral("sz"),   QStringLiteral("24"));
        writeAttrSz(QStringLiteral("szCs"), QStringLiteral("24"));
        w.writeEndElement(); // rPr
        if (i > 0) {
            w.writeStartElement(kWordNS, QStringLiteral("br"));
            w.writeEndElement();
        }
        w.writeStartElement(kWordNS, QStringLiteral("t"));
        w.writeAttribute(QStringLiteral("xml:space"), QStringLiteral("preserve"));
        w.writeCharacters(lines[i]);
        w.writeEndElement(); // t
        w.writeEndElement(); // r
    }
    return result;
}

// ═══════════════════════════════════════════════
//  辅助方法
// ═══════════════════════════════════════════════
QString DocxHandler::unzipToTemp(const QString &docxPath, bool *ok)
{
    m_lastError.clear();

    if (!QFileInfo::exists(docxPath)) {
        m_lastError = QString("文件不存在: %1").arg(docxPath);
        *ok = false;
        return {};
    }

    QString tmpDir = makeTempDirName(kDocxTempPrefix);
    QDir().mkpath(tmpDir);

    QProcess p;
    p.setWorkingDirectory(tmpDir);
    p.start(QStringLiteral("unzip"), {QStringLiteral("-qo"), docxPath});
    if (!p.waitForFinished(30000) || p.exitCode() != 0) {
        m_lastError = QString("解压失败: %1").arg(QString::fromUtf8(p.readAllStandardError().trimmed()));
        removeDir(tmpDir);
        *ok = false;
        return {};
    }

    *ok = true;
    return tmpDir;
}

bool DocxHandler::repackFromTemp(const QString &tmpDir, const QString &target)
{
    QFile::remove(target);

    QProcess p;
    p.setWorkingDirectory(tmpDir);
    p.start(QStringLiteral("zip"), {QStringLiteral("-rq"), target, QStringLiteral(".")});
    if (!p.waitForFinished(30000) || p.exitCode() != 0) {
        m_lastError = QString("打包失败: %1").arg(QString::fromUtf8(p.readAllStandardError().trimmed()));
        return false;
    }
    return true;
}

bool DocxHandler::removeDir(const QString &dirPath)
{
    QDir d(dirPath);
    if (!d.exists()) return true;

    for (const QFileInfo &fi : d.entryInfoList(QDir::NoDotAndDotDot | QDir::AllEntries)) {
        if (fi.isDir())
            removeDir(fi.absoluteFilePath());
        else
            QFile::remove(fi.absoluteFilePath());
    }
    return d.rmdir(dirPath);
}

QString DocxHandler::makeTempDirName(const QString &prefix)
{
    QString path;
    do {
        QString r = QString::number(QRandomGenerator::global()->bounded(100000, 999999));
        path = QDir::tempPath() + QDir::separator() + prefix + r;
    } while (QDir(path).exists());
    return path;
}