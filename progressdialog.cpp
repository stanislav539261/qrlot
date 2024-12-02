#include "progressdialog.h"
#include "ui_progressdialog.h"

ProgressBar::ProgressBar(QWidget *parent) : QWidget(parent),
    ui(new Ui::ProgressBar)
{
    ui->setupUi(this);
}

ProgressBar::~ProgressBar() {
    delete ui;
}

void ProgressBar::updateBar(QString file, float percentage) {
    if (!file.isNull() && !file.isEmpty()) {
        this->ui->label->setText(file);
    }

    this->ui->progressBar->setValue(std::min(static_cast<int>(percentage * 100.f), 100));
}

void ProgressBar::on_buttonBox_rejected() {
    emit this->isCanceled();
}

