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
#include <QMainWindow>
#include <QPushButton>
#include "FileSelectorWidget.h"
#include "echoconfig/Config.h"

namespace echoblind
{

    class MainWindow : public QMainWindow
    {
        Q_OBJECT
    public:
        explicit MainWindow(QWidget* parent = nullptr);

    protected:
        void closeEvent(QCloseEvent* event) override;

    private:
        struct Widgets
        {
            FileSelectorWidget* baseCfgPath = nullptr;
            QLabel* rackTypeLabel = nullptr;
            QLabel* rackNameLabel = nullptr;
            FileSelectorWidget* inSheetPath = nullptr;
            FileSelectorWidget* outSheetPath = nullptr;
            FileSelectorWidget* outCfgPath = nullptr;
            QPushButton* saveSheetButton = nullptr;
            QPushButton* saveCfgButton = nullptr;
        };
        Widgets widgets_;
        echoconfig::Config* config_ = nullptr;

        void initUi();
        void updateAllowedActions();

    private Q_SLOTS:
        void baseCfgChanged(const QString& path);
        void outSheetChanged(const QString& path);
        void inSheetChanged(const QString& path);
        void outCfgChanged(const QString& path);
        void saveSheet();
        void saveCfg();
    };

} // namespace echoblind

#endif // MAINWINDOW_H
