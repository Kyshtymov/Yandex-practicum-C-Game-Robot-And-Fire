#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
MainWindow::MainWindow(Game& game, Controller& controller, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , game_(game)
    , controller_(controller)
{
    ui->setupUi(this);

     controller_.SetRedrawCallback([this, last_draw = std::optional<std::chrono::steady_clock::time_point>{}]() mutable {
       repaint();
    });

    qtimer_.start(40);
    // Подключаем сигнал таймера к слоту on_timer_timeout.
    connect(&qtimer_, &prac::QTimer::timeout, this, &MainWindow::timer_Tick);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::timer_Tick(){
    controller_.Tick();
}
