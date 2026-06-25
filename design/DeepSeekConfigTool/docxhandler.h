#ifndef DOCXHANDLER_H
#define DOCXHANDLER_H

#include <QString>
#include <QStringList>

/**
 * DocxHandler - 纯 C++ 读写 .docx (OOXML) 文件
 *
 * .docx 本质是一个 ZIP 包，里面包含 XML 文档。
 * 本类使用系统 unzip/zip 命令解压和重新打包，
 * 用 QXmlStreamReader/QXmlStreamWriter 解析和修改 document.xml。
 */
class DocxHandler
{
public:
    DocxHandler();

    /**
     * 从 .docx 文件中提取所有文本内容
     * @param docxPath  .docx 文件路径
     * @param ok        输出参数，成功为 true
     * @return          提取出的纯文本（段落之间用换行分隔）
     */
    QString extractText(const QString &docxPath, bool *ok = nullptr);

    /**
     * 从 .docx 文件中提取成绩分数
     * 查找 "实验（践）成绩" 标签后第一个数字行
     * @param docxPath  .docx 文件路径
     * @param ok        输出参数，成功为 true
     * @return          提取出的分数（如 90.0），失败返回 -1.0
     */
    double extractScore(const QString &docxPath, bool *ok = nullptr);

    /**
     * 将评语写入 .docx 文件中的 "教师评语" 位置
     * @param docxPath          .docx 文件路径
     * @param comment           AI 生成的评语
     * @param processedPath     输出参数：处理后的文件路径（覆盖原文件）
     * @return                  成功为 true
     */
    bool writeComment(const QString &docxPath,
                      const QString &comment,
                      QString *processedPath = nullptr);

    /** 获取最后一次错误的描述 */
    QString lastError() const { return m_lastError; }

private:
    /** 内部：解压并读取 document.xml 原始 XML 字符串 */
    QString readDocxXml(const QString &docxPath, bool *ok, QString *tempDirOut);

    /** 构建评语的 XML run 字符串（插入在 "教师评语" 所在 run 之后） */
    QString buildCommentRuns(const QString &comment);

    /**
     * 解压 .docx 到临时目录，返回临时目录路径
     */
    QString unzipToTemp(const QString &docxPath, bool *ok);

    /**
     * 将临时目录重新打包为 .docx，覆盖原文件
     */
    bool repackFromTemp(const QString &tempDir, const QString &targetDocx);

    /**
     * 递归删除临时目录
     */
    bool removeDir(const QString &dirPath);

    /** 生成唯一的临时目录名 */
    QString makeTempDirName(const QString &prefix = "docx_");

    QString m_lastError;
};

#endif // DOCXHANDLER_H