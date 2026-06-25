#ifndef CRUD_H
#define CRUD_H

#include "csv.h"

#include <QString>
#include <QStringList>
#include <QVector>

struct SqlResult
{
    SqlResult();

    bool success;
    bool needsConfirm;
    QString message;
    QStringList headers;
    QVector<QStringList> rows;
    int affectedRows;
};

struct SqlCondition
{
    QString field;
    QString op;
    QString value;
};

struct SqlAssignment
{
    QString field;
    QString value;
};

struct SqlCommand
{
    QString type;
    QString tableName;
    QStringList fields;
    QStringList values;
    QVector<SqlCondition> conditions;
    QVector<SqlAssignment> assignments;
};

class CrudExecutor
{
public:
    explicit CrudExecutor(CsvDatabase *database);

    SqlResult selectRows(const SqlCommand &command);
    SqlResult insertRow(const SqlCommand &command);
    SqlResult updateRows(const SqlCommand &command);
    SqlResult deleteRows(const SqlCommand &command, bool allowDeleteAll);
    SqlResult showTable(const QString &tableName);

private:
    bool rowMatches(const CsvTable &table, const QStringList &row, const QVector<SqlCondition> &conditions, QString *errorMessage) const;
    bool compareValues(const QString &left, const QString &op, const QString &right) const;
    SqlResult error(const QString &message) const;

private:
    CsvDatabase *m_database;
};

#endif
