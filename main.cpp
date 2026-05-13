#include "mainwindow.h"
#include"function_test.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Function_Test FucTest;
    FucTest.testfunction();
    return a.exec();
}
