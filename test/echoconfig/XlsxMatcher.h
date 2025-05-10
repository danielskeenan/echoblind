/**
 * @file XlsxMatcher.h
 *
 * @author Dan Keenan
 * @date 5/10/2025
 * @copyright GNU GPLv3
 */

#ifndef XLSXMATCHER_H
#define XLSXMATCHER_H

#include <catch2/matchers/catch_matchers_templated.hpp>

#include "xlsxdocument.h"

#include <xlsxworkbook.h>

class XlsxMatcher : public Catch::Matchers::MatcherGenericBase
{
public:
    XlsxMatcher(const QXlsx::Document& doc) : doc_(doc) {}

    bool match(const QXlsx::Document& other) const
    {
        // Check sheets.
        if (doc_.sheetNames() != other.sheetNames())
        {
            return false;
        }

        // Check contents.
        for (std::size_t ix = 0; ix < doc_.sheetNames().size() && ix < other.sheetNames().size(); ++ix)
        {
            doc_.workbook()->setActiveSheet(ix);
            other.workbook()->setActiveSheet(ix);
            if (doc_.dimension() != other.dimension())
            {
                return false;
            }
            int lhsMaxRow, lhsMaxCol, rhsMaxRow, rhsMaxCol;
            auto lhsCells = doc_.currentWorksheet()->getFullCells(&lhsMaxRow, &lhsMaxCol);
            std::sort(lhsCells.begin(), lhsCells.end(), &XlsxMatcher::cmpCellLocation);
            auto rhsCells = other.currentWorksheet()->getFullCells(&rhsMaxRow, &rhsMaxCol);
            std::sort(rhsCells.begin(), rhsCells.end(), &XlsxMatcher::cmpCellLocation);
            auto lhsCellsIt = lhsCells.cbegin();
            auto rhsCellsIt = rhsCells.cbegin();
            for (; lhsCellsIt != lhsCells.cend() && rhsCellsIt != rhsCells.cend(); ++lhsCellsIt, ++rhsCellsIt)
            {
                const auto lhsCell = lhsCellsIt->cell;
                const auto rhsCell = rhsCellsIt->cell;
                if (lhsCell->cellType() != rhsCell->cellType())
                {
                    return false;
                }
                if (lhsCell->readValue() != rhsCell->readValue())
                {
                    return false;
                }
                if (lhsCell->format() != rhsCell->format())
                {
                    return false;
                }
            }
        }
        return true;
    }

protected:
    std::string describe() const override
    {
        // TODO: Can this be more descriptive?
        return "Matches Excel workbook.";
    }

private:
    const QXlsx::Document& doc_;

    static bool cmpCellLocation(const QXlsx::CellLocation& a, const QXlsx::CellLocation& b)
    {
        return std::tie(a.row, a.col) < std::tie(b.row, b.col);
    }
};

inline XlsxMatcher MatchesXlsx(const QXlsx::Document& doc) { return XlsxMatcher(doc); }

#endif // XLSXMATCHER_H
