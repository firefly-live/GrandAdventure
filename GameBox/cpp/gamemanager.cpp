#include "gamemanager.h"

GameManager::GameManager(QObject *parent) : QObject(parent), m_totalScore(0) {}

int GameManager::totalScore() const { return m_totalScore; }

void GameManager::addScore(int value) {
    if (value != 0) {
        m_totalScore += value;
        emit totalScoreChanged();
    }
}