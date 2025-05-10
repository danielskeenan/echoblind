/**
 * @file sheet_helpers.cpp
 *
 * @author Dan Keenan
 * @date 5/10/2025
 * @copyright GNU GPLv3
 */

#include "echoconfig/sheet_helpers.h"

namespace echoconfig::sheet_helpers
{
    unsigned int requiredCellUInt(const QXlsx::Document* doc, int row, int col)
    {
        bool isInt;
        auto val = doc->read(row, col);
        if (!val.isValid() || val.toString().isEmpty())
        {
            throw std::runtime_error("Missing required value");
        }
        const auto intVal = val.toUInt(&isInt);
        if (!isInt)
        {
            throw std::runtime_error("Non-numeric value");
        }
        return intVal;
    }
} // namespace echoconfig::sheet_helpers
