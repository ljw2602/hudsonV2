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

#ifndef _TREASURYBENCHMARKDRIVER_HPP_
#define _TREASURYBENCHMARKDRIVER_HPP_

#ifdef WIN32
#pragma warning (disable:4290)
#endif

// STL
#include <fstream>
#include <string>

// Boost
#include <boost/date_time/gregorian/gregorian.hpp>

// Series
#include "FileDriver.hpp"
#include "DayPrice.hpp"

namespace Series
{
    
    class TreasuryBenchmarkDriver: public FileDriver
    {
    public:
        TreasuryBenchmarkDriver(void);
        ~TreasuryBenchmarkDriver(void);
        
        virtual bool open(const std::string& filename);
        virtual void close(void);
        virtual bool next(DayPrice& dp) throw(DriverException);
        virtual bool eof(void);
        
    private:
        enum FIELDS_POS {
            DATE = 0,
            DGS1MO,
            DGS3MO,
            DGS6MO,
            DGS1,
            DGS2,
            DGS3,
            DGS5,
            DGS7,
            DGS10,
            DGS20,
            DGS30
        };
        
    private:
        std::ifstream _infile;
        std::string _line;
        unsigned _linenum;
    };
    
}

#endif // _TREASURYBENCHMARKDRIVER_HPP_
