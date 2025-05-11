/**
 * @file Config.h
 *
 * @author Dan Keenan
 * @date 5/10/2025
 * @copyright GNU GPLv3
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <QObject>
#include <QString>
#include "Circuit.h"
#include "Preset.h"
#include "Space.h"

// Forward-declare Xlsx document so we don't force a downstream dependency.
namespace QXlsx
{
    class Document;
}

namespace echoconfig
{
    /**
     * Base class for panel configurations.
     */
    class Config : public QObject
    {
        Q_OBJECT
    public:
        using QObject::QObject;

        /**
         * Load a config file of unknown type.
         *
         * If the file could not be loaded, returns nullptr.
         *
         * @return
         */
        [[nodiscard]] static Config* loadCfg(const QString& path);

        [[nodiscard]] virtual QString panelType() const = 0;

        [[nodiscard]] virtual QString panelName() const = 0;

        /**
         * Parse a panel configuration file.
         * @param path Path to config file.
         * @throws std::runtime_error if the config cannot be parsed.
         */
        virtual void parseCfg(const QString& path) = 0;

        /**
         * Parse a spreadsheet file.
         * @param path Path to spreadsheet file.
         * @throws std::runtime_error if the sheet cannot be parsed.
         */
        virtual void parseSheet(const QString& path);

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
        virtual void saveSheet(const QString& path) const;

        [[nodiscard]] virtual unsigned int circuitCount() const = 0;
        [[nodiscard]] virtual const Circuit& getCircuitAt(unsigned int ix) const = 0;
        [[nodiscard]] virtual Circuit& getCircuitAt(unsigned int ix) = 0;
        [[nodiscard]] virtual const Circuit& getCircuit(unsigned int num) const = 0;
        [[nodiscard]] virtual Circuit& getCircuit(unsigned int num) = 0;

        [[nodiscard]] virtual unsigned int spaceCount() const = 0;
        [[nodiscard]] virtual const Space& getSpaceAt(unsigned int ix) const = 0;
        [[nodiscard]] virtual Space& getSpaceAt(unsigned int ix) = 0;
        [[nodiscard]] virtual const Space& getSpace(unsigned int num) const = 0;
        [[nodiscard]] virtual Space& getSpace(unsigned int num) = 0;

        [[nodiscard]] virtual unsigned int presetCount() const = 0;
        [[nodiscard]] virtual const Preset& getPresetAt(unsigned int ix) const = 0;
        [[nodiscard]] virtual Preset& getPresetAt(unsigned int ix) = 0;
        [[nodiscard]] virtual const Preset& getPreset(unsigned int num) const = 0;
        [[nodiscard]] virtual Preset& getPreset(unsigned int num) = 0;

    private:
        static constexpr auto kSheetIxLevels = 0;
        static constexpr auto kSheetIxTimes = 1;

        void openSheetLevels(const QXlsx::Document* doc);
        void saveSheetLevels(QXlsx::Document* doc) const;
        void openSheetTimes(const QXlsx::Document* doc);
        void saveSheetTimes(QXlsx::Document* doc) const;
    };

    template <class C>
    concept ConfigClass = std::derived_from<C, Config>;

    /**
     * Generic class to simplify specifying config loaders.
     * @tparam C A derivative of echoconfig::Config.
     */
    template <ConfigClass C>
    struct ConfigLoader
    {
        Config* operator()(const QString& path) const
        {
            C* cfg = new C();
            cfg->parseCfg(path);
            return cfg;
        }
    };
} // namespace echoconfig

#endif // CONFIG_H
