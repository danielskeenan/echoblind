/**
 * @file xml_helpers.cpp
 *
 * @author Dan Keenan
 * @date 5/10/2025
 * @copyright GNU GPLv3
 */

#include "echoconfig/xml_helpers.h"
#include <algorithm>

namespace echoconfig::xml_helpers
{
    unsigned int requiredAttrUInt(QXmlStreamReader& xml, QAnyStringView qualifiedName)
    {
        if (!xml.attributes().hasAttribute(qualifiedName))
        {
            throw std::runtime_error("Required attribute missing");
        }
        bool isInt;
        const unsigned int val = xml.attributes().value(qualifiedName).toUInt(&isInt);
        if (!isInt)
        {
            throw std::runtime_error("Not a UInt");
        }
        return val;
    }

    void replaceAttr(QXmlStreamAttributes& attrs, const QString& qualifiedName, const QString& value)
    {
        std::ranges::replace_if(
            attrs, [&](const QXmlStreamAttribute& attr) { return attr.qualifiedName() == qualifiedName; },
            QXmlStreamAttribute(qualifiedName, value));
    }
} // namespace echoconfig::xml_helpers
