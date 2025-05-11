/**
 * @file EchoAcpConfig.cpp
 *
 * @author Dan Keenan
 * @date 5/10/2025
 * @copyright GNU GPLv3
 */

#include "echoconfig/EchoAcpConfig.h"
#include <QVersionNumber>
#include <QDebug>

namespace echoconfig
{
    bool EchoAcpConfig::isVersionCompatible(QStringView versionStr) const
    {
        const auto version = QVersionNumber::fromString(versionStr);
        if (version.isNull() || !QVersionNumber(2, 0).isPrefixOf(version))
        {
            return false;
        }
        return true;
    }
} // namespace echoconfig
