#include "CharacterSelectScene.h"
#include <QDebug>

void CharacterSelectScene::update(int deltaMs) {
    static bool first = true;
    if (first) {
        qDebug() << "CharacterSelectScene update, delta =" << deltaMs;
        first = false;
    }
}

void CharacterSelectScene::draw() {
    // 暂时空实现
}
