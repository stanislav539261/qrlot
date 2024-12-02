#include <QAction>
#include <QFile>
#include <QFileDialog>
#include <QClipboard>
#include <QMessageBox>
#include <QShortcut>
#include <QTextStream>
#include <QWheelEvent>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

#include "QZXing.h"

#include "state.h"
#include "window.h"
#include "./ui_window.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),
    exportPicturesDialog(*new ExportPicturesDialog()),
    exportSpreadsheetDialog(*new ExportSpreadsheetDialog()),
    ui(*new Ui::MainWindow)
{
    this->pictureSize = 100;
    this->ui.setupUi(this);

    // Setup table widget
    this->clearTable();

    // Setup menus
    auto &fileNew = *new QAction(tr("&New Table"), this);
    fileNew.setIcon(QIcon::fromTheme("document-new"));
    fileNew.setShortcuts(QKeySequence::New);

    auto &fileOpen = *new QAction(tr("&Open Table..."), this);
    fileOpen.setIcon(QIcon::fromTheme("document-open"));
    fileOpen.setShortcuts(QKeySequence::Open);

    auto &fileSave = *new QAction(tr("&Save..."), this);
    fileSave.setIcon(QIcon::fromTheme("document-save"));
    fileSave.setShortcuts(QKeySequence::Save);

    auto &fileSaveAs = *new QAction(tr("&Save As..."), this);
    fileSaveAs.setIcon(QIcon::fromTheme("document-save-as"));
    fileSaveAs.setShortcuts(QKeySequence::SaveAs);

    auto &fileExportAsPictures = *new QAction(tr("&Export As Pictures..."), this);
    fileExportAsPictures.setIcon(QIcon::fromTheme("document-export"));

    auto &fileExportAsSpreadsheet = *new QAction(tr("&Export As Spreadsheet..."), this);

    auto &fileQuit = *new QAction(tr("&Quit"), this);
    fileQuit.setIcon(QIcon::fromTheme("application-exit"));
    fileQuit.setShortcuts(QKeySequence::Quit);

    QObject::connect(&fileNew, &QAction::triggered, this, &MainWindow::newTable);
    QObject::connect(&fileOpen, &QAction::triggered, this, &MainWindow::openTable);
    QObject::connect(&fileSave, &QAction::triggered, this, &MainWindow::saveTable);
    QObject::connect(&fileSaveAs, &QAction::triggered, this, &MainWindow::saveTableAs);
    QObject::connect(&fileExportAsPictures, &QAction::triggered, this, &MainWindow::exportAsPictures);
    QObject::connect(&fileExportAsSpreadsheet, &QAction::triggered, this, &MainWindow::exportAsSpreadsheet);
    QObject::connect(&fileQuit, &QAction::triggered, this, &MainWindow::close);

    const auto menuFile = this->ui.menuFile;
    menuFile->addAction(&fileNew);
    menuFile->addAction(&fileOpen);
    menuFile->addSeparator();
    menuFile->addAction(&fileSave);
    menuFile->addAction(&fileSaveAs);
    menuFile->addSeparator();
    menuFile->addAction(&fileExportAsPictures);
    menuFile->addAction(&fileExportAsSpreadsheet);
    menuFile->addSeparator();
    menuFile->addAction(&fileQuit);

    auto &editCopy = *new QAction(tr("&Copy"), this);
    editCopy.setIcon(QIcon::fromTheme("edit-copy"));
    editCopy.setShortcuts(QKeySequence::Copy);

    auto &editCut = *new QAction(tr("&Cut"), this);
    editCut.setIcon(QIcon::fromTheme("edit-cut"));
    editCut.setShortcuts(QKeySequence::Cut);

    auto &editPaste = *new QAction(tr("&Paste"), this);
    editPaste.setIcon(QIcon::fromTheme("edit-paste"));
    editPaste.setShortcuts(QKeySequence::Paste);

    auto &editDelete = *new QAction(tr("&Delete selected"), this);
    editDelete.setShortcuts(QKeySequence::Delete);

    QObject::connect(&editCopy, &QAction::triggered, this, &MainWindow::copyToClipboard);
    QObject::connect(&editCut, &QAction::triggered, this, &MainWindow::cutToClipboard);
    QObject::connect(&editPaste, &QAction::triggered, this, &MainWindow::pasteFromClipboard);
    QObject::connect(&editDelete, &QAction::triggered, this, &MainWindow::deleteSelectedRows);

    const auto menuEdit = this->ui.menuEdit;
    menuEdit->addAction(&editCopy);
    menuEdit->addAction(&editCut);
    menuEdit->addAction(&editPaste);
    menuEdit->addAction(&editDelete);

    auto &zoomIn = *new QAction(tr("&Zoom In"), this);
    zoomIn.setIcon(QIcon::fromTheme("zoom-in"));
    zoomIn.setShortcuts(QKeySequence::ZoomIn);

    auto &zoomOut = *new QAction(tr("&Zoom Out"), this);
    zoomOut.setIcon(QIcon::fromTheme("zoom-out"));
    zoomOut.setShortcuts(QKeySequence::ZoomOut);

    QObject::connect(&zoomIn, &QAction::triggered, this, &MainWindow::zoomIn);
    QObject::connect(&zoomOut, &QAction::triggered, this, &MainWindow::zoomOut);

    const auto menuView = this->ui.menuView;
    menuView->addAction(&zoomIn);
    menuView->addAction(&zoomOut);

    auto &aboutProgram = *new QAction(tr("&About the program"), this);
    aboutProgram.setIcon(QIcon::fromTheme("info"));
    aboutProgram.setShortcuts(QKeySequence::HelpContents);

    QObject::connect(&aboutProgram, &QAction::triggered, this, &MainWindow::about);

    const auto menuHelp = this->ui.menuHelp;
    menuHelp->addAction(&aboutProgram);

    // Other
    const auto tableWidget = this->ui.tableWidget;

    this->tableOriginalWidth = tableWidget->width();
    this->tableOriginalHeight = tableWidget->height();
    this->windowOriginalWidth = this->width();
    this->windowOriginalHeight = this->height();
}

