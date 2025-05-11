/**
 * @file MainWindow.cpp
 *
 * @author Dan Keenan
 * @date 5/11/2025
 * @copyright GNU GPLv3
 */

#include "MainWindow.h"
#include <QHBoxLayout>
#include <QMessageBox>
#include <QTabWidget>
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
        widgets_.baseCfgPath->setAcceptMode(QFileDialog::AcceptOpen);
        widgets_.baseCfgPath->setFileMode(QFileDialog::FileMode::ExistingFile);
        widgets_.baseCfgPath->setNameFilters({tr("Echo PCP files (*.cfg)"), tr("Echo ACP files (*.eacp)")});
        connect(widgets_.baseCfgPath, &FileSelectorWidget::pathChanged, this, &MainWindow::baseCfgChanged);
        layout->addWidget(widgets_.baseCfgPath);

        // Config info.
        auto* infoLayout = new QHBoxLayout();
        layout->addLayout(infoLayout);
        infoLayout->addStretch();
        widgets_.rackTypeLabel = new QLabel(central);
        infoLayout->addWidget(widgets_.rackTypeLabel);
        infoLayout->addStretch();
        widgets_.rackNameLabel = new QLabel(central);
        infoLayout->addWidget(widgets_.rackNameLabel);
        infoLayout->addStretch();

        // Tabs.
        QTabWidget* tabs = new QTabWidget(central);
        layout->addWidget(tabs);

        // Save Sheet tab.
        auto* saveSheetTab = new QWidget;
        tabs->addTab(saveSheetTab, tr("Save Sheet"));
        auto* saveSheetLayout = new QVBoxLayout(saveSheetTab);

        // Out Spreadsheet.
        auto* outSheetLabel = new QLabel(tr("Spreadsheet:"), saveSheetTab);
        saveSheetLayout->addWidget(outSheetLabel);
        widgets_.outSheetPath = new FileSelectorWidget(saveSheetTab);
        widgets_.outSheetPath->setAcceptMode(QFileDialog::AcceptSave);
        widgets_.outSheetPath->setFileMode(QFileDialog::FileMode::AnyFile);
        widgets_.outSheetPath->setMimeTypeFilters(
            {"application/vnd.openxmlformats-officedocument.spreadsheetml.sheet"});
        connect(widgets_.outSheetPath, &FileSelectorWidget::pathChanged, this, &MainWindow::outSheetChanged);
        saveSheetLayout->addWidget(widgets_.outSheetPath);

        // -----
        saveSheetLayout->addStretch();

        // Save Sheet button.
        widgets_.saveSheetButton = new QPushButton(tr("Save Sheet"), central);
        connect(widgets_.saveSheetButton, &QPushButton::clicked, this, &MainWindow::saveSheet);
        saveSheetLayout->addWidget(widgets_.saveSheetButton);

        // Save Config tab.
        auto* saveCfgTab = new QWidget;
        tabs->addTab(saveCfgTab, tr("Save Config"));
        auto* saveCfgLayout = new QVBoxLayout(saveCfgTab);

        // In Spreadsheet.
        auto* inSheetLabel = new QLabel(tr("Spreadsheet:"), saveCfgTab);
        saveCfgLayout->addWidget(inSheetLabel);
        widgets_.inSheetPath = new FileSelectorWidget(saveCfgTab);
        widgets_.inSheetPath->setAcceptMode(QFileDialog::AcceptOpen);
        widgets_.inSheetPath->setFileMode(QFileDialog::FileMode::ExistingFile);
        widgets_.inSheetPath->setMimeTypeFilters({"application/vnd.openxmlformats-officedocument.spreadsheetml.sheet"});
        connect(widgets_.inSheetPath, &FileSelectorWidget::pathChanged, this, &MainWindow::inSheetChanged);
        saveCfgLayout->addWidget(widgets_.inSheetPath);

        // Out config.
        auto outLabel = new QLabel(tr("Output File:"), saveCfgTab);
        saveCfgLayout->addWidget(outLabel);
        widgets_.outCfgPath = new FileSelectorWidget(saveCfgTab);
        widgets_.outCfgPath->setAcceptMode(QFileDialog::AcceptSave);
        widgets_.outCfgPath->setFileMode(QFileDialog::FileMode::AnyFile);
        widgets_.outCfgPath->setNameFilters({tr("Echo PCP files (*.cfg)"), tr("Echo ACP files (*.eacp)")});
        connect(widgets_.outCfgPath, &FileSelectorWidget::pathChanged, this, &MainWindow::outCfgChanged);
        saveCfgLayout->addWidget(widgets_.outCfgPath);

        // -----
        saveCfgLayout->addStretch();

        // Save Config button.
        widgets_.saveCfgButton = new QPushButton(tr("Save Updated Config"), central);
        connect(widgets_.saveCfgButton, &QPushButton::clicked, this, &MainWindow::saveCfg);
        saveCfgLayout->addWidget(widgets_.saveCfgButton);

        updateAllowedActions();
    }

    void MainWindow::closeEvent(QCloseEvent* event)
    {
        settings::setMainWindowGeometry(saveGeometry());
        QMainWindow::closeEvent(event);
    }

    void MainWindow::updateAllowedActions()
    {
        widgets_.saveSheetButton->setEnabled(config_ != nullptr && !widgets_.outSheetPath->path().isEmpty());
        widgets_.saveCfgButton->setEnabled(config_ != nullptr && !widgets_.inSheetPath->path().isEmpty() &&
                                           config_->isSheetParsed() && !widgets_.outCfgPath->path().isEmpty());
    }

    void MainWindow::baseCfgChanged(const QString& path)
    {
        echoconfig::Config* newConfig = nullptr;
        try
        {
            newConfig = echoconfig::Config::loadCfg(path, this);
        }
        catch (const std::exception&)
        {
            newConfig = nullptr;
        }
        if (config_ != nullptr)
        {
            config_->deleteLater();
        }
        if (newConfig != nullptr)
        {
            widgets_.rackTypeLabel->setText(tr("Type: %1").arg(newConfig->panelType()));
            widgets_.rackNameLabel->setText(tr("Name: %1").arg(newConfig->panelName()));
        }
        else
        {
            widgets_.rackTypeLabel->clear();
            widgets_.rackNameLabel->clear();
            QMessageBox::warning(this, tr("Invalid config"), tr("The config file could not be loaded or is invalid."));
        }
        config_ = newConfig;
        updateAllowedActions();
    }

    void MainWindow::outSheetChanged(const QString& path) { updateAllowedActions(); }

    void MainWindow::inSheetChanged(const QString& path)
    {
        try
        {
            config_->parseSheet(path);
        }
        catch (const std::exception&)
        {
            QMessageBox::warning(this, tr("Invalid sheet"), tr("The sheet could not be loaded or is invalid."));
        }
        updateAllowedActions();
    }

    void MainWindow::outCfgChanged(const QString&) { updateAllowedActions(); }

    void MainWindow::saveSheet()
    {
        if (config_ == nullptr || widgets_.outSheetPath->path().isEmpty())
        {
            QMessageBox::warning(this, tr("Not ready"), tr("Base config and sheet paths must be set before saving."));
            return;
        }

        try
        {
            config_->saveSheet(widgets_.outSheetPath->path());
        }
        catch (const std::exception&)
        {
            QMessageBox::critical(this, tr("Save error"), tr("An error occurred while saving the sheet."));
        }
    }

    void MainWindow::saveCfg()
    {
        if (config_ == nullptr || widgets_.baseCfgPath->path().isEmpty() || widgets_.inSheetPath->path().isEmpty() ||
            !config_->isSheetParsed() || widgets_.inSheetPath->path().isEmpty())
        {
            QMessageBox::warning(this, tr("Not ready"), tr("All paths must be set before saving."));
            return;
        }

        try
        {
            config_->saveCfg(widgets_.baseCfgPath->path(), widgets_.outCfgPath->path());
        }
        catch (const std::exception&)
        {
            QMessageBox::critical(this, tr("Save error"), tr("An error occurred while saving the config."));
        }
    }

} // namespace echoblind
