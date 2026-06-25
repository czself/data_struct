#include "crud.h"

SqlResult::SqlResult()
    : success(false),
      needsConfirm(false),
      affectedRows(0)
{
}

CrudExecutor::CrudExecutor(CsvDatabase *database)
    : m_database(database)
{
}

SqlResult CrudExecutor::showTable(const QString &tableName)
{
    SqlCommand command;
    command.type = "select";
    command.tableName = tableName;
    command.fields << "*";
    return selectRows(command);
}

SqlResult CrudExecutor::selectRows(const SqlCommand &command)
{
    CsvTable *table = m_database->table(command.tableName);
    if (!table) {
        return error("表名错误：" + command.tableName);
    }

    QVector<int> columnIndexes;
    QStringList resultHeaders;

    if (command.fields.size() == 1 && command.fields.at(0).trimmed() == "*") {
        for (int i = 0; i < table->headers().size(); ++i) {
            columnIndexes.append(i);
        }
        resultHeaders = table->headers();
    } else {
        for (int i = 0; i < command.fields.size(); ++i) {
            const int column = table->columnIndex(command.fields.at(i));
            if (column < 0) {
                return error("字段错误：" + command.fields.at(i));
            }
            columnIndexes.append(column);
            resultHeaders.append(table->actualFieldName(command.fields.at(i)));
        }
    }

    SqlResult result;
    result.success = true;
    result.headers = resultHeaders;

    for (int rowIndex = 0; rowIndex < table->rows().size(); ++rowIndex) {
        const QStringList row = table->rows().at(rowIndex);
        QString matchError;
        if (!rowMatches(*table, row, command.conditions, &matchError)) {
            if (!matchError.isEmpty()) {
                return error(matchError);
            }
            continue;
        }

        QStringList resultRow;
        for (int i = 0; i < columnIndexes.size(); ++i) {
            const int column = columnIndexes.at(i);
            resultRow.append(column < row.size() ? row.at(column) : "");
        }
        result.rows.append(resultRow);
    }

    result.affectedRows = result.rows.size();
    result.message = "查询成功，共 " + QString::number(result.rows.size()) + " 行";
    return result;
}

SqlResult CrudExecutor::insertRow(const SqlCommand &command)
{
    CsvTable *table = m_database->table(command.tableName);
    if (!table) {
        return error("表名错误：" + command.tableName);
    }

    QStringList newRow;
    for (int i = 0; i < table->headers().size(); ++i) {
        newRow.append("");
    }

    for (int i = 0; i < command.fields.size(); ++i) {
        const int column = table->columnIndex(command.fields.at(i));
        if (column < 0) {
            return error("字段错误：" + command.fields.at(i));
        }
        newRow[column] = command.values.at(i);
    }

    QStringList primaryColumns;
    if (table->key() == "score") {
        primaryColumns << "STU_ID" << "COURSE_ID";
    } else {
        primaryColumns << "ID";
    }

    for (int i = 0; i < primaryColumns.size(); ++i) {
        const int column = table->columnIndex(primaryColumns.at(i));
        if (column < 0 || newRow.at(column).trimmed().isEmpty()) {
            return error("insert 缺少主键字段：" + primaryColumns.at(i));
        }
    }

    for (int rowIndex = 0; rowIndex < table->rows().size(); ++rowIndex) {
        const QStringList row = table->rows().at(rowIndex);
        bool samePrimaryKey = true;
        for (int i = 0; i < primaryColumns.size(); ++i) {
            const int column = table->columnIndex(primaryColumns.at(i));
            if (row.value(column) != newRow.value(column)) {
                samePrimaryKey = false;
                break;
            }
        }
        if (samePrimaryKey) {
            return error("主键重复，无法插入");
        }
    }

    table->rows().append(newRow);

    QString saveError;
    if (!table->save(&saveError)) {
        table->rows().removeLast();
        return error(saveError);
    }

    SqlResult result;
    result.success = true;
    result.affectedRows = 1;
    result.message = "插入成功，影响 1 行";
    result.headers = table->headers();
    result.rows.append(newRow);
    return result;
}

