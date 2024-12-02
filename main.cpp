#include "state.h"
#include "window.h"

#include <QApplication>
#include <QDir>
#include <QLocale>
#include <QTranslator>

int main(int argc, char *argv[]) {
    auto app = QApplication(argc, argv);
    auto translator = QTranslator();

    const auto uiLanguages = QLocale::system().uiLanguages();

    for (const auto &locale : uiLanguages) {
        const auto baseName = "qrlot_" + QLocale(locale).name();

        if (translator.load(":/i18n/" + baseName)) {
            app.installTranslator(&translator);
            break;
        }
    }

    auto &state = *new State {
        .currentValues = QList<QString>(),
        .lastExportDirectoryPath = QDir::homePath(),
        .lastExportFormat = QString(),
        .lastExportFullPath = QString(),
        .lastExportSize = 0,
        .lastOpenTableFullPath = QString(),
        .lastSaveTableFullPath = QString(),
    };

    g_State = &state;

    auto mainWindow = MainWindow();
    mainWindow.show();

    return app.exec();
}
