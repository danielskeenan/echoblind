/**
 * @file EchoAcpConfig.h
 *
 * @author Dan Keenan
 * @date 5/10/2025
 * @copyright GNU GPLv3
 */

#ifndef ECHOACPCONFIG_H
#define ECHOACPCONFIG_H

#include "EchoPcpConfig.h"

namespace echoconfig
{
    class EchoAcpConfig : public EchoPcpConfig
    {
        Q_OBJECT
    public:
        using EchoPcpConfig::EchoPcpConfig;
        [[nodiscard]] QString panelType() const override
        {
            return tr("Echo ACP v2.0.X");
        }

    protected:
        [[nodiscard]] QString rootTagName() const override { return QStringLiteral("EACP"); }
        [[nodiscard]] QString rackTagName() const override { return QStringLiteral("RACK"); }
        [[nodiscard]] QString outputTagName() const override { return QStringLiteral("OUTPUT"); }
        [[nodiscard]] QString outputAttrName() const override { return QStringLiteral("OUTPUT"); }
        [[nodiscard]] QString fadeTimeAttrName() const override { return QStringLiteral("PREFADELEVEL"); }
        [[nodiscard]] bool isVersionCompatible(QStringView versionStr) const override;
    };
} // namespace echoconfig

#endif // ECHOACPCONFIG_H
