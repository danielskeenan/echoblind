/**
 * @file xml_helpers.h
 *
 * @author Dan Keenan
 * @date 5/10/2025
 * @copyright GNU GPLv3
 */

#ifndef XML_HELPERS_H
#define XML_HELPERS_H

#include <QXmlStreamReader>

namespace echoconfig::xml_helpers
{
    /**
     * Get an unsigned int from the attribute @p qualifiedName.
     * @param xml
     * @param qualifiedName
     * @return
     */
    unsigned int requiredAttrUInt(QXmlStreamReader& xml, QAnyStringView qualifiedName);

    /**
     * Replace (or append, if the attribute is not set) an attribute value.
     *
     * All attributes with the name @p qualifiedName have their value replaced with @p value.
     *
     * @param attrs
     * @param qualifiedName
     * @param value
     */
    void replaceAttr(QXmlStreamAttributes& attrs, const QString& qualifiedName, const QString& value);
} // namespace echoconfig::xml_helpers

#endif // XML_HELPERS_H
