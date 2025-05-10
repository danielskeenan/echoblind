/**
 * @file EchoPcpConfig.cpp
 *
 * @author Dan Keenan
 * @date 5/10/2025
 * @copyright GNU GPLv3
 */

#include "echoconfig/EchoPcpConfig.h"

#include <QFile>
#include <QSaveFile>
#include <QXmlStreamReader>
#include <ranges>

#include "echoconfig/xml_helpers.h"

namespace echoconfig
{
    void EchoPcpConfig::parseCfg(const QString& path)
    {
        QFile f(path);
        if (!f.open(QIODevice::ReadOnly))
        {
            throw std::runtime_error("Failed to open file");
        }

        QXmlStreamReader xml(&f);
        bool parsedRoot = false;
        std::optional<Preset> currentPreset;
        while (!xml.atEnd())
        {
            const auto tokenType = xml.readNext();
            if (tokenType == QXmlStreamReader::StartElement)
            {
                const auto tagName = xml.name();
                if (!parsedRoot && tagName != QStringLiteral("SMARTSWITCH2"))
                {
                    throw std::runtime_error("Incorrect root tag.");
                }
                else
                {
                    parsedRoot = true;
                }

                if (tagName == QStringLiteral("CABINET"))
                {
                    if (xml.attributes().value(QStringLiteral("VERSION")) != QStringLiteral("3.1.X"))
                    {
                        throw std::runtime_error("Incorrect version.");
                    }
                }
                else if (tagName == outputTagName())
                {
                    const auto circuitNum = xml_helpers::requiredAttrUInt(xml, QStringLiteral("NUMBER"));
                    const auto spaceNum = xml_helpers::requiredAttrUInt(xml, QStringLiteral("SPACE"));
                    const auto zoneNum = xml_helpers::requiredAttrUInt(xml, QStringLiteral("ZONE"));
                    auto& circuit = circuits_[circuitNum];
                    circuit.num = circuitNum;
                    circuit.space = spaceNum;
                    circuit.zone = zoneNum;
                }
                else if (tagName == QStringLiteral("SPACE"))
                {
                    static const auto kSpaceInRack = QStringLiteral("SPACEINRACK");
                    static const auto kSpaceInRackExt = QStringLiteral("SPACEINRACKEXT");
                    static const auto kNumber = QStringLiteral("NUMBER");
                    static const auto kNumberExt = QStringLiteral("NUMBEREXT");
                    QString spaceAttr;
                    QString numberAttr;
                    if (xml.attributes().hasAttribute(kSpaceInRack))
                    {
                        spaceAttr = kSpaceInRack;
                        numberAttr = kNumber;
                    }
                    else if (xml.attributes().hasAttribute(kSpaceInRackExt))
                    {
                        spaceAttr = kSpaceInRackExt;
                        numberAttr = kNumberExt;
                    }
                    else
                    {
                        throw std::runtime_error("Failed to read space attribute");
                    }
                    const unsigned int echoSpaceNum = xml_helpers::requiredAttrUInt(xml, numberAttr);
                    unsigned int rackSpaceNum;
                    if (echoSpaceNum == 0)
                    {
                        rackSpaceNum = 0;
                    }
                    else
                    {
                        rackSpaceNum = xml_helpers::requiredAttrUInt(xml, spaceAttr);
                    }
                    rackSpaces_.left.insert({rackSpaceNum, echoSpaceNum});
                    auto& space = spaces_[echoSpaceNum];
                    space.num = echoSpaceNum;
                }
                else if (tagName == QStringLiteral("PRESET"))
                {
                    if (currentPreset.has_value())
                    {
                        presets_.insert_or_assign(currentPreset->num, currentPreset.value());
                    }
                    const auto presetNum = xml_helpers::requiredAttrUInt(xml, QStringLiteral("NUMBER"));
                    currentPreset.emplace();
                    currentPreset->num = presetNum;
                }
                else if (tagName == QStringLiteral("PREFADELEVEL"))
                {
                    if (!currentPreset.has_value())
                    {
                        throw std::runtime_error("No current preset.");
                    }
                    const auto fadeTime = xml_helpers::requiredAttrUInt(xml, fadeTimeAttrName());
                    const auto rackSpaceNum = xml_helpers::requiredAttrUInt(xml, QStringLiteral("SPACEINRACK"));
                    try
                    {
                        const auto echoSpaceNum = rackSpaces_.left.at(rackSpaceNum);
                        currentPreset->fadeTimes[echoSpaceNum] = fadeTime;
                    }
                    catch (const std::out_of_range&)
                    {
                        throw std::runtime_error("No matching Echo preset.");
                    }
                }
                else if (tagName == QStringLiteral("PRELEVEL"))
                {
                    if (!currentPreset.has_value())
                    {
                        throw std::runtime_error("No current preset.");
                    }
                    const auto level = xml_helpers::requiredAttrUInt(xml, QStringLiteral("LEVEL"));
                    const auto circuit = xml_helpers::requiredAttrUInt(xml, outputAttrName());
                    currentPreset->levels[circuit] = level;
                }
            }
        }
        if (xml.hasError())
        {
            throw std::runtime_error("Failed to read file");
        }
    }