SqlResult CrudExecutor::updateRows(const SqlCommand &command)
{
    CsvTable *table = m_database->table(command.tableName);
    if (!table) {
        return error("表名错误：" + command.tableName);
    }

    QVector<int> assignmentColumns;
    for (int i = 0; i < command.assignments.size(); ++i) {
        const int column = table->columnIndex(command.assignments.at(i).field);
        if (column < 0) {
            return error("字段错误：" + command.assignments.at(i).field);
        }
        assignmentColumns.append(column);
    }

    QVector<QStringList> oldRows = table->rows();
    int affected = 0;
    for (int rowIndex = 0; rowIndex < table->rows().size(); ++rowIndex) {
        QStringList &row = table->rows()[rowIndex];
        QString matchError;
        if (!rowMatches(*table, row, command.conditions, &matchError)) {
            if (!matchError.isEmpty()) {
                return error(matchError);
            }
            continue;
        }

        for (int i = 0; i < command.assignments.size(); ++i) {
            row[assignmentColumns.at(i)] = command.assignments.at(i).value;
        }
        ++affected;
    }

    if (affected == 0) {
        SqlResult result;
        result.success = true;
        result.affectedRows = 0;
        result.message = "修改成功，影响 0 行";
        result.headers = table->headers();
        result.rows = table->rows();
        return result;
    }

    QString saveError;
    if (!table->save(&saveError)) {
        table->rows() = oldRows;
        return error(saveError);
    }

    SqlResult result;
    result.success = true;
    result.affectedRows = affected;
    result.message = "修改成功，影响 " + QString::number(affected) + " 行";
    result.headers = table->headers();
    result.rows = table->rows();
    return result;
}

SqlResult CrudExecutor::deleteRows(const SqlCommand &command, bool allowDeleteAll)
{
    CsvTable *table = m_database->table(command.tableName);
    if (!table) {
        return error("表名错误：" + command.tableName);
    }

    if (command.conditions.isEmpty() && !allowDeleteAll) {
        SqlResult result;
        result.needsConfirm = true;
        result.message = "delete 未带 where 条件，将删除整张表";
        return result;
    }

    QVector<QStringList> oldRows = table->rows();
    QVector<QStringList> newRows;
    int affected = 0;

    for (int rowIndex = 0; rowIndex < table->rows().size(); ++rowIndex) {
        const QStringList row = table->rows().at(rowIndex);
        QString matchError;
        const bool matched = rowMatches(*table, row, command.conditions, &matchError);
        if (!matchError.isEmpty()) {
            return error(matchError);
        }

        if (matched) {
            ++affected;
        } else {
            newRows.append(row);
        }
    }

    if (affected == 0) {
        SqlResult result;
        result.success = true;
        result.affectedRows = 0;
        result.message = "删除成功，影响 0 行";
        result.headers = table->headers();
        result.rows = table->rows();
        return result;
    }

    table->rows() = newRows;

    QString saveError;
    if (!table->save(&saveError)) {
        table->rows() = oldRows;
        return error(saveError);
    }

    SqlResult result;
    result.success = true;
    result.affectedRows = affected;
    result.message = "删除成功，影响 " + QString::number(affected) + " 行";
    result.headers = table->headers();
    result.rows = table->rows();
    return result;
}

bool CrudExecutor::rowMatches(const CsvTable &table, const QStringList &row, const QVector<SqlCondition> &conditions, QString *errorMessage) const
{
    for (int i = 0; i < conditions.size(); ++i) {
        const SqlCondition condition = conditions.at(i);
        const int column = table.columnIndex(condition.field);
        if (column < 0) {
            if (errorMessage) {
                *errorMessage = "字段错误：" + condition.field;
            }
            return false;
        }

        const QString left = column < row.size() ? row.at(column) : "";
        if (!compareValues(left, condition.op, condition.value)) {
            return false;
        }
    }

    return true;
}

bool CrudExecutor::compareValues(const QString &left, const QString &op, const QString &right) const
{
    bool leftOk = false;
    bool rightOk = false;
    const double leftNumber = left.toDouble(&leftOk);
    const double rightNumber = right.toDouble(&rightOk);

    if (leftOk && rightOk) {
        if (op == "=") return leftNumber == rightNumber;
        if (op == "!=") return leftNumber != rightNumber;
        if (op == ">") return leftNumber > rightNumber;
        if (op == ">=") return leftNumber >= rightNumber;
        if (op == "<") return leftNumber < rightNumber;
        if (op == "<=") return leftNumber <= rightNumber;
    }

    const int cmp = QString::compare(left, right, Qt::CaseInsensitive);
    if (op == "=") return cmp == 0;
    if (op == "!=") return cmp != 0;
    if (op == ">") return cmp > 0;
    if (op == ">=") return cmp >= 0;
    if (op == "<") return cmp < 0;
    if (op == "<=") return cmp <= 0;

    return false;
}

SqlResult CrudExecutor::error(const QString &message) const
{
    SqlResult result;
    result.success = false;
    result.message = message;
    return result;
}
