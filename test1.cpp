#include "test1.h"
#include <QDebug>
#include <QPainter>
#include <QMessageBox>
#include <QMenu>
#include <QMenuBar>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QApplication>
#include <iostream>
#include "getminstep.h"
Test1::Test1(QWidget *parent) : QWidget(parent),block(":/image/block"),
    man(":/image/man"),target(":/image/target"),box(":/image/box"),
    manAtTarget(createImageWithOverlay(target,man)),boxAtTarget(createImageWithOverlay(target,box))
{
    level = 1;
    step = 0;
    maxStep = 0;
    loadFromFile = new QAction("加载自定义地图",this);
    about = new QAction("帮助",this);
    QAction *aboutQt = new QAction("关于Qt",this);
    QMenuBar *menuBar = new QMenuBar(this);
    QMenu *menu1 = new QMenu("文件",menuBar);
    QMenu *menu2 = new QMenu("关于",menuBar);
    menu1->addAction(loadFromFile);
    menu2->addAction(about);
    menu2->addAction(aboutQt);
    menuBar->addMenu(menu1);
    menuBar->addMenu(menu2);
    QObject::connect(loadFromFile,QAction::triggered,this,startFromFile);
    QObject::connect(about,QAction::triggered,this,help);
    QObject::connect(aboutQt,QAction::triggered,QApplication::aboutQt);
    block = block.scaled(50,50);
    man = man.scaled(50,50);
    target = target.scaled(50,50);
    box = box.scaled(50,50);
    manAtTarget = manAtTarget.scaled(50,50);
    boxAtTarget = boxAtTarget.scaled(50,50);
    for(int i=0;i<MAXN;i++)
    {
        for(int j=0;j<MAXN;j++)
        {
            map[i][j] = NULL;
        }
    }
    step_label = new QLabel(this);
    max_step_label = new QLabel(this);
    initMap(level);
    gridLayout = new QGridLayout(this);
    gridLayout->setContentsMargins(30,30,30,30);
    reset_button = new QPushButton(this);
    level_label = new QLabel(this);
    reset_button->setText("RESTART");
    level_label->setText("LEVEL 1");
    QObject::connect(reset_button,QPushButton::clicked,this,restart);
    for(int i=0;i<MAXN;i++)
    {
        for(int j=0;j<MAXN;j++)
        {
            gridLayout->addWidget(map[i][j],i,j,Qt::AlignCenter);
        }
    }\
    QPushButton *getTip = new QPushButton(this);
    getTip->setText("获得提示");
    QObject::connect(getTip,QPushButton::clicked,this,showTip);
    gridLayout->addWidget(getTip,8,9,1,3,Qt::AlignCenter);
    gridLayout->addWidget(level_label,1,9,1,3,Qt::AlignCenter);
    gridLayout->addWidget(max_step_label,3,9,1,3,Qt::AlignCenter);
    gridLayout->addWidget(step_label,5,9,1,3,Qt::AlignCenter);
    gridLayout->addWidget(reset_button,7,9,1,3,Qt::AlignCenter);
    gridLayout->setSpacing(2);
    setLayout(gridLayout);
}
void Test1::initMap(int l)
{
    //qDebug()<<"level = "<<l;
    //qDebug()<<"size of custom Map : "<<customMap.size();
    step = 0;
    char tmp[20];
    sprintf(tmp,"%d",step);
    step_label->setText(QString("目前推箱子的次数：")+tmp);
    //qDebug()<<step_label->text();

    QString temp[MAXLEVEL];
    temp[1] =       "#########"
                    "###M.####"
                    "###.B..##"
                    "####.#.##"
                    "##T#.#..#"
                    "##TB..#.#"
                    "##T...B.#"
                    "#########"
                    "#########";
    temp[0] =   "#########"
                "#########"
                "####....#"
                "####.##.#"
                "##TMB...#"
                "#########"
                "#########"
                "#########"
                "#########";
    temp[2] =   "#########"
                "#########"
                "###..####"
                "##...####"
                "##..HB.##"
                "##.#T..##"
                "##..M####"
                "#########"
                "#########";
    temp[3] =   "#########"
                "###..####"
                "##MB.####"
                "###B.####"
                "###.B.###"
                "##TB..###"
                "##TTHT###"
                "#########"
                "#########";

    if(l==0)
    {
        maxStep = getMinStep(customMap.toStdString());
    }
    else
    {
        maxStep = getMinStep(temp[l-1].toStdString());
    }
    sprintf(tmp,"%d",maxStep);
    //qDebug()<<"hah2";
    if(maxStep!=-1)
        max_step_label->setText(QString("推箱子的最小次数:")+QString(tmp));
    else
        max_step_label->setText("此地图无解");
    for(int i=0;i<MAXN;i++)
    {
        for(int j=0;j<MAXN;j++)
        {
            if(!map[i][j])
                map[i][j] = new QLabel(this);
            map[i][j]->setMinimumSize(50,50);
            map[i][j]->clear();
            char t;
            //qDebug()<<"!!! level = "<<level;
            if(l!=0)
                t = temp[l-1][i*MAXN+j].toLatin1();
            else
                t = customMap.at(i*MAXN+j).toLatin1();
            //qDebug()<<t;
            switch(t)
            {
            case 'H':
                map[i][j]->setPixmap(QPixmap::fromImage(boxAtTarget));
                flags[i][j] = BOX_AT_TARGET;
                break;
            case '#':
                map[i][j]->setPixmap(QPixmap::fromImage(block));
                flags[i][j] = WALL;
                break;
            case '.':
                flags[i][j] = EMPTY;
                break;
            case 'M':
                flags[i][j] = MAN;
                map[i][j]->setPixmap(QPixmap::fromImage(man));
                focusX = i;
                focusY = j;
                break;
            case 'B':
                flags[i][j] = BOX;
                map[i][j]->setPixmap(QPixmap::fromImage(box));
                break;
            case 'T':
                flags[i][j] = EMPTY_TARGET;
                map[i][j]->setPixmap(QPixmap::fromImage(target));
            }
            map[i][j]->setScaledContents(true);
        }
    }

}

