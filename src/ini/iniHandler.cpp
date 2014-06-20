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

#include "iniHandler.h"

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>

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

std::pair<std::string, std::string> iniHandler::parseKey(const std::string &buffer)
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

bool iniHandler::open(const char *fName)
{
    fileName.assign(fName);

    std::ifstream iniFile(fName);

    if (iniFile.fail())
    {
        return false;
    }

    sections_t::iterator mIt;

    while (iniFile.good())
    {
        std::string buffer;
        getline(iniFile, buffer);

        if (buffer.empty())
            continue;

        switch (buffer.at(0))
        {
        case ';':
        case '#':
            // skip comments
            break;

        case '[':
            try
            {
                const std::string section = parseSection(buffer);
                const keys_t keys;
                std::pair<sections_t::iterator, bool> it = sections.insert(make_pair(section, keys));
                mIt = it.first;
            }
            catch (parseError const &e) {}

            break;

        default:
            try
            {
                (*mIt).second.insert(parseKey(buffer));
            }
            catch (parseError const &e) {}

            break;
        }
    }

    return true;
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
    curSection = sections.find(std::string(section));
    return (curSection != sections.end());
}

const char *iniHandler::getValue(const char *key) const
{
    keys_t::const_iterator keyIt = (*curSection).second.find(std::string(key));
    return (keyIt != (*curSection).second.end()) ? keyIt->second.c_str() : 0;
}

void iniHandler::addSection(const char *section)
{
    const keys_t keys;
    curSection = sections.insert(curSection, make_pair(section, keys));
    changed = true;
}

void iniHandler::addValue(const char *key, const char *value)
{
    (*curSection).second.insert(make_pair(std::string(key), std::string(value)));
    changed = true;
}

bool iniHandler::write(const char *fName)
{
    std::ofstream iniFile(fName);

    if (iniFile.fail())
    {
        return false;
    }

    for (sections_t::iterator section = sections.begin(); section != sections.end(); ++section)
    {
        iniFile << "[" << (*section).first << "]" << std::endl;

        for (keys_t::iterator key = (*section).second.begin(); key != (*section).second.end(); ++key)
        {
            iniFile << (*key).first << " = " << (*key).second << std::endl;
        }
        iniFile << std::endl;
    }

    return true;
}
