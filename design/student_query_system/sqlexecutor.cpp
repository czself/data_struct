#include "sqlexecutor.h"

#include <QRegExp>
#include <QRegularExpression>

static int findWhereKeyword(const QString &text)
{
    bool inSingleQuote = false;
    bool inDoubleQuote = false;

    for (int i = 0; i <= text.length() - 5; ++i) {
        const QChar ch = text.at(i);

        if (ch == '\'' && !inDoubleQuote) {
            inSingleQuote = !inSingleQuote;
        } else if (ch == '"' && !inSingleQuote) {
            inDoubleQuote = !inDoubleQuote;
        }

        if (inSingleQuote || inDoubleQuote) {
            continue;
        }

        if (text.mid(i, 5).compare("where", Qt::CaseInsensitive) == 0) {
            const bool leftOk = (i == 0 || text.at(i - 1).isSpace());
            const bool rightOk = (i + 5 == text.length() || text.at(i + 5).isSpace());
            if (leftOk && rightOk) {
                return i;
            }
        }
    }

    return -1;
}

SqlExecutor::SqlExecutor()
    : m_crudExecutor(&m_database)
{
}

bool SqlExecutor::load(QString *errorMessage)
{
    return m_database.loadAll(errorMessage);
}

SqlResult SqlExecutor::execute(const QString &sql, bool allowDeleteAll)
{
    SqlCommand command;
    SqlResult parseResult = parse(sql, &command);
    if (!parseResult.success) {
        return parseResult;
    }

    if (command.type == "select") {
        return m_crudExecutor.selectRows(command);
    }
    if (command.type == "insert") {
        return m_crudExecutor.insertRow(command);
    }
    if (command.type == "update") {
        return m_crudExecutor.updateRows(command);
    }
    if (command.type == "delete") {
        return m_crudExecutor.deleteRows(command, allowDeleteAll);
    }

    return error("暂不支持该 SQL 语句");
}

SqlResult SqlExecutor::showTable(const QString &tableName)
{
    return m_crudExecutor.showTable(tableName);
}

SqlResult SqlExecutor::parse(const QString &sql, SqlCommand *command) const
{
    QString text = sql.trimmed();
    if (text.isEmpty()) {
        return error("请输入 SQL 查询语句");
    }
    if (!text.endsWith(';')) {
        return error("SQL 语句需要以分号 ; 结尾");
    }

    text.chop(1);//chop() 删除字符串末尾的字符，这里是删除分号
    text = text.trimmed();

    QRegExp selectRx("^select\\s+(.+)\\s+from\\s+([A-Za-z_][A-Za-z0-9_]*)(?:\\s+where\\s+(.+))?$", Qt::CaseInsensitive);
    if (selectRx.exactMatch(text)) {
        command->type = "select";
        command->fields = splitCommaAware(selectRx.cap(1));
        command->tableName = selectRx.cap(2);

        if (command->fields.isEmpty()) {
            return error("select 字段不能为空");
        }
        for (int i = 0; i < command->fields.size(); ++i) {
            command->fields[i] = command->fields.at(i).trimmed();
        }

        if (!selectRx.cap(3).trimmed().isEmpty()) {
            return parseWhere(selectRx.cap(3), &command->conditions);
        }

        SqlResult result;
        result.success = true;
        return result;
    }

    QRegExp insertRx("^insert\\s+into\\s+([A-Za-z_][A-Za-z0-9_]*)\\s*\\((.+)\\)\\s*values\\s*\\((.+)\\)$", Qt::CaseInsensitive);
    if (insertRx.exactMatch(text)) {
        command->type = "insert";
        command->tableName = insertRx.cap(1);
        command->fields = splitCommaAware(insertRx.cap(2));
        command->values = splitCommaAware(insertRx.cap(3));

        if (command->fields.size() != command->values.size()) {
            return error("insert 字段数量和值数量不一致");
        }

        for (int i = 0; i < command->fields.size(); ++i) {
            command->fields[i] = command->fields.at(i).trimmed();
            command->values[i] = cleanValue(command->values.at(i));
        }

        SqlResult result;
        result.success = true;
        return result;
    }

    QRegExp updateRx("^update\\s+([A-Za-z_][A-Za-z0-9_]*)\\s+set\\s+(.+)$", Qt::CaseInsensitive);
    if (updateRx.exactMatch(text)) {
        command->type = "update";
        command->tableName = updateRx.cap(1);

        QString setText = updateRx.cap(2).trimmed();
        QString whereText;
        const int wherePos = findWhereKeyword(setText);
        if (wherePos >= 0) {
            whereText = setText.mid(wherePos + 5).trimmed();
            setText = setText.left(wherePos).trimmed();
        }

        QStringList assignments = splitCommaAware(setText);
        for (int i = 0; i < assignments.size(); ++i) {
            const QString item = assignments.at(i).trimmed();
            const int pos = item.indexOf('=');
            if (pos <= 0) {
                return error("update set 格式错误");
            }

            SqlAssignment assignment;
            assignment.field = item.left(pos).trimmed();
            assignment.value = cleanValue(item.mid(pos + 1));
            command->assignments.append(assignment);
        }

        if (!whereText.isEmpty()) {
            return parseWhere(whereText, &command->conditions);
        }

        SqlResult result;
        result.success = true;
        return result;
    }

    QRegExp deleteRx("^delete\\s+from\\s+([A-Za-z_][A-Za-z0-9_]*)(?:\\s+where\\s+(.+))?$", Qt::CaseInsensitive);
    if (deleteRx.exactMatch(text)) {
        command->type = "delete";
        command->tableName = deleteRx.cap(1);

        if (!deleteRx.cap(2).trimmed().isEmpty()) {
            return parseWhere(deleteRx.cap(2), &command->conditions);
        }

        SqlResult result;
        result.success = true;
        return result;
    }

    return error("SQL 格式错误，只支持 select、insert、update、delete");
}

