/**
 * @file genLevelsMap.h
 *
 * @author Dan Keenan
 * @date 5/10/2025
 * @copyright GNU GPLv3
 */

#ifndef GENLEVELSMAP_H
#define GENLEVELSMAP_H

#include <echoconfig/Preset.h>

inline decltype(echoconfig::Preset::levels) genLevelsMap(unsigned int level, unsigned int startCircuit,
                                                         unsigned int endCircuit)
{
    decltype(echoconfig::Preset::levels) levels;
    for (unsigned int circuitNum = startCircuit; circuitNum <= endCircuit; ++circuitNum)
    {
        levels.emplace(circuitNum, level);
    }
    return levels;
}

#endif // GENLEVELSMAP_H