MainWindow::~MainWindow() {
    delete &this->ui;
}

void MainWindow::on_tableWidget_cellActivated(int, int) {

}

void MainWindow::on_tableWidget_cellChanged(int row, int column) {
    const auto tableWidget = this->ui.tableWidget;

    if (row == tableWidget->rowCount() - 1) {
        if (tableWidget->item(row, 0)) {
            this->createRow(tableWidget->rowCount());
        }
    }

    if (column == 0) {
        const auto currentItem = tableWidget->item(row, 0);

        if (currentItem) {
            const auto value = currentItem->text();

            if (!value.isNull() && !value.isEmpty()) {
                this->updateRow(row, value, false);
            }
        }
    }
}

void MainWindow::on_tableWidget_cellClicked(int, int) {

}

void MainWindow::on_tableWidget_cellEntered(int, int) {

}

void MainWindow::on_tableWidget_cellPressed(int, int) {

}

void MainWindow::resizeEvent(QResizeEvent *) {
    const auto tableWidget = this->ui.tableWidget;
    tableWidget->setFixedWidth(this->tableOriginalWidth + (this->width() - this->windowOriginalWidth));
    tableWidget->setFixedHeight(this->tableOriginalHeight + (this->height() - this->windowOriginalHeight));
}

void MainWindow::wheelEvent(QWheelEvent *event) {
    if (event->modifiers() & Qt::ControlModifier) {
        const auto y = event->angleDelta().y();

        if (y > 0) {
            this->zoomIn();
        } else if (y < 0) {
            this->zoomOut();
        }
    }
}

void MainWindow::clearTable() {
    const auto tableWidget = this->ui.tableWidget;

    while (tableWidget->columnCount() > 0) {
        tableWidget->removeColumn(0);
    }
    while (tableWidget->rowCount() > 0) {
        this->deleteRow(0);
    }

    tableWidget->insertColumn(tableWidget->columnCount());
    tableWidget->insertColumn(tableWidget->columnCount());
    tableWidget->setColumnWidth(1, this->pictureSize);

    const auto horizontalHeader = tableWidget->horizontalHeader();
    horizontalHeader->setSectionResizeMode(0, QHeaderView::Stretch);

    this->createRow(tableWidget->rowCount());
}

