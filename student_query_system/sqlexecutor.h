#ifndef SQLEXECUTOR_H
#define SQLEXECUTOR_H

#include "crud.h"

#include <QString>
#include <QStringList>
#include <QVector>

class SqlExecutor
{
public:
    SqlExecutor();

    bool load(QString *errorMessage);
    SqlResult execute(const QString &sql, bool allowDeleteAll = false);
    SqlResult showTable(const QString &tableName);

private:
    SqlResult parse(const QString &sql, SqlCommand *command) const;
    SqlResult parseWhere(const QString &whereText, QVector<SqlCondition> *conditions) const;
    QStringList splitCommaAware(const QString &text) const;
    QString cleanValue(const QString &value) const;
    SqlResult error(const QString &message) const;

private:
    CsvDatabase m_database;
    CrudExecutor m_crudExecutor;
};

#endif
