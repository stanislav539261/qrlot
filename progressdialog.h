#ifndef PROGRESSDIALOG_H
#define PROGRESSDIALOG_H

#include <QFutureWatcher>
#include <QWidget>

namespace Ui {
    class ProgressBar;
}

class ProgressBar : public QWidget {
    Q_OBJECT

public:
    explicit ProgressBar(QWidget *parent = nullptr);
    ~ProgressBar();

public slots:
    void updateBar(QString file, float percentage);

signals:
    void isCanceled();

private slots:
    void on_buttonBox_rejected();

private:
    Ui::ProgressBar *ui;
};

#endif // PROGRESSDIALOG_H
