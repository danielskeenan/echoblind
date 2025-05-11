/**
 * @file EchoPcpConfigTest.cpp
 *
 * @author Dan Keenan
 * @date 5/10/2025
 * @copyright GNU GPLv3
 */

#include <QDomDocument>
#include <QTemporaryDir>
#include <catch2/catch_test_macros.hpp>
#include <ranges>
#include "XlsxMatcher.h"
#include "echoconfig/EchoPcpConfig.h"
#include "qstring_tostring.h"

using namespace echoconfig;

decltype(Preset::levels) genLevelsMap(unsigned int level)
{
    decltype(Preset::levels) levels;
    for (unsigned int circuitNum = 1; circuitNum <= 48; ++circuitNum)
    {
        levels.emplace(circuitNum, level);
    }
    return levels;
}

static const std::vector<Circuit> kExpectedCircuits = {
    Circuit{.num = 1, .space = 1, .zone = 1},   Circuit{.num = 2, .space = 1, .zone = 2},
    Circuit{.num = 3, .space = 1, .zone = 3},   Circuit{.num = 4, .space = 1, .zone = 4},
    Circuit{.num = 5, .space = 1, .zone = 5},   Circuit{.num = 6, .space = 1, .zone = 6},
    Circuit{.num = 7, .space = 1, .zone = 7},   Circuit{.num = 8, .space = 1, .zone = 8},
    Circuit{.num = 9, .space = 1, .zone = 9},   Circuit{.num = 10, .space = 1, .zone = 10},
    Circuit{.num = 11, .space = 1, .zone = 11}, Circuit{.num = 12, .space = 1, .zone = 12},
    Circuit{.num = 13, .space = 1, .zone = 13}, Circuit{.num = 14, .space = 1, .zone = 14},
    Circuit{.num = 15, .space = 1, .zone = 15}, Circuit{.num = 16, .space = 1, .zone = 16},
    Circuit{.num = 17, .space = 2, .zone = 1},  Circuit{.num = 18, .space = 2, .zone = 2},
    Circuit{.num = 19, .space = 2, .zone = 3},  Circuit{.num = 20, .space = 2, .zone = 4},
    Circuit{.num = 21, .space = 2, .zone = 5},  Circuit{.num = 22, .space = 2, .zone = 6},
    Circuit{.num = 23, .space = 2, .zone = 7},  Circuit{.num = 24, .space = 2, .zone = 8},
    Circuit{.num = 25, .space = 1, .zone = 1},  Circuit{.num = 26, .space = 1, .zone = 1},
    Circuit{.num = 27, .space = 1, .zone = 1},  Circuit{.num = 28, .space = 1, .zone = 1},
    Circuit{.num = 29, .space = 1, .zone = 1},  Circuit{.num = 30, .space = 1, .zone = 1},
    Circuit{.num = 31, .space = 1, .zone = 1},  Circuit{.num = 32, .space = 1, .zone = 1},
    Circuit{.num = 33, .space = 1, .zone = 1},  Circuit{.num = 34, .space = 1, .zone = 1},
    Circuit{.num = 35, .space = 1, .zone = 1},  Circuit{.num = 36, .space = 1, .zone = 1},
    Circuit{.num = 37, .space = 1, .zone = 1},  Circuit{.num = 38, .space = 1, .zone = 1},
    Circuit{.num = 39, .space = 1, .zone = 1},  Circuit{.num = 40, .space = 1, .zone = 1},
    Circuit{.num = 41, .space = 1, .zone = 1},  Circuit{.num = 42, .space = 1, .zone = 1},
    Circuit{.num = 43, .space = 1, .zone = 1},  Circuit{.num = 44, .space = 1, .zone = 1},
    Circuit{.num = 45, .space = 1, .zone = 1},  Circuit{.num = 46, .space = 1, .zone = 1},
    Circuit{.num = 47, .space = 1, .zone = 1},  Circuit{.num = 48, .space = 1, .zone = 1},
};

