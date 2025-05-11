/**
 * @file MainWindow.h
 *
 * @author Dan Keenan
 * @date 5/11/2025
 * @copyright GNU GPLv3
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QPushButton>

#include "FileSelectorWidget.h"

namespace echoblind
{

    class MainWindow : public QMainWindow
    {
        Q_OBJECT
    public:
        explicit MainWindow(QWidget* parent = nullptr);

    private:
        struct Widgets
        {
            FileSelectorWidget* baseCfgPath = nullptr;
            QLabel* rackTypeLabel = nullptr;
            QLabel* rackNameLabel = nullptr;
            FileSelectorWidget* sheetLine = nullptr;
            FileSelectorWidget* outCfgLine = nullptr;
            QPushButton* saveSheetButton = nullptr;
            QPushButton* loadSheetButton = nullptr;
        };
        Widgets widgets_;

        void initUi();
    };

} // namespace echoblind

#endif // MAINWINDOW_H
