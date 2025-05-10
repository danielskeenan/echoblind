/**
 * @file qbytearray_tostring.h
 *
 * @author Dan Keenan
 * @date 5/10/2025
 * @copyright GNU GPLv3
 */

#ifndef QBYTEARRAY_TOSTRING_H
#define QBYTEARRAY_TOSTRING_H

inline std::ostream& operator<<(std::ostream& os, const QByteArray& obj)
{
    os << obj.toHex().toStdString();
    return os;
}

#endif // QBYTEARRAY_TOSTRING_H
