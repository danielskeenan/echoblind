/**
 * @file Config.cpp
 *
 * @author Dan Keenan
 * @date 5/10/2025
 * @copyright GNU GPLv3
 */

#include "echoconfig/Config.h"
#include <QFile>
#include <QRegularExpression>
#include <QSaveFile>
#include <optional>
#include <regex>
#include <xlsxdocument.h>
#include <xlsxworkbook.h>
#include "echoconfig/sheet_helpers.h"

namespace echoconfig
{
    static const auto kRePreset = QRegularExpression(R"(^Preset (\d+)$)");

    void Config::parseSheet(const QString& path)
    {
        QFile f(path);
        if (!f.open(QIODevice::ReadOnly))
        {
            throw std::runtime_error("Failed to open file");
        }
        QXlsx::Document doc(&f);

        // Levels sheet
        if (!doc.selectSheet(tr("Levels")))
        {
            throw std::runtime_error("Missing \"Levels\" sheet.");
        }
        openSheetLevels(&doc);
        if (!doc.selectSheet(tr("Times")))
        {
            throw std::runtime_error("Missing \"Times\" sheet.");
        }
        openSheetTimes(&doc);
    }

    void Config::saveSheet(const QString& path) const
    {
        QXlsx::Document doc;
        auto book = doc.workbook();

        // Levels sheet.
        book->renameSheet(kSheetIxLevels, tr("Levels"));
        book->setActiveSheet(kSheetIxLevels);
        saveSheetLevels(&doc);

        // Times sheet.
        book->addSheet(tr("Times"));
        book->setActiveSheet(kSheetIxTimes);
        saveSheetTimes(&doc);

        // Save.
        QSaveFile f(path);
        f.open(QIODevice::WriteOnly);
        auto success = doc.saveAs(&f);
        if (success)
        {
            success = f.commit();
        }
        if (!success)
        {
            throw std::runtime_error("Error saving config");
        }
    }

    void Config::openSheetLevels(const QXlsx::Document* doc)
    {
        std::optional<int> colCircuit;
        std::optional<int> colSpace;
        std::optional<int> colZone;
        std::map<unsigned int, int> colPresets;

        // Columns
        for (int colIx = 0; colIx < doc->dimension().columnCount(); ++colIx)
        {
            const auto val = doc->read(0, colIx);
            if (!val.isValid() || val.toString().isEmpty())
            {
                break;
            }
            else if (val == tr("Circuit"))
            {
                colCircuit = colIx;
            }
            else if (val == tr("Space"))
            {
                colSpace = colIx;
            }
            else if (val == tr("Zone"))
            {
                colZone = colIx;
            }
            else
            {
                auto match = kRePreset.match(val.toString());
                if (match.hasMatch())
                {
                    const auto presetNum = match.captured(1).toUInt();
                    colPresets.insert_or_assign(presetNum, colIx);
                }
            }
        }
        if (!colCircuit.has_value())
        {
            throw std::runtime_error("Missing circuit column.");
        }
        else if (!colSpace.has_value())
        {
            throw std::runtime_error("Missing space column.");
        }
        else if (!colZone.has_value())
        {
            throw std::runtime_error("Missing zone column.");
        }
        else if (colPresets.empty())
        {
            throw std::runtime_error("Missing preset column.");
        }

        // Data.
        for (int rowIx = 0; rowIx < doc->dimension().rowCount(); ++rowIx)
        {
            const auto circuitNum = sheet_helpers::requiredCellUInt(doc, rowIx, colCircuit.value());
            const auto spaceNum = sheet_helpers::requiredCellUInt(doc, rowIx, colSpace.value());
            const auto zoneNum = sheet_helpers::requiredCellUInt(doc, rowIx, colZone.value());

            auto& circuit = getCircuit(circuitNum);
            circuit.num = circuitNum;
            circuit.space = spaceNum;
            circuit.zone = zoneNum;

            for (const auto [presetNum, colPreset] : colPresets)
            {
                const auto level = sheet_helpers::requiredCellUInt(doc, rowIx, colPreset);
                if (level > 255)
                {
                    throw std::runtime_error("Bad level.");
                }
                auto& preset = getPreset(presetNum);
                preset.num = presetNum;
                preset.levels[circuitNum] = level;
            }
        }
    }

