#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <set>
#include <sstream>
#include <tuple>

#include <QLineEdit>
#include <QtConcurrent>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    running(false),
    battles(0),
    won(0),
    totalRounds(0),
    totalAliveLeft(0),
    totalGood(0),
    totalAtLeastOneGoodDead(0),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->charTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeMode::Fixed);
    ui->charTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeMode::Stretch);
    ui->charTable->horizontalHeader()->resizeSections(QHeaderView::ResizeMode::ResizeToContents);
    ui->charTable->horizontalHeader()->resizeSection(0, 30);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_addCharButton_clicked()
{
    const auto newRowNumber = ui->charTable->rowCount();
    ui->charTable->setRowCount(newRowNumber + 1);

    // Allegiance
    QTableWidgetItem *item = new QTableWidgetItem();
    item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
    item->setTextAlignment(Qt::AlignCenter);
    item->setCheckState(Qt::Checked);
    ui->charTable->setItem(newRowNumber, 0, item);

    // Character name
    item = new QTableWidgetItem(tr("Character %1").arg(QString::number(newRowNumber)));
    item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled);
    ui->charTable->setItem(newRowNumber, 1, item);

    // Skills
    for (auto i = 2; i < (ui->charTable->columnCount() - 1); ++i) {
        QLineEdit *edit = new QLineEdit(ui->charTable);
        edit->setText("0");
        edit->setAlignment(Qt::AlignCenter);
        edit->setFrame(false);
        edit->setValidator(new QIntValidator(edit));
        ui->charTable->setCellWidget(newRowNumber, i, edit);
    }

    // Evade skill
    item = new QTableWidgetItem();
    item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
    item->setTextAlignment(Qt::AlignCenter);
    item->setCheckState(Qt::Unchecked);
    ui->charTable->setItem(newRowNumber, ui->charTable->columnCount() - 1, item);
}

void MainWindow::on_deleteCharButton_clicked()
{
    std::set<int, std::greater<int>> rowsToRemove;
    for (const auto *item : ui->charTable->selectedItems()) {
        rowsToRemove.insert(item->row());
    }

    for (const auto row : rowsToRemove) {
        ui->charTable->removeRow(row);
    }
}

void MainWindow::on_clearAllCharButton_clicked()
{
    ui->charTable->clear();
}

void MainWindow::on_startStopButton_clicked()
{
    if (!running) {
        running = true;
        ui->simulationCounter->display(0);
        ui->startStopButton->setText(tr("Stop"));
        battles = 0;
        won = 0;
        totalRounds = 0;
        totalAliveLeft = 0;
        totalGood = 0;
        totalAtLeastOneGoodDead = 0;

        simulationCharacters.reserve(ui->charTable->rowCount());
        for (auto row = 0; row < ui->charTable->rowCount(); ++row) {
            if (ui->charTable->item(row, 0)->checkState() == Qt::Checked) {
                ++totalGood;
            }
            simulationCharacters.emplace_back(ui->charTable->item(row, 0)->checkState() == Qt::Checked,
                               ui->charTable->item(row, 1)->text(),
                               qobject_cast<QLineEdit *>(ui->charTable->cellWidget(row, 2))->text().toInt(),
                               qobject_cast<QLineEdit *>(ui->charTable->cellWidget(row, 3))->text().toInt(),
                               qobject_cast<QLineEdit *>(ui->charTable->cellWidget(row, 4))->text().toInt(),
                               qobject_cast<QLineEdit *>(ui->charTable->cellWidget(row, 5))->text().toInt(),
                               qobject_cast<QLineEdit *>(ui->charTable->cellWidget(row, 6))->text().toInt(),
                               qobject_cast<QLineEdit *>(ui->charTable->cellWidget(row, 7))->text().toInt(),
                               qobject_cast<QLineEdit *>(ui->charTable->cellWidget(row, 8))->text().toInt(),
                               qobject_cast<QLineEdit *>(ui->charTable->cellWidget(row, 9))->text().toInt(),
                               qobject_cast<QLineEdit *>(ui->charTable->cellWidget(row, 10))->text().toInt(),
                               qobject_cast<QLineEdit *>(ui->charTable->cellWidget(row, 11))->text().toInt(),
                               ui->charTable->item(row, 12)->checkState() == Qt::Checked);
        }

        const auto threads = QThreadPool::globalInstance()->maxThreadCount();
        for (int t = 0; t < threads; ++t) {
            scheduleNewSimulation();
        }
    } else {
        running = false;
        ui->startStopButton->setText(tr("Start"));
        simulationCharacters.clear();
    }
}

void MainWindow::scheduleNewSimulation()
{
    simulators.emplace_back(std::piecewise_construct,
                            std::tuple<>(),
                            std::make_tuple(std::cref(running), simulationCharacters));

    auto& simulator = simulators.back();

    simulator.first.connect(std::addressof(simulator.first), &decltype(simulator.first)::finished,
                    std::bind(&MainWindow::simulationFinished, this, std::prev(simulators.end())));
    const auto future = QtConcurrent::run(std::addressof(simulator.second), &Simulator::simulate);
    simulator.first.setFuture(future);
}

void MainWindow::simulationFinished(decltype(simulators)::iterator it)
{
    if (it->first.result()) {
        ++battles;
        if (it->second.goodWon()) {
            ++won;
        }
        totalRounds += it->second.rounds();
        if (it->second.goodWon() && it->second.goodAlive() < totalGood) {
            ++totalAtLeastOneGoodDead;
        }

        ui->summaryTextBrowser->setText(
            tr("Good guys won %1% of the battles").arg((static_cast<double>(won) / battles) * 100, 0, 'f', 2)
            + QStringLiteral("\n") + tr("Avg number of rounds: %1").arg(totalRounds/battles)
            + QStringLiteral("\n") + tr("Probability that at least one member will die even if the battle is won: %1")
                    .arg((static_cast<double>(totalAtLeastOneGoodDead) / won) * 100, 0, 'f', 2));
        ui->simulationCounter->display(ui->simulationCounter->intValue() + 1);
        simulators.erase(it);
        if (running) {
            scheduleNewSimulation();
        }
    }
}
