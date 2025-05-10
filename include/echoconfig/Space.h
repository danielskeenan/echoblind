/**
 * @file Space.h
 *
 * @author Dan Keenan
 * @date 5/10/2025
 * @copyright GNU GPLv3
 */

#ifndef SPACE_H
#define SPACE_H

namespace echoconfig
{
    class Space
    {
    public:
        auto operator<=>(const Space&) const = default;

        unsigned int num;

        [[nodiscard]] bool isEchoSpace() const { return num >= 1 && num <= 16; }
    };
} // namespace echoconfig

#endif // SPACE_H
