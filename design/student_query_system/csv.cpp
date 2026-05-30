#include "csv.h"

#include <QFile>
#include <QFileInfo>
#include <QTextStream>

static QString normalizeName(const QString &name)
{
    QString result = name.trimmed().toUpper();//trim() 移除首尾空格，toUpper() 转
    //换为大写，remove('_') 移除下划线，这样就能实现不区分大小写和下划线的字段名匹配了   
    result.remove('_');
    return result;
}

static QStringList parseCsvLine(const QString &line)
{
    return line.split(',');
}

CsvTable::CsvTable()
    : m_backupCreated(false)
{
}

CsvTable::CsvTable(const QString &key, const QString &path)
    : m_key(key),
      m_path(path),
      m_backupCreated(false)
{
}

bool CsvTable::load(QString *errorMessage)
{
    QFile file(m_path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        if (errorMessage) {
            *errorMessage = "无法打开数据文件：" + m_path;
        }
        return false;
    }

    QTextStream in(&file);//变成文件流了，然后就不要一个一个递加了，直接读取一行了
    in.setCodec("UTF-8");

    m_headers.clear();
    m_rows.clear();

    if (!in.atEnd()) {
        QString headerLine = in.readLine();
        headerLine.remove('\r');
        m_headers = parseCsvLine(headerLine);
    }

    while (!in.atEnd()) {
        QString line = in.readLine();
        line.remove('\r');
        if (line.trimmed().isEmpty()) {
            continue;
        }

        QStringList row = parseCsvLine(line);
        while (row.size() < m_headers.size()) {
            row.append("");
        }
        while (row.size() > m_headers.size()) {
            row.removeLast();
        }
        m_rows.append(row);
    }

    return true;
}

bool CsvTable::save(QString *errorMessage)
{
    if (!ensureBackup(errorMessage)) {
        return false;
    }

    QFile file(m_path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
        if (errorMessage) {
            *errorMessage = "无法写入数据文件：" + m_path;
        }
        return false;
    }

    QTextStream out(&file);
    out.setCodec("UTF-8");

    QStringList escapedHeaders;
    for (int i = 0; i < m_headers.size(); ++i) {
        escapedHeaders.append(m_headers.at(i));
    }
    out << escapedHeaders.join(",") << "\n";

    for (int rowIndex = 0; rowIndex < m_rows.size(); ++rowIndex) {
        QStringList escapedRow;
        const QStringList row = m_rows.at(rowIndex);
        for (int column = 0; column < m_headers.size(); ++column) {
            escapedRow.append(column < row.size() ? row.at(column) : "");
        }
        out << escapedRow.join(",") << "\n";
    }

    return true;
}

QString CsvTable::key() const
{
    return m_key;
}

QString CsvTable::path() const
{
    return m_path;
}

QStringList CsvTable::headers() const
{
    return m_headers;
}

QVector<QStringList> CsvTable::rows() const
{
    return m_rows;
}

QVector<QStringList> &CsvTable::rows()
{
    return m_rows;
}

int CsvTable::columnIndex(const QString &fieldName) const
{
    const QString normalized = normalizeName(actualFieldName(fieldName));
    for (int i = 0; i < m_headers.size(); ++i) {
        if (normalizeName(m_headers.at(i)) == normalized) {
            return i;
        }
    }
    return -1;
}

QString CsvTable::actualFieldName(const QString &fieldName) const
{
    const QString normalized = normalizeName(fieldName);

    if (m_key == "student" && normalized == "CLASSID") {
        return "CLAZZ_ID";
    }
    if (m_key == "score" && normalized == "SCORE") {
        return "RESULT";
    }

    for (int i = 0; i < m_headers.size(); ++i) {
        if (normalizeName(m_headers.at(i)) == normalized) {
            return m_headers.at(i);
        }
    }

    return fieldName.trimmed().toUpper();
}

bool CsvTable::ensureBackup(QString *errorMessage)
{
    if (m_backupCreated) {
        return true;
    }

    const QString backupPath = m_path + ".bak";
    if (!QFileInfo::exists(backupPath)) {
        if (!QFile::copy(m_path, backupPath)) {
            if (errorMessage) {
                *errorMessage = "创建备份失败：" + backupPath;
            }
            return false;
        }
    }

    m_backupCreated = true;
    return true;
}

CsvDatabase::CsvDatabase()
{
    m_tables.insert("student", CsvTable("student", "../资料/tab_student.csv"));
    m_tables.insert("course", CsvTable("course", "../资料/tab_course.csv"));
    m_tables.insert("class", CsvTable("class", "../资料/tab_class.csv"));
    m_tables.insert("score", CsvTable("score", "../资料/tab_score.csv"));
}

bool CsvDatabase::loadAll(QString *errorMessage)
{
    QMap<QString, CsvTable>::iterator it;
    for (it = m_tables.begin(); it != m_tables.end(); ++it) {
        if (!it.value().load(errorMessage)) {
            return false;
        }
    }
    return true;
}

CsvTable *CsvDatabase::table(const QString &tableName)
{
    const QString key = tableKey(tableName);
    if (key.isEmpty() || !m_tables.contains(key)) {
        return 0;
    }
    return &m_tables[key];
}

const CsvTable *CsvDatabase::table(const QString &tableName) const
{
    const QString key = tableKey(tableName);
    if (key.isEmpty() || !m_tables.contains(key)) {
        return 0;
    }

    QMap<QString, CsvTable>::const_iterator it = m_tables.constFind(key);
    if (it == m_tables.constEnd()) {
        return 0;
    }
    return &it.value();
}

QString CsvDatabase::tableKey(const QString &tableName) const
{
    const QString normalized = tableName.trimmed().toLower();

    if (normalized == "student" || normalized == "studenttable") {
        return "student";
    }
    if (normalized == "course" || normalized == "coursetable") {
        return "course";
    }
    if (normalized == "class" || normalized == "classtable") {
        return "class";
    }
    if (normalized == "score" || normalized == "scoretable") {
        return "score";
    }

    return "";
}
