#pragma once
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "game.h"
#include "controller.h"

#include <QKeyEvent>
#include <QDebug>
#include <QMainWindow>
#include <QTimer>
#include <D:/C++/cpp-qt/mocks_library/prac/QTimer>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(Game& game, Controller& controller, QWidget *parent = nullptr);
    ~MainWindow();

    void keyPressEvent(QKeyEvent *event) override {
        if (event->isAutoRepeat()) {
            return QWidget::keyPressEvent(event);
        }

        switch (event->key()) {
        case Qt::Key_Right:
            controller_.OnMoveKey(Direction::kRight);
            break;
        case Qt::Key_Left:
            controller_.OnMoveKey(Direction::kLeft);
            break;
        case Qt::Key_Up:
            controller_.OnMoveKey(Direction::kUp);
            break;
        case Qt::Key_Down:
            controller_.OnMoveKey(Direction::kDown);
            break;
        default:
            QWidget::keyPressEvent(event); // call base class implementation
        }
    }

    void paintEvent(QPaintEvent *) override{
        QPainter qpaint{this};
        Painter painter(qpaint);
        FlashlightDarkener darkener(game_.GetPlayer().GetPosition(), 7, game_.GetPlayer().GetDirection(), 4);
        DrawContext draw_context{.painter = painter, .darkener = darkener};

        Size window_size{width(), height()};
        auto player_pos = game_.GetPlayer().GetExactPos();
        auto field_size = game_.GetField().GetRect();

        painter.PreparePaint(player_pos, field_size, window_size);
        game_.DrawFrame(draw_context);

    }
private slots:
    void timer_Tick();

private:
    Controller controller_;
    Ui::MainWindow *ui;
    Game& game_;
    prac::QTimer qtimer_{this};
};
#endif // MAINWINDOW_H

