#pragma once

#include "character.h"
#include "simulator.h"

#include <atomic>
#include <list>
#include <QFutureWatcher>
#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_addCharButton_clicked();
    void on_deleteCharButton_clicked();
    void on_clearAllCharButton_clicked();
    void on_startStopButton_clicked();
    void on_duplicateCharButton_clicked();

private:
    void addNewRow();
    void scheduleNewSimulation();

    std::atomic<bool> running;
    int battles;
    int won;
    int totalRounds;
    int totalAliveLeft;
    int totalGood;
    int totalAtLeastOneGoodDead;
    Ui::MainWindow *ui;
    std::vector<Character> simulationCharacters;
    std::list<std::pair<QFutureWatcher<decltype(std::declval<Simulator>().simulate())>, Simulator>> simulators;

    void simulationFinished(decltype(simulators)::iterator);
};