static const std::vector<Preset> kExpectedPresets = []()
{
    const decltype(Preset::levels) levels100 = genLevelsMap(255);
    const decltype(Preset::levels) levels75 = genLevelsMap(192);
    const decltype(Preset::levels) levels50 = genLevelsMap(128);
    const decltype(Preset::levels) levels25 = genLevelsMap(64);
    const decltype(Preset::fadeTimes) times0 = {{1, 0}, {2, 0}};
    return std::vector<Preset>{
        Preset{.num = 1, .levels = levels100, .fadeTimes = {{1, 2}, {2, 3}}},
        Preset{.num = 2, .levels = levels75, .fadeTimes = times0},
        Preset{.num = 3, .levels = levels50, .fadeTimes = times0},
        Preset{.num = 4, .levels = levels25, .fadeTimes = times0},
        Preset{.num = 5, .levels = levels100, .fadeTimes = times0},
        Preset{.num = 6, .levels = levels75, .fadeTimes = times0},
        Preset{.num = 7, .levels = levels50, .fadeTimes = times0},
        Preset{.num = 8, .levels = levels25, .fadeTimes = times0},
        Preset{.num = 9, .levels = levels100, .fadeTimes = times0},
        Preset{.num = 10, .levels = levels75, .fadeTimes = times0},
        Preset{.num = 11, .levels = levels50, .fadeTimes = times0},
        Preset{.num = 12, .levels = levels25, .fadeTimes = times0},
        Preset{.num = 13, .levels = levels100, .fadeTimes = times0},
        Preset{.num = 14, .levels = levels75, .fadeTimes = times0},
        Preset{.num = 15, .levels = levels50, .fadeTimes = times0},
        Preset{.num = 16, .levels = levels25, .fadeTimes = times0},
        Preset{.num = 17, .levels = levels100, .fadeTimes = times0},
        Preset{.num = 18, .levels = levels75, .fadeTimes = times0},
        Preset{.num = 19, .levels = levels50, .fadeTimes = times0},
        Preset{.num = 20, .levels = levels25, .fadeTimes = times0},
        Preset{.num = 21, .levels = levels100, .fadeTimes = times0},
        Preset{.num = 22, .levels = levels75, .fadeTimes = times0},
        Preset{.num = 23, .levels = levels50, .fadeTimes = times0},
        Preset{.num = 24, .levels = levels25, .fadeTimes = times0},
        Preset{.num = 25, .levels = levels100, .fadeTimes = times0},
        Preset{.num = 26, .levels = levels75, .fadeTimes = times0},
        Preset{.num = 27, .levels = levels50, .fadeTimes = times0},
        Preset{.num = 28, .levels = levels25, .fadeTimes = times0},
        Preset{.num = 29, .levels = levels100, .fadeTimes = times0},
        Preset{.num = 30, .levels = levels75, .fadeTimes = times0},
        Preset{.num = 31, .levels = levels50, .fadeTimes = times0},
        Preset{.num = 32, .levels = levels25, .fadeTimes = times0},
        Preset{.num = 33, .levels = levels100, .fadeTimes = times0},
        Preset{.num = 34, .levels = levels75, .fadeTimes = times0},
        Preset{.num = 35, .levels = levels50, .fadeTimes = times0},
        Preset{.num = 36, .levels = levels25, .fadeTimes = times0},
        Preset{.num = 37, .levels = levels100, .fadeTimes = times0},
        Preset{.num = 38, .levels = levels75, .fadeTimes = times0},
        Preset{.num = 39, .levels = levels50, .fadeTimes = times0},
        Preset{.num = 40, .levels = levels25, .fadeTimes = times0},
        Preset{.num = 41, .levels = levels100, .fadeTimes = times0},
        Preset{.num = 42, .levels = levels75, .fadeTimes = times0},
        Preset{.num = 43, .levels = levels50, .fadeTimes = times0},
        Preset{.num = 44, .levels = levels25, .fadeTimes = times0},
        Preset{.num = 45, .levels = levels100, .fadeTimes = times0},
        Preset{.num = 46, .levels = levels75, .fadeTimes = times0},
        Preset{.num = 47, .levels = levels50, .fadeTimes = times0},
        Preset{.num = 48, .levels = levels25, .fadeTimes = times0},
        Preset{.num = 49, .levels = levels100, .fadeTimes = times0},
        Preset{.num = 50, .levels = levels75, .fadeTimes = times0},
        Preset{.num = 51, .levels = levels50, .fadeTimes = times0},
        Preset{.num = 52, .levels = levels25, .fadeTimes = times0},
        Preset{.num = 53, .levels = levels100, .fadeTimes = times0},
        Preset{.num = 54, .levels = levels75, .fadeTimes = times0},
        Preset{.num = 55, .levels = levels50, .fadeTimes = times0},
        Preset{.num = 56, .levels = levels25, .fadeTimes = times0},
        Preset{.num = 57, .levels = levels100, .fadeTimes = times0},
        Preset{.num = 58, .levels = levels75, .fadeTimes = times0},
        Preset{.num = 59, .levels = levels50, .fadeTimes = times0},
        Preset{.num = 60, .levels = levels25, .fadeTimes = times0},
        Preset{.num = 61, .levels = levels100, .fadeTimes = times0},
        Preset{.num = 62, .levels = levels75, .fadeTimes = times0},
        Preset{.num = 63, .levels = levels50, .fadeTimes = times0},
        Preset{.num = 64, .levels = levels25, .fadeTimes = times0},
    };
}();