    void Config::saveSheetLevels(QXlsx::Document* doc) const
    {
        constexpr auto kColCircuit = 0;
        constexpr auto kColSpace = 1;
        constexpr auto kColZone = 2;
        constexpr auto kColPreset = 3;

        // Header.
        doc->write(0, kColCircuit, tr("Circuit"));
        doc->write(0, kColSpace, tr("Space"));
        doc->write(0, kColZone, tr("Zone"));
        for (unsigned int presetIx = 0; presetIx < presetCount(); ++presetIx)
        {
            const auto& preset = getPresetAt(presetIx);
            doc->write(0, kColPreset + preset.num - 1, tr("Preset %1").arg(preset.num));
        }

        // Values.
        for (unsigned int circuitIx = 0; circuitIx < circuitCount(); ++circuitIx)
        {
            const auto& circuit = getCircuitAt(circuitIx);
            const auto row = circuitIx + 1;
            doc->write(row, kColCircuit, circuit.num);
            doc->write(row, kColSpace, circuit.space);
            doc->write(row, kColZone, circuit.zone);
            for (unsigned int presetIx = 0; presetIx < presetCount(); ++presetIx)
            {
                const auto& preset = getPresetAt(presetIx);
                doc->write(row, kColPreset + preset.num - 1, preset.levels.at(circuit.num));
            }
        }
    }

    void Config::saveSheetTimes(QXlsx::Document* doc) const
    {
        constexpr auto kColSpace = 0;
        constexpr auto kColPreset = 1;

        // Header.
        doc->write(0, kColSpace, tr("Space"));
        for (unsigned int presetIx = 0; presetIx < presetCount(); ++presetIx)
        {
            const auto& preset = getPresetAt(presetIx);
            doc->write(0, kColPreset + preset.num - 1, tr("Preset %1").arg(preset.num));
        }

        // Values.
        for (unsigned int spaceIx = 0; spaceIx < spaceCount(); ++spaceIx)
        {
            const auto& space = getSpaceAt(spaceIx);
            const auto row = spaceIx + 1;
            doc->write(row, kColSpace, space.num);
            for (unsigned int presetIx = 0; presetIx < presetCount(); ++presetIx)
            {
                const auto& preset = getPresetAt(presetIx);
                doc->write(0, kColPreset + preset.num - 1, preset.fadeTimes.at(space.num));
            }
        }
    }

    void Config::openSheetTimes(const QXlsx::Document* doc)
    {
        std::optional<int> colSpace;
        std::map<unsigned int, int> colPresets;

        // Columns
        for (int colIx = 0; colIx < doc->dimension().columnCount(); ++colIx)
        {
            const auto val = doc->read(0, colIx);
            if (!val.isValid() || val.toString().isEmpty())
            {
                break;
            }
            else if (val == tr("Space"))
            {
                colSpace = colIx;
            }
            else
            {
                auto match = kRePreset.match(val.toString());
                if (match.hasMatch())
                {
                    const auto presetNum = match.captured(1).toUInt();
                    colPresets.insert_or_assign(presetNum, colIx);
                }
            }
        }
        if (!colSpace.has_value())
        {
            throw std::runtime_error("Missing space column.");
        }
        else if (colPresets.empty())
        {
            throw std::runtime_error("Missing preset column.");
        }

        // Data.
        for (int rowIx = 0; rowIx < doc->dimension().rowCount(); ++rowIx)
        {
            const auto spaceNum = sheet_helpers::requiredCellUInt(doc, rowIx, colSpace.value());

            for (const auto [presetNum, colPreset] : colPresets)
            {
                const auto uptime = sheet_helpers::requiredCellUInt(doc, rowIx, colPreset);
                auto& preset = getPreset(presetNum);
                preset.num = presetNum;
                preset.fadeTimes[spaceNum] = uptime;
            }
        }
    }
} // namespace echoconfig