void MainWindow::about() {
    QMessageBox::information(
        this,
        tr("About the program — Qrlot"),
        "<a>Author: </a><a href=\"https://github.com/stanislav539261\">https://github.com/stanislav539261</a><br></br><a>License: GNU General Public License Version 3</a><br></br><a>Version: 0.1</a>",
        QMessageBox::Close
    );
}

void MainWindow::copyToClipboard() {
    const auto clipboard = QGuiApplication::clipboard();
    const auto tableWidget = this->ui.tableWidget;
    const auto model = tableWidget->selectionModel();
    const auto selections = model->selectedRows();

    auto text = QString();

    for (const auto &selection : selections) {
        const auto row = selection.row();
        const auto &value = g_State->currentValues.at(row);

        if (!value.isNull() && !value.isEmpty()) {
            text += value;
            text += "\n";
        }
    }

    clipboard->setText(text);
}

void MainWindow::cutToClipboard() {
    this->copyToClipboard();
    this->deleteSelectedRows();
}

void MainWindow::deleteSelectedRows() {
    const auto tableWidget = this->ui.tableWidget;
    const auto model = tableWidget->selectionModel();
    const auto selections = model->selectedRows();

    for (auto i = 0; i < selections.length(); i++) {
        this->deleteRow(selections.first().row());
    }

    if (tableWidget->rowCount() == 0) {
        this->createRow(0);
    } else {
        const auto item = tableWidget->item(tableWidget->rowCount() - 1, 0);

        if (item) {
            const auto text = item->text();

            if (!text.isNull() && !text.isEmpty()) {
                this->createRow(tableWidget->rowCount());
            }
        }
    }
}

void MainWindow::exportAsPictures() {
    auto &dialog = this->exportPicturesDialog;

    dialog.show();
}

void MainWindow::exportAsSpreadsheet() {
    auto &dialog = this->exportSpreadsheetDialog;

    dialog.show();
}

void MainWindow::newTable() {
    g_State->lastSaveTableFullPath = QString();

    this->clearTable();
}

void MainWindow::openTable() {
    auto &values = g_State->currentValues;

    auto fullPath = QFileDialog::getOpenFileName(
        this,
        tr("Open file — Qrlot"),
        g_State->lastOpenTableFullPath,
        "XML files (*.xml)"
    );

    auto file = QFile(fullPath);

    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        g_State->lastOpenTableFullPath = fullPath;
        g_State->lastSaveTableFullPath = fullPath;

        auto reader = QXmlStreamReader(&file);

        this->clearTable();

        for (auto i = 0; !reader.atEnd(); reader.readNext()) {
            if (reader.isStartElement() && reader.name() == QString("Row")) {
                values.push_back(reader.readElementText());
                this->updateRow(i, values.last(), false);
                i++;
            }
        }
    }
}

void MainWindow::pasteFromClipboard() {
    const auto clipboard = QGuiApplication::clipboard();
    const auto tableWidget = this->ui.tableWidget;
    const auto model = tableWidget->selectionModel();
    const auto selections = model->selectedRows();
    const auto indents = clipboard->text().split("\n");

    auto currentIndent = indents.begin();
    auto currentSelection = selections.begin();

    if (selections.length() == 0) {
        return;
    }

    while (currentIndent != indents.end() && currentSelection != selections.end()) {
        if (!currentIndent->isNull() && !currentIndent->isEmpty()) {
            const auto currentRow = currentSelection->row();

            this->updateRow(currentRow, *currentIndent, false);
        }

        currentIndent++;
        currentSelection++;
    }

    auto currentRow = selections.back().row() + 1;

    while (currentIndent != indents.end()) {
        if (!currentIndent->isNull() && !currentIndent->isEmpty()) {
            if (!tableWidget->item(currentRow, 0)) {
                this->createRow(currentRow);
            }

            this->updateRow(currentRow, *currentIndent, false);
        }

        currentIndent++;
        currentRow++;
    }
}

