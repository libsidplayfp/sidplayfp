/*
 *  Copyright (C) 2010-2014 Leandro Nini
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

#ifndef INIHANDLER_H
#define INIHANDLER_H

#include <string>
#include <vector>

class iniHandler
{
private:
    typedef std::pair<std::string, std::string> stringPair_t;
    typedef std::vector<stringPair_t> keys_t;

    typedef std::pair<std::string, keys_t> keyPair_t;
    typedef std::vector<keyPair_t> sections_t;

    class parseError {};

private:
    template<class T>
    class compare
    {
    private:
        std::string s;

    public:
        compare(const char *str) : s(str) {}

        bool operator () (T const &p) { return s.compare(p.first) == 0; }
    };

private:
    sections_t sections;

    sections_t::iterator curSection;

    std::string fileName;

    bool changed;

private:
    std::string parseSection(const std::string &buffer);

    stringPair_t parseKey(const std::string &buffer);

public:
    iniHandler();
    ~iniHandler();

    bool open(const char *fName);
    bool write(const char *fName);
    void close();

    bool setSection(const char *section);
    const char *getValue(const char *key) const;

    void addSection(const char *section);
    void addValue(const char *key, const char *value);
};

#endif // INIHANDLER_H
