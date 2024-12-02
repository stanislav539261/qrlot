#ifndef DIALOGS_H
#define DIALOGS_H

#include <QDialog>

#include "progressdialog.h"

namespace Ui {
    class ExportPicturesDialog;
    class ExportSpreadsheetDialog;
}

class ExportPicturesWork : public QObject {
    Q_OBJECT

public slots:
    void abort();
    void savePictures();

signals:
    void isCompleted();
    void updateProgressBar(QString, float);
};

class ExportPicturesDialog : public QDialog {
    Q_OBJECT

public:
    explicit ExportPicturesDialog(QWidget *parent = nullptr);
    ~ExportPicturesDialog();

private slots:
    void on_buttonBox_accepted();

private:
    ProgressBar &progressBar;
    Ui::ExportPicturesDialog &ui;
};

class ExportSpreadsheetWork : public QObject {
    Q_OBJECT

public slots:
    void saveSpreadsheet();

signals:
    void isCompleted();
    void updateProgressBar(QString, float);
};

class ExportSpreadsheetDialog : public QDialog {
    Q_OBJECT

public:
    explicit ExportSpreadsheetDialog(QWidget *parent = nullptr);
    ~ExportSpreadsheetDialog();

private slots:
    void on_buttonBox_accepted();

    void on_sizeSpinBox_valueChanged(int arg1);

private:
    ProgressBar &progressBar;
    Ui::ExportSpreadsheetDialog &ui;
};

#endif // DIALOGS_H
