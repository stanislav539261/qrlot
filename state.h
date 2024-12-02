#ifndef STATE_H
#define STATE_H

#include <QList>
#include <QPixmap>
#include <QString>
#include <QThread>

struct State {
    QList<QString> currentValues;
    QString lastExportDirectoryPath;
    QString lastExportFormat;
    QString lastExportFullPath;
    int lastExportSize;
    QString lastOpenTableFullPath;
    QString lastSaveTableFullPath;
};

extern State *g_State;

#endif // STATE_H
