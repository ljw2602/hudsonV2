/*
 * Copyright (C) 2007, Alberto Giannetti
 *
 * This file is part of Hudson.
 *
 * Hudson is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Hudson is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Hudson.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _SERIES_FILEDRIVER_HPP_
#define _SERIES_FILEDRIVER_HPP_

#ifdef WIN32
#pragma warning (disable:4290)
#endif

// STL
#include <string>
#include <stdexcept>

// Hudson
#include "DayPrice.hpp"


namespace Series
{
    
    class DriverException: public std::exception
    {
    public:
        DriverException(const std::string& msg):
        _Str("DriverException: ")
        {
            _Str += msg;
        }
        
        virtual ~DriverException(void) throw() { }
        virtual const char *what() const throw() { return _Str.c_str(); }
        
    private:
        std::string _Str;
    };
    
    /*!
     A simple driver interface to parse EOD data from a file.
     */
    class FileDriver
    {
    public:
        virtual ~FileDriver(void) { }
        
        //! Open a file for parsing.
        /*!
         \param filename File path to parse.
         */
        virtual bool open(const std::string& filename) = 0;
        //! Close file.
        virtual void close(void) = 0;
        //! Read the next EOD record from the file.
        virtual bool next(DayPrice& record) throw(DriverException) = 0;
        //! EOF check.
        virtual bool eof(void) = 0;
    };
    
} // namespace Series

#endif // _SERIES_FILEDRIVER_HPP_
