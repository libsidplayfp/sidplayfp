/*
 * This file is part of sidplayfp, a console SID player.
 *
 * Copyright 2012-2026 Leandro Nini
 * Copyright 1998, 2002 LaLa <LaLa@C64.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

//
// STILView - command line version
//

#include "fmt/format.h"

#include <fstream>
#include <iostream>
#include <string>

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <codeConvert.h>

#include <stilview/stil.h>

#include "sidcxx11.h"

STIL myStil;
char *hvscLoc = nullptr;
char *entryStr = nullptr;
int tuneNo = 0;
STIL::STILField field = STIL::all;
bool showBug = true;
bool showEntry = true;
bool showSection = true;
bool showVersion = false;
bool interactive = false;
bool demo = false;

char STIL_DEMO_ENTRY[]="/Galway_Martin/Green_Beret.sid";

// This is used for testing setBaseDir() when switching between different
// HVSC base directories. Ideally, it should point to a valid HVSC dir.
char OTHER_HVSC_BASE_DIR[]="E:\\MUSIC\\SID\\C64music\\";

constexpr int STIL_MAX_PATH_SIZE = 1024;

template<typename T>
requires std::is_enum_v<T>
auto format_as(T t) { return fmt::underlying(t); } 

//using namespace std;

char toLowerAscii(char c)
{
    return (c < 'A' || c > 'Z') ? c : c + ('a' - 'A');
}

bool strEquals(const char* s1, const char* s2)
{
    while (*s1 && *s2)
    {
        if (toLowerAscii(*s1) != toLowerAscii(*s2))
            return false;
        s1++;
        s2++;
    }

    return !(*s1 || *s2);
}

void printUsageStr(void)
{
    fmt::print("\n{}", myStil.getVersion());
    fmt::print("USAGE: STILView [-e=<entry>] [-l=<HVSC loc>] [-t=<tuneNo>] [-f=<field>]\n");
    fmt::print("                [-d] [-i] [-s] [-b] [-o] [-v] [-h] [-m]\n");
}

void printUsage(void)
{
    printUsageStr();
    exit(EXIT_FAILURE);
}

void printHelp(void)
{
    printUsageStr();
    fmt::print("Arguments can be specified in any order.\n\n");
    fmt::print("-e=<entry>    - Specifies the desired STIL entry with an HVSC-relative path.\n");
    fmt::print("-l=<HVSC loc> - Specifies the location of the HVSC base directory. If not\n");
    fmt::print("                specified, the value of the HVSC_BASE env. variable will be\n");
    fmt::print("                used. Specifying this option will override HVSC_BASE.\n");
    fmt::print("-t=<tuneNo>   - If specified, only the STIL entry for the given tune number is\n");
    fmt::print("                printed. (Default: 0)\n");
    fmt::print("-f=<field>    - If specified, only the STIL entry for the given field is\n");
    fmt::print("                printed. (Default: all)\n");
    fmt::print("                Valid values for <field> are:\n");
    fmt::print("                all, name, author, title, artist, comment\n");
    fmt::print("-d            - Turns on debug mode for STILView.\n");
    fmt::print("-i            - Enter interactive mode.\n");
    fmt::print("-m            - Demo mode (tests STILView and shows its capabilities).\n");
    fmt::print("-s            - If specified, section-global (per dir/per composer) comments\n");
    fmt::print("                will NOT be printed.\n");
    fmt::print("-b            - If specified, BUG entries will NOT be printed.\n");
    fmt::print("-o            - If specified, STIL entries will NOT be printed.\n");
    fmt::print("-v            - Print STILView's and STIL's version number.\n");
    fmt::print("-h            - Print this help screen (all other options are ignored).\n\n");
    fmt::print("See user manual for further details and for examples.\n\n");
    exit(EXIT_SUCCESS);
}

char *getArgValue(char *argStr)
{
    char *temp = (char *)std::strchr(argStr, '=');

    if (temp == nullptr)
    {
        return nullptr;
    }

    if (*(temp+1) == '\0')
    {
        return nullptr;
    }

    return (temp+1);
}

void processArguments(int argc, char **argv)
{
    for (int i=1; i<argc; i++)
    {
        if (argv[i][0] == '-')
        {
            switch (argv[i][1])
            {
                case 'd':
                case 'D':
                    myStil.STIL_DEBUG = true;
                    break;
                case 'e':
                case 'E':
                    entryStr = getArgValue(argv[i]);
                    if (entryStr == nullptr) {
                        fmt::print(stderr, "ERROR: STIL entry was not specified correctly!\n");
                        printUsage();
                    }
                    break;
                case 'i':
                case 'I':
                    interactive = true;
                    break;
                case 'm':
                case 'M':
                    demo = true;
                    break;
                case 'l':
                case 'L':
                {
                    char *tempLoc = getArgValue(argv[i]);
                    if (tempLoc != nullptr) {
                        hvscLoc = tempLoc;
                    }
                }
                    break;
                case 't':
                case 'T':
                {
                    char *tuneStr = getArgValue(argv[i]);
                    if (tuneStr == nullptr) {
                        fmt::print(stderr, "ERROR: tune number was not specified correctly!\n");
                        printUsage();
                    }
                    sscanf(tuneStr, "%d", &tuneNo);
                }
                    break;
                case 's':
                case 'S':
                    showSection = false;
                    break;
                case 'b':
                case 'B':
                    showBug = false;
                    break;
                case 'o':
                case 'O':
                    showEntry = false;
                    break;
                case 'v':
                case 'V':
                    showVersion = true;
                    break;
                case 'f':
                case 'F':
                {
                    char *fieldStr = getArgValue(argv[i]);
                    if (fieldStr == nullptr) {
                        fmt::print(stderr, "ERROR: field was not specified correctly!\n");
                        printUsage();
                    }
                    if (strEquals(fieldStr, "all")) {
                        field = STIL::all;
                    }
                    else if (strEquals(fieldStr, "name")) {
                        field = STIL::name;
                    }
                    else if (strEquals(fieldStr, "author")) {
                        field = STIL::author;
                    }
                    else if (strEquals(fieldStr, "title")) {
                        field = STIL::title;
                    }
                    else if (strEquals(fieldStr, "artist")) {
                        field = STIL::artist;
                    }
                    else if (strEquals(fieldStr, "comment")) {
                        field = STIL::comment;
                    }
                    else {
                        fmt::print(stderr, "ERROR: Unknown STIL field specified: '{}' !\n", fieldStr);
                        fmt::print(stderr, "Valid values for <field> are:\n");
                        fmt::print(stderr, "all, name, author, title, artist, comment.\n");
                        printUsage();
                    }
                }
                    break;
                case 'h':
                case 'H':
                    printHelp();
                    break;
                default:
                    fmt::print(stderr, "ERROR: Unknown argument: '{}' !\n", argv[i]);
                    printUsage();
                    break;
            }
        }
        else
        {
            fmt::print(stderr, "ERROR: Unknown argument: '{}' !\n", argv[i]);
            printUsage();
        }
    }
}

void checkArguments(void)
{
    if (hvscLoc == nullptr)
    {
        if (interactive || demo)
        {
            hvscLoc = new char[STIL_MAX_PATH_SIZE];
            fmt::print("Enter HVSC base directory: ");
            std::cin.width(STIL_MAX_PATH_SIZE);
            std::cin >> *hvscLoc;
        }
        else
        {
            if (showVersion)
            {
                showBug = false;
                showEntry = false;
                showSection = false;
            }
            else
            {
                fmt::print(stderr, "ERROR: HVSC base dir was not specified and HVSC_BASE is not set, either!\n");
                printUsage();
            }
        }
    }

    if (entryStr == nullptr)
    {
        if ((!interactive) && (!demo))
        {
            if (showVersion) {
                showBug = false;
                showEntry = false;
                showSection = false;
            }
            else
            {
                fmt::print(stderr, "ERROR: STIL entry was not specified!\n");
                printUsage();
            }
        }
        else
        {
            entryStr = STIL_DEMO_ENTRY;
        }
    }
}

int main(int argc, char **argv)
{
    const char *tmpptr, *sectionPtr, *entryPtr, *bugPtr;
    const char *versionPtr;
    float tempval;

    if (argc < 2)
    {
        printHelp();
    }

    hvscLoc = getenv("HVSC_BASE");

    processArguments(argc, argv);

    checkArguments();

    codeConvert cvt;

    if (interactive || demo) {
        fmt::print("Reading STIL...\n");
    }
    else
    {
        if (showVersion && (hvscLoc == nullptr))
        {
            versionPtr = myStil.getVersion();
            if (versionPtr == nullptr)
            {
                fmt::print(stderr, "ERROR: No STIL version string was found!\n");
            }
            else
            {
                fmt::print("{}", versionPtr);
            }

            exit(EXIT_SUCCESS);
        }
    }

    if (myStil.setBaseDir(hvscLoc) != true)
    {
        fmt::print(stderr, "STIL error #{}: {}\n", myStil.getError(), myStil.getErrorStr());
        exit(EXIT_FAILURE);
    }

    if ((!interactive) && (!demo))
    {
        // Pure command-line version.

        if (showVersion)
        {
            versionPtr = myStil.getVersion();
        }
        else
        {
            versionPtr = nullptr;
        }

        if (showSection) {
            sectionPtr = myStil.getGlobalComment(entryStr);
        }
        else
        {
            sectionPtr = nullptr;
        }

        if (showEntry)
        {
            entryPtr = myStil.getEntry(entryStr, tuneNo, field);
        }
        else {
            entryPtr = nullptr;
        }

        if (showBug)
        {
            bugPtr = myStil.getBug(entryStr, tuneNo);
        }
        else {
            bugPtr = nullptr;
        }

        if (versionPtr != nullptr)
        {
            if ((sectionPtr != nullptr) || (entryPtr != nullptr) || (bugPtr != nullptr))
            {
                fmt::print("--- STILView  VERSION ---\n");
            }
            fmt::print("{}", versionPtr);
        }

        if (sectionPtr != nullptr)
        {
            if ((versionPtr != nullptr) || (entryPtr != nullptr) || (bugPtr != nullptr))
            {
                fmt::print("---- GLOBAL  COMMENT ----\n");
            }
            fmt::print("{}", cvt.convert(sectionPtr));
        }

        if (entryPtr != nullptr)
        {
            if ((versionPtr != nullptr) || (sectionPtr != nullptr) || (bugPtr != nullptr))
            {
                fmt::print("------ STIL  ENTRY ------\n");
            }
            fmt::print("{}", cvt.convert(entryPtr));
        }

        if (bugPtr != nullptr) {
            if ((versionPtr != nullptr) || (sectionPtr != nullptr) || (entryPtr != nullptr))
            {
                fmt::print("---------- BUG ----------\n");
            }
            fmt::print("{}", cvt.convert(bugPtr));
        }
    }
    else {

        // We are either in interactive or demo mode here.

        if (demo)
        {
            fmt::print("==== STILVIEW  DEMO MODE ====\n");
            fmt::print("\n---- STIL VERSION ----\n");
            fmt::print("---- ONE STRING ----\n");
        }

        // This gets printed regardless.

        versionPtr = myStil.getVersion();
        if (versionPtr == nullptr)
        {
            fmt::print(stderr, "ERROR: No STIL version string was found!\n");
        }
        else
        {
            fmt::print("{}", versionPtr);
        }

        if (demo)
        {
            // Demo mode.

            fmt::print("---- STIL CLASS VERSION # ----\n");
            tempval = myStil.getVersionNo();
            if (tempval == 0)
            {
                fmt::print(stderr, "ERROR: STILView version number was not found!\n");
            }
            else
            {
                fmt::print("STILView v{}\n", tempval);
            }

            fmt::print("---- STIL.txt VERSION # ----\n");
            tempval = myStil.getSTILVersionNo();
            if (tempval == 0)
            {
                fmt::print(stderr, "ERROR: STIL version number was not found!\n");
            }
            else
            {
                fmt::print("STIL v{}\n", tempval);
            }

            // For testing setBaseDir().

            if (myStil.STIL_DEBUG == true)
            {
                if (myStil.setBaseDir(OTHER_HVSC_BASE_DIR) != true)
                {
                    fmt::print(stderr, "STIL error #{}: {}\n", myStil.getError(), myStil.getErrorStr());
                    fmt::print(stderr, "Couldn't switch to new dir: '{}'\n", OTHER_HVSC_BASE_DIR);
                    fmt::print(stderr, "Reverting back to '{}'\n", hvscLoc);
                }
                else
                {
                    hvscLoc = OTHER_HVSC_BASE_DIR;

                    fmt::print("Switch to new dir '{}' was successful!\n", hvscLoc);

                    fmt::print("---- ONE STRING ----\n");

                    versionPtr = myStil.getVersion();
                    if (versionPtr == nullptr)
                    {
                        fmt::print(stderr, "ERROR: No STIL version string was found!\n");
                    }
                    else
                    {
                        fmt::print("{}", versionPtr);
                    }

                    fmt::print("---- STIL CLASS VERSION # ----\n");
                    tempval = myStil.getVersionNo();
                    if (tempval == 0)
                    {
                        fmt::print(stderr, "ERROR: STILView version number was not found!\n");
                    }
                    else
                    {
                       fmt::print("STILView v{}\n", tempval);
                    }

                    fmt::print("---- STIL.txt VERSION # ----\n");
                    tempval = myStil.getSTILVersionNo();
                    if (tempval == 0)
                    {
                        fmt::print(stderr, "ERROR: STIL version number was not found!\n");
                    }
                    else
                    {
                        fmt::print("STIL v{}\n", tempval);
                    }
                }
            }

            fmt::print("\n==== STIL ABSOLUTE PATH TO {}, Tune #{} ====\n\n", entryStr, tuneNo);

            std::string t(hvscLoc);

            // Chop the trailing slash
            if (t.back() == SLASH)
            {
                t.pop_back();
            }
            t.append(entryStr);

            fmt::print("---- GLOBAL  COMMENT ----\n");

            tmpptr = myStil.getAbsGlobalComment(t.c_str());

            if (tmpptr == nullptr)
            {
                fmt::print(stderr, "STIL error #{}: {}\n", myStil.getError(), myStil.getErrorStr());
            }
            else
            {
                fmt::print("{}", cvt.convert(tmpptr));
            }

            fmt::print("-- TUNE GLOBAL COMMENT --\n");

            tmpptr = myStil.getAbsEntry(t.c_str(), 0, STIL::comment);

            if (tmpptr == nullptr)
            {
                fmt::print(stderr, "STIL error #{}: {}\n", myStil.getError(), myStil.getErrorStr());
            }
            else
            {
                fmt::print("{}", cvt.convert(tmpptr));
            }

            fmt::print("------ STIL  ENTRY ------\n");
            fmt::print("(For tune #1)\n");

            tmpptr = myStil.getAbsEntry(t.c_str(), 1, STIL::all);

            if (tmpptr == nullptr)
            {
                fmt::print(stderr, "STIL error #{}: {}\n", myStil.getError(), myStil.getErrorStr());
            }
            else
            {
                fmt::print("{}", cvt.convert(tmpptr));
            }

            fmt::print("---------- BUG ----------\n");

            tmpptr = myStil.getAbsBug(t.c_str(), tuneNo);

            if (tmpptr == nullptr)
            {
                fmt::print(stderr, "STIL error #{}: {}\n", myStil.getError(), myStil.getErrorStr());
            }
            else
            {
                fmt::print("{}", cvt.convert(tmpptr));
            }

            fmt::print("==== END OF ENTRY ====\n");

            fmt::print("\nTrying to do setBaseDir() to wrong location...\n");

            if (myStil.setBaseDir("This_should_not_work") != true)
            {
                fmt::print("setBaseDir() failed!\n");
                fmt::print("But it should't have an impact on private data!\n");
                fmt::print("You should see the same entry below:\n");
                fmt::print("\n------ STIL  ENTRY ------\n");

                tmpptr = myStil.getAbsEntry(t.c_str(), tuneNo, STIL::all);

                if (tmpptr == nullptr)
                {
                    fmt::print(stderr, "STIL error #{}: {}\n", myStil.getError(), myStil.getErrorStr());
                }
                else
                {
                    fmt::print("{}", cvt.convert(tmpptr));
                }
            }
            else
            {
                fmt::print("Oops, it should've failed!\n");
            }
        }

        if (interactive)
        {
            // Interactive mode.

            fmt::print("\n==== ENTERING INTERACTIVE MODE ====\n\n");

            char temp[STIL_MAX_PATH_SIZE];
            do
            {
                fmt::print("Enter desired entry (relative path) or 'q' to exit.\n");
                fmt::print("Entry: ");
                std::cin.width(STIL_MAX_PATH_SIZE);
                std::cin >> temp;

                if (*temp == '/')
                {
                    fmt::print("Enter tune number (can enter 0, too): ");
                    std::cin >> tuneNo;

                    fmt::print("Field [(A)ll, (N)ame, A(U)thor (T)itle, A(R)tist,(C)omment]: ");
                    char fieldchar;
                    std::cin >> fieldchar;

                    switch (fieldchar)
                    {
                        case 'a':
                        case 'A':
                            field = STIL::all;
                            break;
                        case 'n':
                        case 'N':
                            field = STIL::name;
                            break;
                        case 'u':
                        case 'U':
                            field = STIL::author;
                            break;
                        case 't':
                        case 'T':
                            field = STIL::title;
                            break;
                        case 'r':
                        case 'R':
                            field = STIL::artist;
                            break;
                        case 'c':
                        case 'C':
                            field = STIL::comment;
                            break;
                        default:
                            fmt::print("Wrong field. Assuming (A)ll.\n");
                            field = STIL::all;
                            break;
                    }

                    fmt::print("\n==== {}, Tune #{} ====\n\n", temp, tuneNo);
                    fmt::print("---- GLOBAL  COMMENT ----\n");

                    tmpptr = myStil.getGlobalComment(temp);

                    if (tmpptr)
                    {
                        fmt::print("{}", cvt.convert(tmpptr));
                    }
                    else
                    {
                        fmt::print("NONE!\n");
                    }

                    fmt::print("------ STIL  ENTRY ------\n");

                    tmpptr = myStil.getEntry(temp, tuneNo, field);

                    if (tmpptr)
                    {
                        fmt::print("{}", cvt.convert(tmpptr));
                    }
                    else
                    {
                        fmt::print("NONE!\n");
                    }

                    fmt::print("---------- BUG ----------\n");

                    tmpptr = myStil.getBug(temp, tuneNo);

                    if (tmpptr)
                    {
                        fmt::print("{}", cvt.convert(tmpptr));
                    }
                    else
                    {
                        fmt::print("NONE!\n");
                    }

                    fmt::print("==== END OF ENTRY ====\n\n");
                }
            } while (*temp == '/');

            fmt::print("BYE!\n");
        }
    }

    return EXIT_SUCCESS;
}