TEST_CASE("Echo PCP Config")
{
    EchoPcpConfig config;
    SECTION("Parse Cfg")
    {
        REQUIRE_NOTHROW(config.parseCfg(RESOURCES_PATH "/EchoPcpConfigTest/ERP.cfg"));

        // Circuits.
        std::vector<Circuit> actualCircuits;
        for (std::size_t ix = 0; ix < config.circuitCount(); ++ix)
        {
            actualCircuits.push_back(config.getCircuitAt(ix));
        }
        CHECK(kExpectedCircuits == actualCircuits);

        // Presets.
        std::vector<Preset> actualPresets;
        for (std::size_t presetIx = 0; presetIx < config.presetCount(); ++presetIx)
        {
            actualPresets.push_back(config.getPresetAt(presetIx));
        }
        CHECK(kExpectedPresets == actualPresets);
    }

    SECTION("Write Sheet")
    {
        REQUIRE_NOTHROW(config.parseCfg(RESOURCES_PATH "/EchoPcpConfigTest/ERP.cfg"));

        QTemporaryDir testDir;
        const auto xlsxFilePath = testDir.filePath("erp.xlsx");
        REQUIRE_NOTHROW(config.saveSheet(xlsxFilePath));
        QXlsx::Document expected(RESOURCES_PATH "/EchoPcpConfigTest/ERP.xlsx");
        QXlsx::Document actual(xlsxFilePath);

        CHECK_THAT(expected, MatchesXlsx(actual));
    }

    SECTION("Read Sheet")
    {
        REQUIRE_NOTHROW(config.parseSheet(RESOURCES_PATH "/EchoPcpConfigTest/ERP_changed.xlsx"));

        // Circuits.
        const auto expectedCircuits = []()
        {
            auto circuits{kExpectedCircuits};
            for (auto& circuit : circuits)
            {
                circuit.space += 1;
            }

            return circuits;
        }();
        std::vector<Circuit> actualCircuits;
        for (std::size_t ix = 0; ix < config.circuitCount(); ++ix)
        {
            actualCircuits.push_back(config.getCircuitAt(ix));
        }
        CHECK(expectedCircuits == actualCircuits);

        // Presets.
        const auto expectedPresets = []()
        {
            auto presets{kExpectedPresets};
            for (auto& preset : presets)
            {
                decltype(Preset::fadeTimes) fadeTimes;
                for (const auto [spaceNum, fadeTime] : preset.fadeTimes)
                {
                    fadeTimes.emplace(spaceNum + 1, 5);
                }
                preset.fadeTimes = fadeTimes;
            }
            auto preset16 = std::ranges::find_if(presets, [](const Preset& preset) { return preset.num == 16; });
            for (auto& level : preset16->levels | std::views::values)
            {
                level = 0;
            }
            return presets;
        }();
        std::vector<Preset> actualPresets;
        for (std::size_t presetIx = 0; presetIx < config.presetCount(); ++presetIx)
        {
            actualPresets.push_back(config.getPresetAt(presetIx));
        }
        CHECK(expectedPresets == actualPresets);
    }

    SECTION("Write Cfg")
    {
        REQUIRE_NOTHROW(config.parseSheet(RESOURCES_PATH "/EchoPcpConfigTest/ERP_changed.xlsx"));

        QTemporaryDir testDir;
        const auto cfgFilePath = testDir.filePath("erp_changed.cfg");
        REQUIRE_NOTHROW(config.saveCfg(RESOURCES_PATH "/EchoPcpConfigTest/ERP.cfg", cfgFilePath));

        // Parsing the expected and actual are the best way to account for minor differences in formatting.
        QFile fExpected(RESOURCES_PATH "/EchoPcpConfigTest/ERP_changed.cfg");
        REQUIRE(fExpected.open(QIODevice::ReadOnly));
        QDomDocument xmlExpected;
        xmlExpected.setContent(&fExpected);
        QFile fActual(cfgFilePath);
        REQUIRE(fActual.open(QIODevice::ReadOnly));
        QDomDocument xmlActual;
        xmlActual.setContent(&fActual);
        CHECK(xmlExpected.toString(4) == xmlActual.toString(4));
    }
}
