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

#include "iniHandler.h"

#include <cstdlib>

#include <algorithm>

#ifdef _WIN32
#  include <windows.h>
#  include <stringapiset.h>
#endif

namespace fs = ghc::filesystem;

template<class T>
class compare
{
private:
    std::string s;

public:
    compare(const char *str) : s(str) {}

    bool operator () (T const &p) { return s.compare(p.first) == 0; }
};

//

iniHandler::iniHandler() :
    changed(false)
{}

iniHandler::~iniHandler()
{
    close();
}

std::string iniHandler::parseSection(const std::string &buffer)
{
    const size_t pos = buffer.find(']');

    if (pos == std::string::npos)
    {
        throw parseError();
    }

    return buffer.substr(1, pos-1);
}

iniHandler::stringPair_t iniHandler::parseKey(const std::string &buffer)
{
    const size_t pos = buffer.find('=');

    if (pos == std::string::npos)
    {
        throw parseError();
    }

    const std::string key = buffer.substr(0, buffer.find_last_not_of(' ', pos-1) + 1);
    const size_t vpos = buffer.find_first_not_of(' ', pos+1);
    const std::string value = (vpos == std::string::npos) ? "" : buffer.substr(vpos);
    return make_pair(key, value);
}

bool iniHandler::open(const fs::path &fName)
{
    if (tryOpen(fName))
        return true;

    // Try creating new file
    fs::ofstream newIniFile(fName);
    return newIniFile.is_open();
}

bool iniHandler::tryOpen(const fs::path &fName)
{
    fileName.assign(fName);

    fs::ifstream iniFile(fName);

    if (!iniFile.is_open())
    {
        return false;
    }

    std::string buffer;

    while (getline(iniFile, buffer))
    {
        if (buffer.empty())
            continue;

        switch (buffer.at(0))
        {
        case ';':
        case '#':
            // Comments
            if (!sections.empty())
            {
                sections_t::reference lastSect(sections.back());
                lastSect.second.push_back(make_pair(std::string(), buffer));
            }
            break;

        case '[':
            try
            {
                const std::string section = parseSection(buffer);
                const keys_t keys;
                sections.push_back(make_pair(section, keys));
            }
            catch (parseError const &e) {}

            break;

        default:
            try
            {
                if (!sections.empty()) //FIXME add a default section?
                {
                    sections_t::reference lastSect(sections.back());
                    lastSect.second.push_back(parseKey(buffer));
                }
            }
            catch (parseError const &e) {}

            break;
        }
    }

    return !iniFile.bad();
}

void iniHandler::close()
{
    if (changed)
    {
        write(fileName.c_str());
    }

    sections.clear();
    changed = false;
}

bool iniHandler::setSection(const char *section)
{
    curSection = std::find_if(sections.begin(), sections.end(), compare<keyPair_t>(section));
    return (curSection != sections.end());
}

const char *iniHandler::getValue(const char *key) const
{
    keys_t::const_iterator keyIt = std::find_if((*curSection).second.begin(), (*curSection).second.end(), compare<stringPair_t>(key));
    return (keyIt != (*curSection).second.end()) ? keyIt->second.c_str() : nullptr;
}

void iniHandler::addSection(const char *section)
{
    const keys_t keys;
    curSection = sections.insert(curSection, make_pair(section, keys));
    changed = true;
}

void iniHandler::addValue(const char *key, const char *value)
{
    (*curSection).second.push_back(make_pair(std::string(key), std::string(value)));
    changed = true;
}

void iniHandler::removeValue(const char *key)
{
    auto section = &(*curSection).second;
    section->erase(std::remove_if(section->begin(), section->end(), compare<stringPair_t>(key)));
    changed = true;
}

bool iniHandler::write(const fs::path &fName)
{
    fs::ofstream iniFile(fName);

    if (!iniFile.is_open())
    {
        return false;
    }

    for (auto & section : sections)
    {
        iniFile << "[" << section.first << "]" << std::endl;

        for (keys_t::iterator entry = section.second.begin(); entry != section.second.end(); ++entry)
        {
            const std::string key = (*entry).first;
            if (!key.empty())
                iniFile << key << " = ";
            iniFile << (*entry).second << std::endl;
        }
        iniFile << std::endl;
    }

    return true;
}
