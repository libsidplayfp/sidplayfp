/*
 *  Copyright (C) 2014 Leandro Nini
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

#ifndef SID_FSTREAM_H
#define SID_FSTREAM_H

#include <iostream>

#ifdef _WIN32

#  include <windows.h>

#  ifdef __GLIBCXX__
#    include <ext/stdio_filebuf.h>

class sid_ifstream : public std::ifstream
{
private:
    __gnu_cxx::stdio_filebuf<char>  buffer;
    std::basic_streambuf<char>     *p_original_buf;

public:
    sid_ifstream(const TCHAR* filename, ios_base::openmode mode = ios_base::in) :
        std::ifstream(),
        buffer(_wfopen(filename, TEXT("r")), mode),
        p_original_buf(basic_ios::rdbuf(&buffer))
    {}

    ~sid_ifstream()
    {
        basic_ios::rdbuf(p_original_buf);
        buffer.close();
    }
};

class sid_ofstream : public std::ofstream
{
private:
    __gnu_cxx::stdio_filebuf<char>  buffer;
    std::basic_streambuf<char>     *p_original_buf;

public:
    sid_ofstream(const TCHAR* filename, ios_base::openmode mode = ios_base::out) :
        std::ofstream(),
        buffer(_wfopen(filename, TEXT("w")), mode),
        p_original_buf(basic_ios::rdbuf(&buffer))
    {}

    ~sid_ofstream()
    {
        basic_ios::rdbuf(p_original_buf);
        buffer.close();
    }
};

class sid_wifstream : public std::wifstream
{
private:
    __gnu_cxx::stdio_filebuf<TCHAR>  buffer;
    std::basic_streambuf<TCHAR>     *p_original_buf;

public:
    sid_wifstream(const TCHAR* filename, ios_base::openmode mode = ios_base::in) :
        std::wifstream(),
        buffer(_wfopen(filename, TEXT("r")), mode),
        p_original_buf(basic_ios::rdbuf(&buffer))
    {}

    ~sid_wifstream()
    {
        basic_ios::rdbuf(p_original_buf);
        buffer.close();
    }
};

class sid_wofstream : public std::wofstream
{
private:
    __gnu_cxx::stdio_filebuf<TCHAR>  buffer;
    std::basic_streambuf<TCHAR>     *p_original_buf;

public:
    sid_wofstream(const TCHAR* filename, ios_base::openmode mode = ios_base::out) :
        std::wofstream(),
        buffer(_wfopen(filename, TEXT("w")), mode),
        p_original_buf(basic_ios::rdbuf(&buffer))
    {}

    ~sid_wofstream()
    {
        basic_ios::rdbuf(p_original_buf);
        buffer.close();
    }
};

#  else // _MSC_VER
#    define sid_wifstream std::wifstream
#    define sid_wofstream std::wofstream
#    define sid_ifstream std::ifstream
#    define sid_ofstream std::ofstream
#  endif // __GLIBCXX__

#else
#  define sid_wifstream std::ifstream
#  define sid_wofstream std::ofstream
#  define sid_ifstream std::ifstream
#  define sid_ofstream std::ofstream
#endif // _WIN32

#endif // SID_FSTREAM_H
