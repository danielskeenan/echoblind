/**
 * @file Preset.h
 *
 * @author Dan Keenan
 * @date 5/10/2025
 * @copyright GNU GPLv3
 */

#ifndef PRESET_H
#define PRESET_H

#include <map>

namespace echoconfig
{
    class Preset
    {
        friend std::ostream& operator<<(std::ostream& os, const Preset& val)
        {
            os << "<Preset " << val.num << ": ";
            for (const auto [spaceNum, fadeTime] : val.fadeTimes)
            {
                os << "S" << spaceNum << "T" << fadeTime << ", ";
            }
            for (const auto [circuitNum, level] : val.levels)
            {
                os << circuitNum << "@" << level << ", ";
            }
            os << ">";
            return os;
        }

    public:
        auto operator<=>(const Preset&) const = default;

        unsigned int num;
        std::map<unsigned int, unsigned int> levels;
        /** Space num > fade time (seconds) */
        std::map<unsigned int, unsigned int> fadeTimes;
        /** Rack ckt num > level (0-255) */
    };
} // namespace echoconfig

#endif // PRESET_H