SqlResult SqlExecutor::parseWhere(const QString &whereText, QVector<SqlCondition> *conditions) const
{
    QRegularExpression andRx("\\s+and\\s+", QRegularExpression::CaseInsensitiveOption);
    QStringList parts = whereText.split(andRx, Qt::SkipEmptyParts);
    if (parts.isEmpty()) {
        return error("where 条件不能为空");
    }

    QRegExp conditionRx("^\\s*([A-Za-z_][A-Za-z0-9_]*)\\s*(>=|<=|!=|=|>|<)\\s*(.+)\\s*$", Qt::CaseInsensitive);
    for (int i = 0; i < parts.size(); ++i) {
        if (!conditionRx.exactMatch(parts.at(i))) {
            return error("where 条件格式错误");
        }

        SqlCondition condition;
        condition.field = conditionRx.cap(1).trimmed();
        condition.op = conditionRx.cap(2).trimmed();
        condition.value = cleanValue(conditionRx.cap(3));
        conditions->append(condition);
    }

    SqlResult result;
    result.success = true;
    return result;
}

QStringList SqlExecutor::splitCommaAware(const QString &text) const
{
    QStringList result;
    QString current;
    bool inSingleQuote = false;
    bool inDoubleQuote = false;

    for (int i = 0; i < text.length(); ++i) {
        const QChar ch = text.at(i);
        if (ch == '\'' && !inDoubleQuote) {
            inSingleQuote = !inSingleQuote;
            current.append(ch);
        } else if (ch == '"' && !inSingleQuote) {
            inDoubleQuote = !inDoubleQuote;
            current.append(ch);
        } else if (ch == ',' && !inSingleQuote && !inDoubleQuote) {
            result.append(current.trimmed());
            current.clear();
        } else {
            current.append(ch);
        }
    }

    if (!current.trimmed().isEmpty()) {
        result.append(current.trimmed());
    }

    return result;
}

QString SqlExecutor::cleanValue(const QString &value) const
{
    QString result = value.trimmed();
    if (result.length() >= 2) {
        const QChar first = result.at(0);
        const QChar last = result.at(result.length() - 1);
        if ((first == '\'' && last == '\'') || (first == '"' && last == '"')) {
            result = result.mid(1, result.length() - 2);
        }
    }
    result.replace("\\'", "'");
    result.replace("\\\"", "\"");
    return result;
}

SqlResult SqlExecutor::error(const QString &message) const
{
    SqlResult result;
    result.success = false;
    result.message = message;
    return result;
}
