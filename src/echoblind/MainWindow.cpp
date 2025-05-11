/**
 * @file MainWindow.cpp
 *
 * @author Dan Keenan
 * @date 5/11/2025
 * @copyright GNU GPLv3
 */

#include "MainWindow.h"
#include <QHBoxLayout>
#include <QVBoxLayout>

#include "Settings.h"

namespace echoblind
{

    MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) { initUi(); }

    void MainWindow::initUi()
    {
        if (!restoreGeometry(settings::getMainWindowGeometry()))
        {
            resize(800, 600);
        }

        auto* central = new QWidget(this);
        setCentralWidget(central);
        auto* layout = new QVBoxLayout(central);

        // Base config.
        auto* baseCfgLabel = new QLabel(tr("Base Config:"), central);
        layout->addWidget(baseCfgLabel);
        widgets_.baseCfgPath = new FileSelectorWidget(central);
        layout->addWidget(widgets_.baseCfgPath);

        // Spreadsheet.
        auto* sheetLabel = new QLabel(tr("Spreadsheet:"), central);
        layout->addWidget(sheetLabel);
        widgets_.sheetLine = new FileSelectorWidget(central);
        layout->addWidget(widgets_.sheetLine);

        // Out config.
        auto outLabel = new QLabel(tr("Output File:"), central);
        layout->addWidget(outLabel);
        widgets_.outCfgLine = new FileSelectorWidget(central);
        layout->addWidget(widgets_.outCfgLine);

        // -----
        layout->addStretch();

        // Action buttons.
        auto* buttonsLayout = new QHBoxLayout();
        layout->addLayout(buttonsLayout);
        widgets_.saveSheetButton = new QPushButton(tr("Save Sheet"), central);
        buttonsLayout->addWidget(widgets_.saveSheetButton);
        widgets_.loadSheetButton = new QPushButton(tr("Update Config From Sheet"), central);
        buttonsLayout->addWidget(widgets_.loadSheetButton);
    }

} // namespace echoblind
