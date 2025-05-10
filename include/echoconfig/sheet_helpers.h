/**
 * @file sheet_helpers.h
 *
 * @author Dan Keenan
 * @date 5/10/2025
 * @copyright GNU GPLv3
 */

#ifndef SHEET_HELPERS_H
#define SHEET_HELPERS_H

#include <xlsxdocument.h>

namespace echoconfig::sheet_helpers
{
    /**
     * Get an unsigned int from @p doc at @p row @p col
     * @param doc
     * @param row
     * @param col
     * @return
     * @throws std::runtime_error if the value does not exist or cannot be converted to unsigned int.
     */
    unsigned int requiredCellUInt(const QXlsx::Document* doc, int row, int col);
} // namespace echoconfig::sheet_helpers

#endif // SHEET_HELPERS_H