    void EchoPcpConfig::saveCfg(const QString& basePath, const QString& outPath) const
    {
        QFile fIn(basePath);
        if (!fIn.open(QIODevice::ReadOnly))
        {
            throw std::runtime_error("Failed to open base file");
        }
        QSaveFile fOut(outPath);
        if (!fOut.open(QIODevice::WriteOnly))
        {
            throw std::runtime_error("Failed to open output file");
        }

        QXmlStreamReader xmlIn(&fIn);
        QXmlStreamWriter xmlOut(&fOut);
        xmlOut.setAutoFormatting(true);
        bool parsedRoot = false;
        std::optional<Preset> currentPreset;
        while (!xmlIn.atEnd() && !xmlOut.hasError())
        {
            const auto tokenType = xmlIn.readNext();
            // All other token types will be written as-is.
            if (tokenType == QXmlStreamReader::StartElement)
            {
                const auto tagName = xmlIn.name();
                // Mutable attributes.
                auto attrs = xmlIn.attributes();
                if (!parsedRoot && tagName != QStringLiteral("SMARTSWITCH2"))
                {
                    throw std::runtime_error("Incorrect root tag.");
                }
                else
                {
                    parsedRoot = true;
                }

                if (tagName == QStringLiteral("CABINET"))
                {
                    if (xmlIn.attributes().value(QStringLiteral("VERSION")) != QStringLiteral("3.1.X"))
                    {
                        throw std::runtime_error("Incorrect version.");
                    }
                }
                else if (tagName == outputTagName())
                {
                    const auto circuitNum = xml_helpers::requiredAttrUInt(xmlIn, QStringLiteral("NUMBER"));
                    auto circuit = circuits_.find(circuitNum);
                    if (circuit != circuits_.end())
                    {
                        xml_helpers::replaceAttr(attrs, QStringLiteral("SPACE"),
                                                 QString::number(circuit->second.space));
                        xml_helpers::replaceAttr(attrs, QStringLiteral("ZONE"), QString::number(circuit->second.zone));
                    }
                }
                else if (tagName == QStringLiteral("SPACE"))
                {
                    static const auto kSpaceInRack = QStringLiteral("SPACEINRACK");
                    static const auto kSpaceInRackExt = QStringLiteral("SPACEINRACKEXT");
                    static const auto kNumber = QStringLiteral("NUMBER");
                    static const auto kNumberExt = QStringLiteral("NUMBEREXT");
                    QString spaceAttr;
                    QString numberAttr;
                    if (xmlIn.attributes().hasAttribute(kSpaceInRack))
                    {
                        spaceAttr = kSpaceInRack;
                    }
                    else if (xmlIn.attributes().hasAttribute(kSpaceInRackExt))
                    {
                        spaceAttr = kSpaceInRackExt;
                    }
                    else
                    {
                        throw std::runtime_error("Failed to read space attribute");
                    }
                    const unsigned int rackSpaceNum = xml_helpers::requiredAttrUInt(xmlIn, spaceAttr);
                    try
                    {
                        const auto echoSpaceNum = rackSpaces_.left.at(rackSpaceNum);
                        if (echoSpaceNum > 16 || echoSpaceNum < 1)
                        {
                            spaceAttr = kSpaceInRackExt;
                            numberAttr = kNumberExt;
                        }
                        else
                        {
                            spaceAttr = kSpaceInRack;
                            numberAttr = kNumber;
                        }
                        xml_helpers::replaceAttr(attrs, spaceAttr, QString::number(rackSpaceNum));
                        xml_helpers::replaceAttr(attrs, numberAttr, QString::number(echoSpaceNum));
                    }
                    catch (const std::out_of_range&)
                    {
                        throw std::runtime_error("No matching Echo space.");
                    }
                }
                else if (tagName == QStringLiteral("PRESET"))
                {
                    const auto presetNum = xml_helpers::requiredAttrUInt(xmlIn, QStringLiteral("NUMBER"));
                    try
                    {
                        currentPreset = presets_.at(presetNum);
                    }
                    catch (const std::out_of_range&)
                    {
                        throw std::runtime_error("No matching Echo preset.");
                    }
                }
                else if (tagName == QStringLiteral("PREFADELEVEL"))
                {
                    if (!currentPreset.has_value())
                    {
                        throw std::runtime_error("No current preset.");
                    }
                    const auto rackSpaceNum = xml_helpers::requiredAttrUInt(xmlIn, QStringLiteral("SPACEINRACK"));
                    try
                    {
                        const auto echoSpaceNum = rackSpaces_.left.at(rackSpaceNum);
                        const auto fadeTime = currentPreset->fadeTimes.at(echoSpaceNum);
                        xml_helpers::replaceAttr(attrs, fadeTimeAttrName(), QString::number(fadeTime));
                    }
                    catch (const std::out_of_range&)
                    {
                        throw std::runtime_error("No matching Echo space.");
                    }
                }
                else if (tagName == QStringLiteral("PRELEVEL"))
                {
                    if (!currentPreset.has_value())
                    {
                        throw std::runtime_error("No current preset.");
                    }
                    const auto circuit = xml_helpers::requiredAttrUInt(xmlIn, outputAttrName());
                    try
                    {
                        const auto level = currentPreset->levels.at(circuit);
                        xml_helpers::replaceAttr(attrs, QStringLiteral("LEVEL"), QString::number(level));
                    }
                    catch (const std::out_of_range&)
                    {
                        throw std::runtime_error("No matching Echo circuit.");
                    }
                }

                // Write the modified element.
                xmlOut.writeStartElement(xmlIn.qualifiedName());
                for (const auto& ns : xmlIn.namespaceDeclarations())
                {
                    if (ns.prefix().empty())
                    {
                        xmlOut.writeDefaultNamespace(ns.namespaceUri());
                    }
                    else
                    {
                        xmlOut.writeNamespace(ns.namespaceUri(), ns.prefix());
                    }
                }
                xmlOut.writeAttributes(attrs);
            }
            else
            {
                xmlOut.writeCurrentToken(xmlIn);
            }
        }
        if (xmlIn.hasError() || xmlOut.hasError())
        {
            throw std::runtime_error("Failed to save file");
        }

        fOut.commit();
    }

