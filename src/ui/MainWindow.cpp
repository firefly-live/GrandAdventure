#include "MainWindow.h"

#include <QQuickWidget>
#include <QQmlContext>
#include <QQuickItem>//用于转化对象关系存储
MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setFixedSize(1280, 720);
    m_qmlWidget = new QQuickWidget(this);
    m_qmlWidget->setResizeMode(QQuickWidget::SizeRootObjectToView);
    m_qmlWidget->setSource(QUrl("../../qml/GameCanvas.qml"));
   m_qmlRoot = static_cast<QObject*>(m_qmlWidget->rootObject());
    setCentralWidget(m_qmlWidget);
}


void MainWindow::updatePlayerPosition(const QPointF& pos) {
    if (m_qmlRoot) {
        m_qmlRoot->setProperty("playerX", pos.x());
        m_qmlRoot->setProperty("playerY", pos.y());
    }
}

MainWindow* MainWindow::instance() {
    static MainWindow mw;
    return &mw;
}
