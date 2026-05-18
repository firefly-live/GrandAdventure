#include "function_test.h"


Function_Test::Function_Test():QWidget() {

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
    _label = new QLabel(this);//锚定测试窗口

    _animTimer = new QTimer(this);
    QObject::connect(_animTimer,&QTimer::timeout,[this](){
        //转化下图像
        if(!_frames.isEmpty())
            {
        QPixmap pix = QPixmap::fromImage(*_frames[_currentFrame]);
        _label->setFixedSize(80, 80);
        _label->setPixmap(pix.scaled(_label->size()));
        _currentFrame = (_currentFrame+1)%_frames.size();//让帧率索引为容器的大小

       // qDebug() << "_frames size:" << _frames.size();
        }else{
            qDebug()<<"_frames is Null";
        }
    });
    _animTimer->start(100);




}


void Function_Test::testfunction(){



    _frames = loadAnimFrames("/../../Resource/role/paimeng/paimon_right",5);

    this->setFixedSize(800, 600);

     startAnimation();
   this->show();

}


void Function_Test::keyPressEvent(QKeyEvent *event){
    qDebug()<<"按键输入触发\n";

    switch(event->key()) {
    case Qt::Key_W:
    case Qt::Key_Up:
        _y -= 10;
        break;
    case Qt::Key_S:
    case Qt::Key_Down:
        _y += 10;
        break;
    case Qt::Key_A:
    case Qt::Key_Left:
        _x -= 10;
        break;
    case Qt::Key_D:
    case Qt::Key_Right:
        _x += 10;
        break;
    }

    _label->move(_x,_y);


    //测试移动
}
