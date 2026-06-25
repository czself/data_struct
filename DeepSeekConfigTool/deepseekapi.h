#ifndef DEEPSEEKAPI_H
#define DEEPSEEKAPI_H

#include <QObject>
#include <QNetworkRequest>
#include <QString>

class QNetworkAccessManager;
class QNetworkReply;

class DeepSeekAPI : public QObject
{
    Q_OBJECT

public:
    explicit DeepSeekAPI(QObject *parent = nullptr);

    void setApiUrl(const QString &url);
    void setApiKey(const QString &key) { m_apiKey = key; }

    void generateComment(const QString &systemPrompt,
                         const QString &studentText);
    void sendPrompt(const QString &prompt);

signals:
    void commentReady(const QString &comment);
    void errorOccurred(const QString &error);

private slots:
    void onReplyFinished(QNetworkReply *reply);

private:
    QNetworkRequest createRequest() const;

    QNetworkAccessManager *m_manager;
    QString m_apiUrl;
    QString m_apiKey;
};

#endif // DEEPSEEKAPI_H
