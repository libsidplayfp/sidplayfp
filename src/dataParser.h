/*
 *  Copyright (C) 2014-2026 Leandro Nini
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef DATAPARSER_H
#define DATAPARSER_H

#include <sstream>
#include <cstring>

class dataParser
{
public:
    class parseError {};

private:
    template<typename T, typename U>
    static T convertString(const U* data)
    {
        T value;

        std::basic_stringstream<U> stream(data);
        stream >> std::boolalpha >> value;
        if (stream.fail()) {
            throw parseError();
        }
        return value;
    }

public:
    template<typename U>
    static double parseDouble(const U* data) { return convertString<double>(data); }
    template<typename U>
    static int parseInt(const U* data) { return convertString<int>(data); }
    template<typename U>
    static bool parseBool(const U* data) { return convertString<bool>(data); }
};

#endif // DATAPARSER_H
