#ifndef CSV_H
#define CSV_H

#include <QMap>
#include <QString>
#include <QStringList>
#include <QVector>

class CsvTable
{
public:
    CsvTable();
    CsvTable(const QString &key, const QString &path);

    bool load(QString *errorMessage);
    bool save(QString *errorMessage);

    QString key() const;
    QStringList headers() const;
    QVector<QStringList> rows() const;
    QVector<QStringList> &rows();

    int columnIndex(const QString &fieldName) const;
    QString actualFieldName(const QString &fieldName) const;

private:
    bool ensureBackup(QString *errorMessage);

private:
    QString m_key;
    QString m_path;
    QStringList m_headers;
    QVector<QStringList> m_rows;
    bool m_backupCreated;
};

class CsvDatabase
{
public:
    CsvDatabase();

    bool loadAll(QString *errorMessage);
    CsvTable *table(const QString &tableName);
    QString tableKey(const QString &tableName) const;

private:
    QMap<QString, CsvTable> m_tables;
};

#endif