void MainWindow::saveTable() {
    auto fullPath = g_State->lastSaveTableFullPath;

    if (fullPath.isNull() || fullPath.isEmpty()) {
        fullPath = QFileDialog::getSaveFileName(
            this,
            tr("Save file — Qrlot"),
            g_State->lastOpenTableFullPath,
            "XML files (*.xml)"
        );

        g_State->lastSaveTableFullPath = fullPath;
    }

    if (!fullPath.isNull() && !fullPath.isEmpty()) {
        const auto &values = g_State->currentValues;

        auto file = QFile(fullPath);

        if (file.open(QIODevice::ReadWrite | QIODevice::Text | QIODevice::Truncate)) {
            g_State->lastSaveTableFullPath = fullPath;

            auto writer = QXmlStreamWriter(&file);
            writer.setAutoFormatting(true);
            writer.setAutoFormattingIndent(true);
            writer.writeStartDocument();
            writer.writeStartElement("Table");

            for (const QString &value : values) {
                if (!value.isNull() && !value.isEmpty()) {
                    writer.writeStartElement("Row");
                    writer.writeCharacters(value);
                    writer.writeEndElement();
                }
            }

            writer.writeEndElement();
            writer.writeEndDocument();
        }
    }
}

void MainWindow::saveTableAs() {
    const auto fullPath = QFileDialog::getSaveFileName(
        this,
        tr("Save file — Qrlot"),
        g_State->lastOpenTableFullPath,
        "XML files (*.xml)"
    );

    if (!fullPath.isNull() && !fullPath.isEmpty()) {
        g_State->lastSaveTableFullPath = fullPath;
        this->saveTable();
    }
}

void MainWindow::zoomIn() {
    const auto tableWidget = this->ui.tableWidget;
    const auto &values = g_State->currentValues;

    this->pictureSize += this->pictureSize < 400 ? 10 : 0;

    tableWidget->setColumnWidth(1, this->pictureSize);

    auto i = 0;

    for (const auto &item : values) {
        tableWidget->setRowHeight(i, this->pictureSize);
        this->updateRow(i, item, true);
        i++;
    }
}

void MainWindow::zoomOut() {
    const auto tableWidget = this->ui.tableWidget;
    const auto &values = g_State->currentValues;

    this->pictureSize -= this->pictureSize > 100 ? 10 : 0;

    tableWidget->setColumnWidth(1, this->pictureSize);

    auto i = 0;

    for (const auto &item : values) {
        tableWidget->setRowHeight(i, this->pictureSize);
        this->updateRow(i, item, true);
        i++;
    }
}

void MainWindow::createRow(int row) {
    g_State->currentValues.insert(row, QString());

    const auto tableWidget = this->ui.tableWidget;
    tableWidget->insertRow(row);
    tableWidget->setRowHeight(row, this->pictureSize);

    // Create read only cell for picture
    auto &secondColumnItem = *new QTableWidgetItem();
    secondColumnItem.setFlags(Qt::ItemIsEnabled);

    tableWidget->setItem(row, 1, &secondColumnItem);
}

void MainWindow::deleteRow(int row) {
    g_State->currentValues.removeAt(row);

    const auto tableWidget = this->ui.tableWidget;
    tableWidget->removeRow(row);
}

void MainWindow::updateRow(int row, const QString &value, bool forceUpdate) {
    const auto tableWidget = this->ui.tableWidget;

    if (forceUpdate || value != g_State->currentValues.at(row)) {
        g_State->currentValues.replace(row, value);

        const auto image = QZXing::encodeData(
            value.toUtf8(),
            QZXing::EncoderFormat_QR_CODE,
            QSize(this->pictureSize, this->pictureSize),
            QZXing::EncodeErrorCorrectionLevel_L,
            true
        );

        if (!value.isNull() && !value.isEmpty()) {
            auto &firstColumnItem = *new QTableWidgetItem();
            firstColumnItem.setText(value);

            auto &secondColumnItem = *new QTableWidgetItem();
            secondColumnItem.setData(Qt::BackgroundRole, QPixmap::fromImage(image));
            secondColumnItem.setFlags(Qt::ItemIsEnabled);

            tableWidget->setItem(row, 0, &firstColumnItem);
            tableWidget->setItem(row, 1, &secondColumnItem);
        } else {
            tableWidget->setItem(row, 0, nullptr);
            tableWidget->setItem(row, 1, nullptr);
        }
    }
}

