/**
 * @file Settings.h
 *
 * @author Dan Keenan
 * @date 5/11/2025
 * @copyright GNU GPLv3
 */

#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>
#include <QStandardPaths>

#define DO_SETTING(T, name, defaultValue)                                                                              \
    [[nodiscard]] inline T get##name() { return settings::detail::getSetting<T>(#name, defaultValue); }                \
    inline void set##name(T value) { settings::detail::setSetting(#name, std::forward<T>(value)); }                    \
    inline T default##name() { return defaultValue; }

namespace echoblind::settings
{
    namespace detail
    {
        template <typename T>
        T getSetting(const char* name, T defaultValue)
        {
            QSettings settings;
            return settings.value(name, std::forward<T>(defaultValue)).template value<T>();
        }
        template <typename T>
        void setSetting(const char* name, T value)
        {
            QSettings settings;
            settings.setValue(name, std::forward<T>(value));
        }

    } // namespace detail

    DO_SETTING(QByteArray, MainWindowGeometry, {});
    DO_SETTING(QString, LastFileDialogPath, QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));

} // namespace echoblind::settings

#endif // SETTINGS_H
