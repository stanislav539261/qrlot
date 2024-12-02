#ifndef WINDOW_H
#define WINDOW_H

#include <QMainWindow>

#include "dialogs.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_tableWidget_cellChanged(int row, int column);
    void on_tableWidget_cellPressed(int row, int column);
    void on_tableWidget_cellActivated(int row, int column);
    void on_tableWidget_cellEntered(int row, int column);
    void on_tableWidget_cellClicked(int row, int column);

private:
    ExportPicturesDialog &exportPicturesDialog;
    ExportSpreadsheetDialog &exportSpreadsheetDialog;
    Ui::MainWindow &ui;

    int pictureSize;
    int tableOriginalWidth;
    int tableOriginalHeight;
    int windowOriginalWidth;
    int windowOriginalHeight;

    void resizeEvent(QResizeEvent *);
    void wheelEvent(QWheelEvent *event);

    void about();
    void copyToClipboard();
    void cutToClipboard();
    void clearTable();
    void deleteSelectedRows();
    void exportAsPictures();
    void exportAsSpreadsheet();
    void newTable();
    void openTable();
    void pasteFromClipboard();
    void saveTable();
    void saveTableAs();
    void zoomIn();
    void zoomOut();

    void createRow(int row);
    void deleteRow(int row);
    void updateRow(int row, const QString &value, bool forceUpdate);
    // void generateQrCode(int row);
};
#endif // WINDOW_H
