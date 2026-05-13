#include "function_test.h"


Function_Test::Function_Test() {

}


QVector<QImage*> Function_Test::loadAnimFrames(const QString& basePath,int count){
    QVector<QImage*> frames;

    for(int i=0;i<count;i++){
        //读取目标目录的图片导入到容器中
        QString path =  QCoreApplication::applicationDirPath()+basePath + "_"+QString::number(i) + ".png";
        QImage *img = new QImage(path);
        if(!img->isNull())
        {
            frames.push_back(img);
            qDebug()<<path<<" read success\n";
        }else{
            qDebug()<<path<<" read error\n";
        }
    }
    return frames;
}
void Function_Test::startAnimation(){
    _label = new QLabel(testwindow);//锚定测试窗口

    _animTimer = new QTimer(testwindow);
    QObject::connect(_animTimer,&QTimer::timeout,[this](){
        //转化下图像
        if(!_frames.isEmpty())
            {
        QPixmap pix = QPixmap::fromImage(*_frames[_currentFrame]);
        _label->setFixedSize(80, 80);
        _label->setPixmap(pix.scaled(_label->size()));
        _currentFrame = (_currentFrame+1)%_frames.size();//让帧率索引为容器的大小
        qDebug() << "_frames size:" << _frames.size();
        }else{
            qDebug()<<"_frames is Null";
        }
    });
    _animTimer->start(100);




}


void Function_Test::testfunction(){



    _frames = loadAnimFrames("/../../role/paimeng/paimon_right",5);

    testwindow = new QMainWindow();

     startAnimation();
    testwindow->show();


}
