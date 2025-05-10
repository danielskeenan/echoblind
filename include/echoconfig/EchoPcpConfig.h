/**
 * @file EchoPcpConfig.h
 *
 * @author Dan Keenan
 * @date 5/10/2025
 * @copyright GNU GPLv3
 */

#ifndef ECHOPCPCONFIG_H
#define ECHOPCPCONFIG_H

#include <boost/bimap.hpp>
#include <boost/bimap/unordered_set_of.hpp>
#include <unordered_map>
#include "echoconfig/Config.h"
#include "echoconfig/Space.h"

namespace echoconfig
{
    class EchoPcpConfig : public Config
    {
    public:
        [[nodiscard]] QString panelType() const override { return tr("Echo PCP v3.1.X"); }
        [[nodiscard]] QString panelName() const override { return name_; }

        void parseCfg(const QString& path) override;
        void saveCfg(const QString& basePath, const QString& outPath) const override;

        [[nodiscard]] unsigned circuitCount() const override { return circuits_.size(); }
        [[nodiscard]] const Circuit& getCircuitAt(unsigned int ix) const override;
        [[nodiscard]] Circuit& getCircuitAt(unsigned int ix) override;
        [[nodiscard]] const Circuit& getCircuit(unsigned num) const override { return circuits_.at(num); }
        [[nodiscard]] Circuit& getCircuit(unsigned num) override { return circuits_[num]; }

        [[nodiscard]] unsigned spaceCount() const override { return spaces_.size(); }
        [[nodiscard]] const Space& getSpaceAt(unsigned int ix) const override;
        [[nodiscard]] Space& getSpaceAt(unsigned int ix) override;
        [[nodiscard]] const Space& getSpaceAtRack(unsigned int ix) const;
        [[nodiscard]] Space& getSpaceAtRack(unsigned int ix);
        [[nodiscard]] const Space& getSpace(unsigned num) const override { return spaces_.at(num); }
        [[nodiscard]] Space& getSpace(unsigned num) override { return spaces_[num]; }
        [[nodiscard]] const Space& getRackSpace(unsigned num) const { return spaces_.at(rackSpaces_.left.at(num)); }
        [[nodiscard]] Space& getRackSpace(unsigned num) { return spaces_[rackSpaces_.left.at(num)]; }

        [[nodiscard]] unsigned presetCount() const override { return presets_.size(); }
        [[nodiscard]] const Preset& getPresetAt(unsigned int ix) const override;
        [[nodiscard]] Preset& getPresetAt(unsigned int ix) override;
        [[nodiscard]] const Preset& getPreset(unsigned num) const override { return presets_.at(num); }
        [[nodiscard]] Preset& getPreset(unsigned num) override { return presets_[num]; }

    protected:
        [[nodiscard]] virtual const QString& outputTagName() const { return QStringLiteral("RELAY"); }
        [[nodiscard]] virtual const QString& outputAttrName() const { return QStringLiteral("RELAY"); }
        [[nodiscard]] virtual const QString& fadeTimeAttrName() const { return QStringLiteral("UPTIME"); }

    private:
        using RackSpaceMap = boost::bimap<unsigned int, unsigned int>;

        QString name_;
        /** Circuit num > Circuit */
        std::unordered_map<unsigned int, Circuit> circuits_;
        /** Rack space num > Echo space num */
        RackSpaceMap rackSpaces_;
        /** Echo space num > Space */
        std::unordered_map<unsigned int, Space> spaces_;
        /** Preset num > Preset */
        std::unordered_map<unsigned int, Preset> presets_;
    };
} // namespace echoconfig

#endif // ECHOPCPCONFIG_H
