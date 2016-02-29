#include <QApplication>
#include <QQmlApplicationEngine>
#include <QtQml>
#include "accountmanager.h"
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QQmlApplicationEngine engine;
    qmlRegisterType<AccountManager>("com.nrauh", 1, 0, "AccountManager");

    AccountManager accManager;
    QUrl lastFilePath = accManager.getLastFile();
    QFile lastFile(lastFilePath.toLocalFile());

    if (lastFile.fileName() != "" && lastFile.exists()) {
        engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    } else {
        engine.load(QUrl(QStringLiteral("qrc:/newbudget.qml")));
    }

    return app.exec();
}
