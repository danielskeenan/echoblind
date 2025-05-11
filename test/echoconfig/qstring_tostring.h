/**
 * @file qstring_tostring.h
 *
 * @author Dan Keenan
 * @date 5/10/2025
 * @copyright GNU GPLv3
 */

#ifndef QSTRING_TOSTRING_H
#define QSTRING_TOSTRING_H

inline std::ostream& operator<<(std::ostream& os, const QString& obj)
{
    os << obj.toStdString();
    return os;
}

#endif // QSTRING_TOSTRING_H
