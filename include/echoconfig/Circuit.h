/**
 * @file Circuit.h
 *
 * @author Dan Keenan
 * @date 5/10/2025
 * @copyright GNU GPLv3
 */

#ifndef CIRCUIT_H
#define CIRCUIT_H

namespace echoconfig
{
    /**
     * A single circuit in a panel.
     */
    class Circuit
    {
    public:
        unsigned int num;
        unsigned int space_num;
        unsigned int zone_num;
    };
} // namespace echoconfig

#endif // CIRCUIT_H
