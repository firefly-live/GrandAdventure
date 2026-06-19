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


    //注册对象,之后进行设置窗口操作
   QString qmlFile = "../../qml/GameCanvas.qml";  // 建议改为 qrc:/qml/GameCanvas.qml
   Scene* contextObject = SceneManager::instance()->findTypeScene(SceneType::Play);
    if (contextObject) {
        m_qmlWidget->rootContext()->setContextProperty("playScene", contextObject);
    }

    contextObject = SceneManager::instance()->findTypeScene(SceneType::Death);

    if (contextObject) {
        m_qmlWidget->rootContext()->setContextProperty("deathScene", contextObject);
    }




    m_qmlRoot = static_cast<QObject*>(m_qmlWidget->rootObject());
    //setCentralWidget(m_qmlWidget);
    // m_qmlWidget->setVisible(true);
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


void MainWindow::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        auto current = SceneManager::instance()->currentScene();
        if (PlayScene* scene = dynamic_cast<PlayScene*>(current)) {
            // 将鼠标坐标转换为游戏世界坐标（需根据实际映射）
            scene->shootBullet(event->pos()); // 具体转换省略
        }
    }
}

int MainWindow::getAnimDir() const {
    auto scene = dynamic_cast<PlayScene*>(SceneManager::instance()->currentScene());
    return scene ? scene->getAnimDir() : 0;
}

bool MainWindow::isMoving() const {
    auto scene = dynamic_cast<PlayScene*>(SceneManager::instance()->currentScene());
    return scene ? scene->isMoving() : false;
}


void MainWindow::loadSceneQML(SceneType type) {

    QString qmlFile;
    Scene* contextObject = nullptr;

    switch (type) {
    case SceneType::Play:
        qmlFile = "../../qml/GameCanvas.qml";  // 建议改为 qrc:/qml/GameCanvas.qml

        break;
    case SceneType::Death:
        qmlFile = "../../qml/DeathScene.qml";

        break;
    default:
        return;
    }

    // ========== 4. 加载 QML 并强制刷新 ==========
    if (!qmlFile.isEmpty()) {
        m_qmlWidget->setSource(QUrl(qmlFile));
        m_qmlWidget->repaint();
        m_qmlRoot = m_qmlWidget->rootObject();
        qDebug() << "QML loaded, root:" << m_qmlRoot;
    }
}
