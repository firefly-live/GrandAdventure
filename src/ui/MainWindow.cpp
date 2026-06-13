#include "MainWindow.h"

#include <QQuickWidget>
#include <QQmlContext>
#include <QQuickItem>//用于转化对象关系存储
#include"../scenes/PlayScene.h"
#include"../core/SceneManager.h"

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setFixedSize(1625,968);
    m_qmlWidget = new QQuickWidget(this);
    m_qmlWidget->setResizeMode(QQuickWidget::SizeRootObjectToView);



    auto scene = dynamic_cast<PlayScene*>(SceneManager::instance()->findTypeScene(SceneType::Play));//获取游戏对象场景
    m_qmlWidget->rootContext()->setContextProperty("playScene", scene);//然后将游戏场景注册到qml中
    m_qmlWidget->setSource(QUrl("../../qml/GameCanvas.qml"));

    m_qmlRoot = static_cast<QObject*>(m_qmlWidget->rootObject());
    setCentralWidget(m_qmlWidget);
}


void MainWindow::updatePlayerPosition(const QPointF& pos) {
    if (m_qmlRoot) {
        m_qmlRoot->setProperty("playerX", pos.x());
        m_qmlRoot->setProperty("playerY", pos.y());
        QCoreApplication::processEvents();
        //qDebug()<<"  "<<pos.x()<<"  "<<pos.y();
    }

}

MainWindow* MainWindow::instance() {
    static MainWindow mw;
    return &mw;
}

void MainWindow::keyPressEvent(QKeyEvent* event) {
    auto scene = dynamic_cast<PlayScene*>(SceneManager::instance()->currentScene());
    if (scene) scene->onKeyPress(Qt::Key(event->key()));
}

void MainWindow::keyReleaseEvent(QKeyEvent* event) {
    auto scene = dynamic_cast<PlayScene*>(SceneManager::instance()->currentScene());
    if (scene) scene->onKeyRelease(Qt::Key(event->key()));
}
