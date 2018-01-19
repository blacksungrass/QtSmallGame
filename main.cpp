#include <QApplication>
#include <QLabel>
#include <QPushButton>
#include <QDebug>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QWidget>
#include <QIcon>
#include <QSizePolicy>
#include <QSpinBox>
#include <QSlider>
#include <QIMage>
#include <QLabel>
#include <QMenuBar>
#include <QPicture>
#include <QPixmap>
#include <QKeyEvent>
#include <QMainWindow>
#include <QDialog>
#include <QMessageBox>
#include <QFileDialog>
#include "test1.h"
int main(int argc, char *argv[])
{
    QApplication a(argc,argv);
    Test1 test;
    test.setWindowTitle("推箱子");
    test.show();
    return a.exec();
}
