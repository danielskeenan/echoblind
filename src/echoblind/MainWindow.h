/**
 * @file MainWindow.h
 *
 * @author Dan Keenan
 * @date 5/11/2025
 * @copyright GNU GPLv3
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

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
        };
        Widgets widgets_;
    };

} // namespace echoblind

#endif // MAINWINDOW_H
