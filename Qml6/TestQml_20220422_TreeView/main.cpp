#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "MyTreeModel.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    qmlRegisterType<MyTreeModel>("MyModel", 1, 0, "MyTreeModel");

    QQmlApplicationEngine engine;
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.loadFromModule("TestQml_20220422_TreeView", "Main");

    return app.exec();
}
