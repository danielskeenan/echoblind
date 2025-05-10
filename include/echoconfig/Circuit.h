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
        friend std::ostream& operator<<(std::ostream& os, const Circuit& val)
        {
            os << "<Circuit " << val.num << ": S" << val.space << "Z" << val.zone << ">";
            return os;
        }

    public:
        auto operator<=>(const Circuit&) const = default;

        unsigned int num;
        unsigned int space;
        unsigned int zone;
    };
} // namespace echoconfig

#endif // CIRCUIT_H
