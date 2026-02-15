/*
 *  Copyright (C) 2010-2026 Leandro Nini
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

#include "filesystem/filesystem.hpp"

#include <string>
#include <vector>

namespace fs = ghc::filesystem;

class iniHandler
{
private:
    using stringPair_t = std::pair<std::string, std::string>;
    using keys_t = std::vector<stringPair_t>;

    using keyPair_t = std::pair<std::string, keys_t>;
    using sections_t = std::vector<keyPair_t>;

    class parseError {};

private:
    sections_t sections;

    sections_t::iterator curSection;

    fs::path fileName;

    bool changed;

private:
    std::string parseSection(const std::string &buffer);

    stringPair_t parseKey(const std::string &buffer);

public:
    iniHandler();
    ~iniHandler();

    std::string getFilename() const { return fileName.string(); }

    bool tryOpen(const fs::path &fName);
    bool open(const fs::path &fName);
    bool write(const fs::path &fName);
    void close();

    bool setSection(const char *section);
    const char *getValue(const char *key) const;

    void addSection(const char *section);
    void addValue(const char *key, const char *value);
    void removeValue(const char *key);
};

#endif // INIHANDLER_H
