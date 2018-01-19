#ifndef TEST1_H
#define TEST1_H

#include <QWidget>
#include <QKeyEvent>
#include <Qlabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QPushButton>
#include <QImage>
#include <QPixmap>
#include <QAction>
#include <string>
using namespace std;
class Test1 : public QWidget
{
    Q_OBJECT
private:
    enum {WALL,EMPTY,MAN,BOX,EMPTY_TARGET,BOX_AT_TARGET,MAN_AT_TARGET};
    int focusX,focusY;
    int level;
    int step;
    int maxStep;
    static const int MAXLEVEL = 4;
    static const int MAXN = 9;
    int flags[MAXN][MAXN];
    QString customMap;
    QAction *loadFromFile;
    QAction *about;
    QGridLayout *gridLayout;
    QPushButton *reset_button;
    QLabel *level_label;
    QLabel *step_label;
    QLabel *max_step_label;
    QImage block,man,target,box,manAtTarget,boxAtTarget;
    QLabel* map[MAXN][MAXN];
    void initMap(int l);
    bool isSuccess();
    void whenSuccess();
    void restart();
    void startFromFile();
    void help();
    void showTip();
public:
    explicit Test1(QWidget *parent = 0);
    //~Test1();
    static QImage createImageWithOverlay(const QImage&,const QImage&);
signals:

public slots:
protected:
    void keyReleaseEvent(QKeyEvent* event);
};

#endif // TEST1_H
