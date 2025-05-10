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
    public:
        auto operator<=>(const Preset&) const = default;

        unsigned int num;
        /** Space num > fade time (seconds) */
        std::map<unsigned int, unsigned int> fadeTimes;
        /** Rack ckt num > level (0-255) */
        std::map<unsigned int, unsigned int> levels;
    };
} // namespace echoconfig

#endif // PRESET_H
