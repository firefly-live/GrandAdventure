#pragma once

#include <QObject>
#include <QtQml/qqmlregistration.h> //注册用头文件
#include <QQmlEngine>
#include <QJSEngine>

class GameManager : public QObject
{
    Q_OBJECT
    QML_ELEMENT           // 关键：将这个类注册到 QML
    QML_SINGLETON         // 关键：注册为单例模式
    Q_PROPERTY(int totalScore READ totalScore NOTIFY totalScoreChanged)

public:

    static GameManager* create(QQmlEngine* qmlEngine, QJSEngine* jsEngine) {
        Q_UNUSED(qmlEngine)
        Q_UNUSED(jsEngine)
        return new GameManager();  // 创建唯一实例
    }

    explicit GameManager(QObject *parent = nullptr);
    int totalScore() const;

public slots:
    void addScore(int value);

signals:
    void totalScoreChanged();

private:
    int m_totalScore;
};