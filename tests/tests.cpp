
// Core headers
#include <QtTest>
#include <QtWidgets/QApplication>
#include <memory>

#include "level_loader.h"
#include "utility/painter.h"
#include "utility/geometry.h"

class TestYourApp : public QObject
{
    Q_OBJECT
public:
    TestYourApp()=default;
    ~TestYourApp()=default;

private slots:

    void init();
    void cleanup();

    void TestLoadField();
    void TestLoadPlayer();
    void TestLoadFloors();

private:
    std::shared_ptr<Game> game;

};


void TestYourApp::init()
{
    game.reset();

    game = std::make_shared<Game>();

    LevelLoader level_loader;
    level_loader.LoadFromFile(*game, QString(":/resources/levels/main_level.json"));
}

void TestYourApp::TestLoadField() {
    const auto field = game->GetField();
    QVERIFY(field.GetWidth() == 30);
    QVERIFY(field.GetHeight() == 30);
}

void TestYourApp::TestLoadPlayer() {
    const auto player = game->GetPlayer();
    QVERIFY(player.IsActive());

    const auto position = player.GetExactPos();
    const double epsilon = 1e-6;

    QVERIFY(std::abs(position.x_pos - 14) < epsilon);
    QVERIFY(std::abs(position.y_pos - 15) < epsilon);
    QVERIFY(std::abs(position.z_pos - 0) < epsilon);

    QVERIFY(player.GetDirection() == Direction::kRight);
}

void TestYourApp::TestLoadFloors() {
    QVERIFY(game->GetCurrentFloor().GetLevel() == 0);

    auto field = game->GetField();
    QVERIFY(field.GetFloor(0).GetLevel() == 0);
    QVERIFY(field.GetFloor(1).GetLevel() == 1);

    auto floor = field.GetFloor(0);
    auto wall = floor.GetWall({5, 10}, Direction::kUp);
    const auto player = game->GetPlayer();
    QVERIFY(player.IsActive());
    QVERIFY(wall != nullptr);
    QVERIFY(!(wall->CanPass(player, Direction::kRight)));
    QVERIFY(!(wall->CanPass(player, Direction::kLeft)));
    QVERIFY(!(wall->CanPass(player, Direction::kUp)));
    QVERIFY(!(wall->CanPass(player, Direction::kDown)));
}

void TestYourApp::cleanup()
{
}

QTEST_MAIN(TestYourApp)

#include "tests.moc"