void Test1::keyReleaseEvent(QKeyEvent *event)
{
    //QWidget::keyReleaseEvent(event);
    //return;
    int key = event->key();
    int nx = focusX,ny = focusY;
    int boxx,boxy;
    switch(key)
    {
    case Qt::Key_Up:
        nx--;
        break;
    case Qt::Key_Down:
        nx++;
        break;
    case Qt::Key_Left:
        ny--;
        break;
    case Qt::Key_Right:
        ny++;
        break;
    default:
        return;
    }
    //qDebug()<<"nx= "<<nx<<" ny="<<ny<<" value:"<<flags[nx][ny];
    if(nx>=0&&nx<MAXN&&ny>=0&&ny<MAXN&&flags[nx][ny]!=WALL)
    {
        //qDebug()<<"checkPoint";
        if(flags[nx][ny]==BOX||flags[nx][ny]==BOX_AT_TARGET)
        {
            boxx = nx;
            boxy = ny;
            boxx += nx-focusX;
            boxy += ny-focusY;
            //qDebug()<<"boxx="<<boxx<<" boxy="<<boxy;
            if(boxx>=0&&boxx<MAXN&&boxy>=0&&boxy<MAXN&&(flags[boxx][boxy]!=WALL&&flags[boxx][boxy]!=BOX&&flags[boxx][boxy]!=BOX_AT_TARGET))
            {
                //qDebug()<<"route 1";
                if(flags[boxx][boxy]==EMPTY)
                {
                    //qDebug()<<"route 2";
                    if(flags[focusX][focusY]==MAN)
                    {
                        //qDebug()<<"route 3";
                        if(flags[nx][ny]==BOX)
                        {
                            //qDebug()<<"route 4";
                            flags[focusX][focusY] = EMPTY;
                            flags[nx][ny] = MAN;
                            flags[boxx][boxy] = BOX;
                            map[focusX][focusY]->clear();
                            map[nx][ny]->clear();
                            map[boxx][boxy]->clear();
                            map[nx][ny]->setPixmap(QPixmap::fromImage(man));
                            map[boxx][boxy]->setPixmap(QPixmap::fromImage(box));
                            focusX = nx;
                            focusY = ny;
                            step++;
                            qDebug()<<"branch 1";
                        }
                        else if(flags[nx][ny]==BOX_AT_TARGET)
                        {
                            flags[focusX][focusY] = EMPTY;
                            flags[nx][ny] = MAN_AT_TARGET;
                            flags[boxx][boxy] = BOX;
                            map[focusX][focusY]->clear();
                            map[nx][ny]->clear();
                            map[boxx][boxy]->clear();
                            map[nx][ny]->setPixmap(QPixmap::fromImage(manAtTarget));
                            map[boxx][boxy]->setPixmap(QPixmap::fromImage(box));
                            focusX = nx;
                            focusY = ny;
                            step++;
                            qDebug()<<"branch 2";
                        }
                        else
                        {
                            qDebug()<<"error 7";
                            return;
                        }
                    }
                    else if(flags[focusX][focusY]==MAN_AT_TARGET)
                    {
                        if(flags[nx][ny]==BOX)
                        {
                            flags[focusX][focusY] = EMPTY_TARGET;
                            flags[nx][ny] = MAN;
                            flags[boxx][boxy] = BOX;
                            map[focusX][focusY]->clear();
                            map[nx][ny]->clear();
                            map[boxx][boxy]->clear();
                            map[focusX][focusY]->setPixmap(QPixmap::fromImage(target));
                            map[nx][ny]->setPixmap(QPixmap::fromImage(man));
                            map[boxx][boxy]->setPixmap(QPixmap::fromImage(box));
                            focusX = nx;
                            focusY = ny;
                            step++;
                            qDebug()<<"branch 3";
                        }
                        else if(flags[nx][ny]==BOX_AT_TARGET)
                        {
                            flags[focusX][focusY] = EMPTY_TARGET;
                            flags[nx][ny] = MAN_AT_TARGET;
                            flags[boxx][boxy] = BOX;
                            map[focusX][focusY]->clear();
                            map[nx][ny]->clear();
                            map[boxx][boxy]->clear();
                            map[focusX][focusY]->setPixmap(QPixmap::fromImage(target));
                            map[nx][ny]->setPixmap(QPixmap::fromImage(manAtTarget));
                            map[boxx][boxy]->setPixmap(QPixmap::fromImage(box));
                            focusX = nx;
                            focusY = ny;
                            step++;
                            qDebug()<<"branch 4";
                        }
                        else
                        {
                            qDebug()<<"error 7";
                            return;
                        }
                    }
                    else
                    {
                        qDebug()<<"error 6";
                        return;
                    }
                }
                else if(flags[boxx][boxy]==EMPTY_TARGET)
                {
                    if(flags[focusX][focusY]==MAN)
                    {
                        if(flags[nx][ny]==BOX)
                        {
                            flags[focusX][focusY] = EMPTY;
                            flags[nx][ny] = MAN;
                            flags[boxx][boxy] = BOX_AT_TARGET;
                            map[focusX][focusY]->clear();
                            map[nx][ny]->clear();
                            map[boxx][boxy]->clear();
                            //map[focusX][focusY]->setPixmap(QPixmap::fromImage(target));
                            map[nx][ny]->setPixmap(QPixmap::fromImage(man));
                            map[boxx][boxy]->setPixmap(QPixmap::fromImage(boxAtTarget));
                            focusX = nx;
                            focusY = ny;
                            step++;
                            qDebug()<<"branch 5";
                        }
                        else if(flags[nx][ny]==BOX_AT_TARGET)
                        {
                            flags[focusX][focusY] = EMPTY;
                            flags[nx][ny] = MAN_AT_TARGET;
                            flags[boxx][boxy] = BOX_AT_TARGET;
                            map[focusX][focusY]->clear();
                            map[nx][ny]->clear();
                            map[boxx][boxy]->clear();
                            //map[focusX][focusY]->setPixmap(QPixmap::fromImage(target));
                            map[nx][ny]->setPixmap(QPixmap::fromImage(manAtTarget));
                            map[boxx][boxy]->setPixmap(QPixmap::fromImage(boxAtTarget));
                            focusX = nx;
                            focusY = ny;
                            step++;
                            qDebug()<<"branch 6";
                        }
                        else
                        {
                            qDebug()<<"error 7";
                            return;
                        }
                    }
                    else if(flags[focusX][focusY]==MAN_AT_TARGET)
                    {
                        if(flags[nx][ny]==BOX)
                        {
                            flags[focusX][focusY] = EMPTY_TARGET;
                            flags[nx][ny] = MAN;
                            flags[boxx][boxy] = BOX_AT_TARGET;
                            map[focusX][focusY]->clear();
                            map[nx][ny]->clear();
                            map[boxx][boxy]->clear();
                            map[focusX][focusY]->setPixmap(QPixmap::fromImage(target));
                            map[nx][ny]->setPixmap(QPixmap::fromImage(man));
                            map[boxx][boxy]->setPixmap(QPixmap::fromImage(boxAtTarget));
                            focusX = nx;
                            focusY = ny;
                            step++;
                            qDebug()<<"branch 7";
                        }
                        else if(flags[nx][ny]==BOX_AT_TARGET)
                        {
                            flags[focusX][focusY] = EMPTY_TARGET;
                            flags[nx][ny] = MAN_AT_TARGET;
                            flags[boxx][boxy] = BOX_AT_TARGET;
                            map[focusX][focusY]->clear();
                            map[nx][ny]->clear();
                            map[boxx][boxy]->clear();
                            map[focusX][focusY]->setPixmap(QPixmap::fromImage(target));
                            map[nx][ny]->setPixmap(QPixmap::fromImage(manAtTarget));
                            map[boxx][boxy]->setPixmap(QPixmap::fromImage(boxAtTarget));
                            focusX = nx;
                            focusY = ny;
                            step++;
                            qDebug()<<"branch 8";
                        }
                        else
                        {
                            qDebug()<<"error 7";
                            return;
                        }
                    }
                    else
                    {
                        qDebug()<<"error 6";
                        return;
                    }
                }
                else
                {
                    qDebug()<<"error 5";
                    return;
                }
            }
        }
        else if(flags[nx][ny]==EMPTY||flags[nx][ny]==EMPTY_TARGET)
        {
            if(flags[nx][ny]==EMPTY)
            {
                if(flags[focusX][focusY]==MAN)
                {
                    flags[nx][ny] = MAN;
                    flags[focusX][focusY] = EMPTY;
                    map[nx][ny]->setPixmap(QPixmap::fromImage(man));
                    map[focusX][focusY]->clear();
                    focusX = nx;
                    focusY = ny;
                    qDebug()<<"branch 9";
                }
                else if(flags[focusX][focusY]==MAN_AT_TARGET)
                {
                    flags[nx][ny] = MAN;
                    flags[focusX][focusY] = EMPTY_TARGET;
                    map[nx][ny]->setPixmap(QPixmap::fromImage(man));
                    map[focusX][focusY]->clear();
                    map[focusX][focusY]->setPixmap(QPixmap::fromImage(target));
                    focusX = nx;
                    focusY = ny;
                    qDebug()<<"branch 10";
                }
                else
                {
                    qDebug()<<"error 1";
                    return;
                }
            }
            else if(flags[nx][ny]==EMPTY_TARGET)
            {
                if(flags[focusX][focusY]==MAN)
                {
                    flags[nx][ny] = MAN_AT_TARGET;
                    flags[focusX][focusY] = EMPTY;
                    map[nx][ny]->clear();
                    map[nx][ny]->setPixmap(QPixmap::fromImage(manAtTarget));
                    map[focusX][focusY]->clear();
                    focusX = nx;
                    focusY = ny;
                    qDebug()<<"branch 11";
                }
                else if(flags[focusX][focusY]==MAN_AT_TARGET)
                {
                    flags[nx][ny] = MAN_AT_TARGET;
                    flags[focusX][focusY] = EMPTY_TARGET;
                    map[nx][ny]->clear();
                    map[nx][ny]->setPixmap(QPixmap::fromImage(manAtTarget));
                    map[focusX][focusY]->clear();
                    map[focusX][focusY]->setPixmap(QPixmap::fromImage(target));
                    focusX = nx;
                    focusY = ny;
                    qDebug()<<"branch 12";
                    qDebug()<<"nx = "<<nx<<" ny = "<<ny;
                    qDebug()<<"flags[nx][ny] = "<<flags[nx][ny];
                }
                else
                {
                    qDebug()<<"error 2";
                    return;
                }
            }
            else
            {
                qDebug()<<"error 3";
                return;
            }
        }
        else
        {
            qDebug()<<"error 4";
            qDebug()<<"x:"<<focusX<<" y="<<focusY<<" nx="<<nx<<" ny="<<ny;
            return;
        }
    }
    for(int i=0;i<MAXN;i++)
    {
        for(int j=0;j<MAXN;j++)
        {
            map[i][j]->setStyleSheet("");
        }
    }
    char tmp[20];
    sprintf(tmp,"%d",step);
    step_label->setText(QString("目前推箱子的次数：")+tmp);
    if(isSuccess())
        whenSuccess();
    for(int i=0;i<9;i++)
    {
        for(int j=0;j<9;j++)
        {
            std::cout<<flags[i][j]<<' ';
        }
        std::cout<<std::endl;
    }
    std::cout<<std::endl;
    return;
}
QImage Test1::createImageWithOverlay(const QImage& baseImage, const QImage& overlayImage)
{
    QImage imageWithOverlay = QImage(baseImage.size(), QImage::Format_ARGB32_Premultiplied);
    QPainter painter(&imageWithOverlay);
    painter.setCompositionMode(QPainter::CompositionMode_Source);
    painter.fillRect(imageWithOverlay.rect(), Qt::transparent);
    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
    painter.drawImage(0, 0, baseImage);
    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
    painter.drawImage(0, 0, overlayImage);
    painter.end();

    return imageWithOverlay;
}
bool Test1::isSuccess()
{
    for(int i=0;i<MAXN;i++)
    {
        for(int j=0;j<MAXN;j++)
        {
            if(flags[i][j]==EMPTY_TARGET||flags[i][j]==BOX)
                return false;
        }
    }
    return true;
}
void Test1::whenSuccess()
{
    qDebug()<<"whenSuccess has benn called";
    QString add;
    if(step>maxStep)
        add="但步数还是不够少，继续努力哦";
    if(level==0)
    {

        QMessageBox::information(this,"congratulations","你通过了自定义关卡"+add);
    }
    else if(level<MAXLEVEL)
    {
        QMessageBox::information(this,"congratulations","你通过了本关卡!"+add);
        level++;
        char tmp[10];
        sprintf(tmp,"%d",level);
        level_label->setText(QString("LEVEL ")+QString(tmp));
        //qDebug()<<"ready to initMap with "<<level;
        initMap(level);
    }
    else
    {
        QMessageBox::information(this,"厉害了！","你已经通过了所有的关卡，接下来你可以试试自定义关卡"+add);
    }
}
void Test1::restart()
{
    for(int i=0;i<MAXN;i++)
    {
        for(int j=0;j<MAXN;j++)
        {
            map[i][j]->setStyleSheet("");
        }
    }
    initMap(level);
}
void Test1::startFromFile()
{
    QString fileName = QFileDialog::getOpenFileName(this,"选择自定义地图文件");
    if(fileName.isEmpty())
    {
        return;
    }
    QFile file(fileName);
    file.open(QFile::ReadOnly);
    QTextStream in(&file);
    QString content,tmp;
    in>>tmp;
    while(!tmp.isEmpty())
    {
        content += tmp;
        in>>tmp;
    }
    if(content.size()<81)
    {
        QMessageBox::warning(this,"警告","所选文件长度不足");
        return;
    }
    int cntM = 0;
    int cntT = 0;
    int cntB = 0;
    for(int i=0;i<81;i++)
    {
        char c = content.at(i).toLatin1();
        switch(c)
        {
        case '#':break;
        case '.':break;
        case 'B':
            cntB++;
            break;
        case 'T':
            cntT++;
            break;
        case 'M':
            cntM++;
            break;
        case 'H':
            cntT++;
            cntB++;
            break;
        default:
            QMessageBox::warning(this,"警告","所选文件含有非法字符");
            return;
        }
    }
    if(cntB!=cntT)
    {
        QMessageBox::warning(this,"警告","目标数和箱子数不等");
        return;
    }
    if(cntM!=1)
    {
        QMessageBox::warning(this,"警告","地图中应有且只有一个人");
        return;
    }
    customMap = content;
    qDebug()<<content;
    level_label->setText("自定义关卡");
    level = 0;
    for(int i=0;i<MAXN;i++)
    {
        for(int j=0;j<MAXN;j++)
        {
            map[i][j]->setStyleSheet("");
        }
    }
    initMap(0);
}
void Test1::help()
{
    //QDialog dialog(NULL);
    //dialog.setWindowTitle("关于");
    //QLabel label(&dialog);
    QMessageBox dialog;
    dialog.setWindowTitle("关于");
    dialog.setText("<meta charset=\"utf-8\">"
                   "<h5>帮助</h5>"
                   "<p>若要制作自定义关卡的地图，只需要自己创建一个txt文件，里面写一个用字符组成的，九行九列的字符串即可。其中#代表墙，.代表空地，B代表箱子，M代表人，T代表目的地，H代表在目的地的箱子。注意所有的字符一定得是英文格式。一个典型的地图如下</p>"
                   "#########<br>"
                   "###M.####<br>"
                   "###.B..##<br>"
                   "####.#.##<br>"
                   "##T#.#..#<br>"
                   "##TB..#.#<br>"
                   "##T...B.#<br>"
                   "#########<br>"
                   "#########<br>"
                   "<hr>"
                   "本程序由南通大学软件162班胡亚伟同学制作。");
    //label.setTextFormat(Qt::RichText);
    dialog.addButton(QMessageBox::Ok);
    dialog.exec();
}
using namespace std;
void Test1::showTip()
{
    vector<Point> v = getNextMove(flags);
    if(v.size()==0)
    {
        QMessageBox::warning(this,"警告","已经不可能赢了，重新来吧");
        return;
    }
    for(size_t i=0;i<v.size();i++)
    {
        Point t(v[i].x,v[i].y);
        if(flags[t.x][t.y]!=BOX&&flags[t.x][t.y]!=BOX_AT_TARGET)
        {
            map[t.x][t.y]->setStyleSheet("border-width: 3px;border-style: solid;border-color: rgb(255, 0, 0);");
        }
        for(int i=0;i<MAXN;i++)
        {
            for(int j=0;j<MAXN;j++)
            {
                if(flags[i][j]==BOX||flags[i][j]==BOX_AT_TARGET)
                {
                    //cout<<(std::find(v.begin(),v.end(),Point(i,j))==v.end());
                    if(std::find(v.begin(),v.end(),Point(i,j))==v.end())
                    {
                        map[i][j]->setStyleSheet("border-width: 3px;border-style: solid;border-color: rgb(255, 0, 0);");
                    }
                }
            }
        }
    }
}