    const Circuit& EchoPcpConfig::getCircuitAt(const unsigned int ix) const
    {
        Q_ASSERT(ix < circuits_.size());
        auto it = circuits_.cbegin();
        std::advance(it, ix);
        return it->second;
    }

    Circuit& EchoPcpConfig::getCircuitAt(unsigned int ix)
    {
        Q_ASSERT(ix < circuits_.size());
        auto it = circuits_.begin();
        std::advance(it, ix);
        return it->second;
    }

    const Space& EchoPcpConfig::getSpaceAt(unsigned int ix) const
    {
        Q_ASSERT(ix < spaces_.size());
        auto it = spaces_.cbegin();
        std::advance(it, ix);
        return it->second;
    }

    Space& EchoPcpConfig::getSpaceAt(unsigned int ix)
    {
        Q_ASSERT(ix < spaces_.size());
        auto it = spaces_.begin();
        std::advance(it, ix);
        return it->second;
    }

    const Space& EchoPcpConfig::getSpaceAtRack(unsigned int ix) const
    {
        Q_ASSERT(ix < spaces_.size());
        auto rackIt = rackSpaces_.left.begin();
        std::advance(rackIt, ix);
        const auto echoSpaceNum = rackIt->second;
        return spaces_.at(echoSpaceNum);
    }

    Space& EchoPcpConfig::getSpaceAtRack(unsigned int ix)
    {
        Q_ASSERT(ix < spaces_.size());
        auto rackIt = rackSpaces_.left.begin();
        std::advance(rackIt, ix);
        const auto echoSpaceNum = rackIt->second;
        return spaces_.at(echoSpaceNum);
    }

    const Preset& EchoPcpConfig::getPresetAt(unsigned int ix) const
    {
        Q_ASSERT(ix < presets_.size());
        auto it = presets_.cbegin();
        std::advance(it, ix);
        return it->second;
    }

    Preset& EchoPcpConfig::getPresetAt(unsigned int ix)
    {
        Q_ASSERT(ix < presets_.size());
        auto it = presets_.begin();
        std::advance(it, ix);
        return it->second;
    }
} // namespace echoconfig
