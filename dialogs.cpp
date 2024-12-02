#include <QFileDialog>
#include <QProgressDialog>

#include "xlsxdocument.h"
#include "QZXing.h"

#include "dialogs.h"
#include "state.h"
#include "./ui_exportpicturesdialog.h"
#include "./ui_exportspreadsheetdialog.h"

void ExportPicturesWork::abort() {
    emit isCompleted();
}

void ExportPicturesWork::savePictures() {
    const auto &values = g_State->currentValues;
    const auto &directoryPath = g_State->lastExportDirectoryPath;
    const auto &format = g_State->lastExportFormat;
    const auto &size = g_State->lastExportSize;

    auto percentage = 0.f;

    emit updateProgressBar(tr("Wait..."), percentage);

    for (const auto &value : values) {
        auto fullPath = QString();

        if (!value.isNull() && !value.isEmpty()) {
            fullPath += directoryPath + QDir::separator() + value;

            if (format == "JPEG") {
                fullPath += ".jpg";
            } else if (format ==  "PNG") {
                fullPath += ".png";
            }

            auto file = QFile(fullPath);

            if (file.open(QIODevice::ReadWrite)) {
                const auto image = QZXing::encodeData(
                    value.toUtf8(),
                    QZXing::EncoderFormat_QR_CODE,
                    QSize(size, size),
                    QZXing::EncodeErrorCorrectionLevel_L,
                    true
                );

                image.save(&file, format.toUtf8());
            }
        }

        percentage += 1.f / static_cast<float>(values.length());

        emit updateProgressBar(fullPath, percentage);
    }

    emit isCompleted();
}

ExportPicturesDialog::ExportPicturesDialog(QWidget *parent) : QDialog(parent),
    progressBar(*new ProgressBar(parent)),
    ui(*new Ui::ExportPicturesDialog)
{
    ui.setupUi(this);
}

ExportPicturesDialog::~ExportPicturesDialog() {
    delete &ui;
}

void ExportPicturesDialog::on_buttonBox_accepted() {
    const auto formatComboBox = this->ui.formatComboBox;
    const auto sizeSpinBox = this->ui.sizeSpinBox;

    const auto format = formatComboBox->currentText().trimmed();
    const auto size = sizeSpinBox->value();
    const auto directoryPath = QFileDialog::getExistingDirectory(this, tr("Export files — Qrlot"), g_State->lastExportDirectoryPath);

    if (!directoryPath.isNull() && !directoryPath.isEmpty()) {
        g_State->lastExportDirectoryPath = directoryPath;
        g_State->lastExportFormat = format;
        g_State->lastExportSize = size;

        auto thread = new QThread(this->parent());

        auto worker = new ExportPicturesWork();
        worker->moveToThread(thread);

        QObject::connect(&this->progressBar, SIGNAL(isCanceled()), worker, SLOT(abort()));
        QObject::connect(worker, SIGNAL(isCompleted()), thread, SLOT(quit()));
        QObject::connect(thread, SIGNAL(started()), &this->progressBar, SLOT(show()));
        QObject::connect(thread, SIGNAL(started()), worker, SLOT(savePictures()));
        QObject::connect(thread, SIGNAL(finished()), &this->progressBar, SLOT(close()));
        QObject::connect(thread, SIGNAL(finished()), worker, SLOT(deleteLater()));
        QObject::connect(worker, SIGNAL(updateProgressBar(QString, float)), &this->progressBar, SLOT(updateBar(QString, float)));

        thread->start();
    }
}

void ExportSpreadsheetWork::saveSpreadsheet() {
    const auto &values = g_State->currentValues;
    const auto &fullPath = g_State->lastExportFullPath;
    const auto &size = g_State->lastExportSize;

    auto percentage = 0.f;

    emit updateProgressBar(tr("Wait..."), percentage);

    if (!fullPath.isNull() && !fullPath.isEmpty()) {
        auto file = QFile(fullPath);
        auto workbook = QXlsx::Document();

        if (file.open(QIODevice::ReadWrite)) {
            auto row = 1;

            workbook.setColumnWidth(2, static_cast<double>(size) * .142542253338);

            for (const auto &value : values) {
                if (!value.isNull() && !value.isEmpty()) {
                    workbook.write(row, 1, value);
                    workbook.setRowHeight(row, static_cast<double>(size) * .7516629711);

                    const auto image = QZXing::encodeData(
                        value.toUtf8(),
                        QZXing::EncoderFormat_QR_CODE,
                        QSize(size, size),
                        QZXing::EncodeErrorCorrectionLevel_L,
                        true
                        );

                    workbook.insertImage(row - 1, 1, image);
                    row++;
                }

                percentage += 1.f / static_cast<float>(values.length());

                emit updateProgressBar(fullPath, percentage);
            }
        }

        workbook.autosizeColumnWidth(1, 1);
        workbook.saveAs(fullPath);
    }

    emit isCompleted();
}

ExportSpreadsheetDialog::ExportSpreadsheetDialog(QWidget *parent) : QDialog(parent),
    progressBar(*new ProgressBar(parent)),
    ui(*new Ui::ExportSpreadsheetDialog)
{
    ui.setupUi(this);
}

ExportSpreadsheetDialog::~ExportSpreadsheetDialog() {
    delete &ui;
}

void ExportSpreadsheetDialog::on_buttonBox_accepted() {
    const auto sizeSpinBox = this->ui.sizeSpinBox;
    const auto size = sizeSpinBox->value();

    const auto fullPath = QFileDialog::getSaveFileName(
        this,
        tr("Export file — Qrlot"),
        QDir::homePath(),
        "Excel Files (*.xlsx)"
    );

    auto directoryPath = fullPath;
    directoryPath.remove(QUrl(fullPath).fileName());

    if (!fullPath.isNull() && !fullPath.isEmpty()) {
        g_State->lastExportDirectoryPath = directoryPath;
        g_State->lastExportFullPath = fullPath;
        g_State->lastExportSize = size;

        auto thread = new QThread(this->parent());

        auto worker = new ExportSpreadsheetWork();
        worker->moveToThread(thread);

        QObject::connect(worker, SIGNAL(isCompleted()), thread, SLOT(quit()));
        QObject::connect(thread, SIGNAL(started()), &this->progressBar, SLOT(show()));
        QObject::connect(thread, SIGNAL(started()), worker, SLOT(saveSpreadsheet()));
        QObject::connect(thread, SIGNAL(finished()), &this->progressBar, SLOT(close()));
        QObject::connect(thread, SIGNAL(finished()), worker, SLOT(deleteLater()));
        QObject::connect(worker, SIGNAL(updateProgressBar(QString, float)), &this->progressBar, SLOT(updateBar(QString, float)));

        thread->start();
    }
}

void ExportSpreadsheetDialog::on_sizeSpinBox_valueChanged(int size) {
    const auto sizeSpinBox = this->ui.sizeSpinBox;

    sizeSpinBox->setSuffix(tr(" px (%1 cm)").arg(QString::number(static_cast<double>(size) * .026484375, 'f', 2)));
}

