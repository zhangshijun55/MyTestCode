#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>
#include <QTableWidgetItem>
#include <QDateTime>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("TaskManager");

    // 添加一条测试任务
    connect(ui->btnRun, &QPushButton::clicked, this, &MainWindow::onRun);
    // 取消所有任务
    connect(ui->btnCancel, &QPushButton::clicked, this, &MainWindow::onCancel);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onRun()
{
    // 先创建future watcher关联信号槽
    auto pwatcher = TaskManager::instance()->create();
    auto watcher = pwatcher.get();

    // 列表更新
    auto table = ui->tableWidget;
    int row = table->rowCount();
    table->insertRow(row);
    QString time = QTime::currentTime().toString("hh:mm:ss.zzz");
    auto title = new QLabel(time);
    table->setCellWidget(row, 0, title);
    auto bar = new QProgressBar();
    table->setCellWidget(row, 1, bar);
    auto btn = new QPushButton("Cancel");
    table->setCellWidget(row, 2, btn);
    connect(btn, &QPushButton::clicked, [pwatcher]{ pwatcher->cancel(); });
    // 测试对象释放
    connect(btn, &QPushButton::destroyed, this, [time]{ qDebug() << "free" << time; });

    // 状态更新
    connect(watcher, &TaskManager::TWatcher::progressTextChanged,
            this, [](const QString &text){
                // qDebug() << "progress text changed" << text;
            });
    connect(watcher, &TaskManager::TWatcher::progressRangeChanged,
            this, [this, bar](int min, int max){
                // qDebug() << "progress range changed" << min << max;
                bar->setRange(min, max);
                bar->setValue(min);
            });
    connect(watcher, &TaskManager::TWatcher::progressValueChanged,
            this, [this, bar](int value){
                // qDebug() << "progress changed" << value;
                bar->setValue(value);
            });
    connect(watcher, &TaskManager::TWatcher::started,
            this, [time](){
                qDebug() << "started" << time;
            });
    connect(watcher, &TaskManager::TWatcher::finished,
            this, [this, table, time, btn, pwatcher](){
                qDebug() << "finished" << time;
                int index = -1;
                for (int row = 0; row < table->rowCount(); ++row) {
                    QPushButton *item = static_cast<QPushButton *>(table->cellWidget(row, 2));
                    if (item && item == btn) {
                        index = row;
                        break;
                    }
                }
                if (index < 0) return;
                ui->tableWidget->removeRow(index);
            });
    connect(watcher, &TaskManager::TWatcher::canceled,
            this, [this, time](){
                qDebug() << "canceled" << time;
            });
    connect(watcher, &TaskManager::TWatcher::resultReadyAt,
            this, [this, time, pwatcher](int index){
                qDebug() << "result" << time << index << pwatcher->resultAt(index);
            });


    // 执行任务，并通过关联的future watcher更新ui
    auto task = [](TaskManager::TPromise &promise){
        promise.setProgressRange(0, 10);
        for (int i = 0; i < 10 && !promise.isCanceled(); ++i)
        {
            promise.setProgressValue(i);
            QThread::sleep(1);
        }
        if (promise.isCanceled())
            return;
        promise.setProgressValue(10);
        promise.addResult(nullptr);
    };
    TaskManager::instance()->run(task, pwatcher);
}

void MainWindow::onCancel()
{
    TaskManager::instance()->cancel();
}

