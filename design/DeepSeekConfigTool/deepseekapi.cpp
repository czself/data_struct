#include "deepseekapi.h"

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QUrl>

namespace {
const char kModelName[] = "deepseek-chat";
const char kChatPath[] = "/chat/completions";
}

DeepSeekAPI::DeepSeekAPI(QObject *parent)
    : QObject(parent)
    , m_manager(new QNetworkAccessManager(this))
{
    connect(m_manager, &QNetworkAccessManager::finished,
            this, &DeepSeekAPI::onReplyFinished);
}

void DeepSeekAPI::setApiUrl(const QString &url)
{
    m_apiUrl = url.trimmed();
    while (m_apiUrl.endsWith('/'))
        m_apiUrl.chop(1);
}

void DeepSeekAPI::generateComment(const QString &systemPrompt,
                                   const QString &studentText)
{
    if (m_apiUrl.isEmpty() || m_apiKey.isEmpty()) {
        emit errorOccurred("API 地址或 Key 未设置");
        return;
    }

    QJsonObject body;
    body["model"] = kModelName;

    QJsonArray messages;
    {
        QJsonObject msg;
        msg["role"] = "system";
        msg["content"] = systemPrompt;
        messages.append(msg);
    }
    {
        QString userMsg = QString(
            "学生实验报告内容：\n%1")
            .arg(studentText.left(2000));
        QJsonObject msg;
        msg["role"] = "user";
        msg["content"] = userMsg;
        messages.append(msg);
    }
    body["messages"] = messages;
    body["stream"] = false;
    body["max_tokens"] = 512;
    body["temperature"] = 0.8;

    m_manager->post(createRequest(),
                    QJsonDocument(body).toJson(QJsonDocument::Compact));
}

void DeepSeekAPI::sendPrompt(const QString &prompt)
{
    if (m_apiUrl.isEmpty() || m_apiKey.isEmpty()) {
        emit errorOccurred("API 地址或 Key 未设置");
        return;
    }

    QJsonObject body;
    body["model"] = kModelName;

    QJsonArray messages;
    QJsonObject userMsg;
    userMsg["role"] = "user";
    userMsg["content"] = prompt;
    messages.append(userMsg);
    body["messages"] = messages;
    body["stream"] = false;
    body["max_tokens"] = 1024;

    m_manager->post(createRequest(),
                    QJsonDocument(body).toJson(QJsonDocument::Compact));
}

QNetworkRequest DeepSeekAPI::createRequest() const
{
    QNetworkRequest req(QUrl(m_apiUrl + kChatPath));
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    req.setRawHeader("Authorization", ("Bearer " + m_apiKey).toUtf8());
    return req;
}

void DeepSeekAPI::onReplyFinished(QNetworkReply *reply)
{
    reply->deleteLater();

    if (reply->error() != QNetworkReply::NoError) {
        QString detail = QString::fromUtf8(reply->readAll());
        emit errorOccurred("网络错误: " + reply->errorString()
                           + "\n" + detail.left(300));
        return;
    }

    QByteArray data = reply->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull() || !doc.isObject()) {
        emit errorOccurred("API 返回了非法的 JSON");
        return;
    }

    QJsonObject obj = doc.object();
    if (obj.contains("error")) {
        QString err = obj["error"].toObject()["message"].toString();
        emit errorOccurred("API 错误: " + err);
        return;
    }

    QJsonArray choices = obj["choices"].toArray();
    if (choices.isEmpty()) {
        emit errorOccurred("API 返回空 choices");
        return;
    }

    QString content = choices[0].toObject()["message"].toObject()["content"].toString();
    if (content.isEmpty())
        content = "(AI 未生成评语)";

    emit commentReady(content.trimmed());
}
