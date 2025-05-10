/**
 * @file Config.h
 *
 * @author Dan Keenan
 * @date 5/10/2025
 * @copyright GNU GPLv3
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <QString>
#include <map>
#include "Circuit.h"
#include "Preset.h"

namespace echoconfig
{
    /**
     * Base class for panel configurations.
     */
    class Config
    {
    public:
        virtual ~Config() = default;

        [[nodiscard]] virtual QString configType() const = 0;

        /**
         * Parse a panel configuration file.
         * @param path Path to config file.
         */
        virtual void parseCfg(const QString& path) = 0;

        /**
         * Parse a spreadsheet file.
         * @param path Path to spreadsheet file.
         */
        void parseSheet(const QString& path);

        /**
         * Save to a new configuration file.
         * @param basePath Path to original config file.
         * @param outPath Path to new config file.
         */
        virtual void saveCfg(const QString& basePath, const QString& outPath) const = 0;

        /**
         * Save to spreadsheet file.
         * @param path Path to spreadsheet file.
         */
        void saveSheet(const QString& path) const;

    protected:
        std::map<unsigned int, Circuit> circuits_;
        std::map<unsigned int, Preset> presets_;
    };
} // namespace echoconfig

#endif // CONFIG_H
