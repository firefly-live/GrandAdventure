#ifndef FUNCTION_TEST_H
#define FUNCTION_TEST_H

#include<Qvector>
#include<QImage>
#include<QString>
#include<QDebug>
#include<QMainWindow>
#include<QTimer>
#include<QLabel>
#include<QPixmap>
#include<QCoreApplication>
class Function_Test
{
public:
    Function_Test();

    //测试读取图片然后装入容器中,通过容器来遍历图像成动画
     QVector<QImage*> loadAnimFrames(const QString& basePath,int count);
    //测试播放设置,设置测试窗口
     void startAnimation();

    void testfunction();


    //测试属性
    QVector<QImage*> _frames;//存储动画
    int _currentFrame = 0;//记录当前帧率
    QLabel *_label;//存储label的image
    QTimer *_animTimer;//时钟差--动起来
    QMainWindow *testwindow;

};

#endif // FUNCTION_TEST_H
