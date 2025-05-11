/**
 * @file main.cpp
 *
 * @author Dan Keenan
 * @date 5/11/2025
 * @copyright GNU GPLv3
 */

#include <QApplication>
#include <QStyleFactory>

#include "MainWindow.h"
#include "echoblind_config.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    app.setOrganizationName(echoblind::config::kProjectOrganizationName);
    app.setOrganizationDomain(echoblind::config::kProjectOrganizationDomain);
    app.setApplicationName(echoblind::config::kProjectName);
    app.setApplicationDisplayName(echoblind::config::kProjectDisplayName);
    app.setApplicationVersion(echoblind::config::kProjectVersion);

#ifdef PLATFORM_WINDOWS
    // Using fusion style enables dark-mode detection on Windows.
    auto* style = QStyleFactory::create("fusion");
    if (style != nullptr)
    {
        app.setStyle(style);
    }
#endif

    echoblind::MainWindow mainWindow;
    mainWindow.show();

    return app.exec();
}
