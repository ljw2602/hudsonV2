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

#include "StdAfx.h"

// Hudson
#include "EODDB.hpp"
#include "EODSeries.hpp"


std::auto_ptr<Series::EODDB> Series::EODDB::_pInstance;


Series::EODDB& Series::EODDB::instance(void)
{
    if( _pInstance.get() == 0 )
        _pInstance.reset(new EODDB);
    
    return *_pInstance;
}


void Series::EODDB::load(const std::string& name, const std::string& filename, DriverType dt,
                         const boost::gregorian::date& begin, const boost::gregorian::date& end) throw(EODDBException)
{
    // Verify this series aren't loaded yet
    if( _sDB.find(name) != _sDB.end() )
        throw EODDBException("Series already loaded");
    
    // Select driver
    FileDriver* pFD = NULL;
    switch( dt ) {
        case YAHOO:
            pFD = &_yd;
            break;
            
        case DMYC:
            pFD = &_dmycd;
            break;
            
        case EQUITYBENCHMARK:
            pFD = &_ebd;
            break;
            
        case TREASURYBENCHMARK:
            pFD = &_tbd;
            break;
            
        default:
            throw EODDBException("Unknown driver");
    }
    
    // Load new series
    EODSeries* pSeries = new EODSeries(name);
    pSeries->load(*pFD, filename, begin, end);
    
    _sDB.insert(DB::value_type(name, pSeries));
}


const Series::EODSeries& Series::EODDB::get(const std::string& name) const throw(EODDBException)
{
    DB::const_iterator iter;
    if( (iter = _sDB.find(name)) == _sDB.end() )
        throw EODDBException("Unknown series");
    
    return *(iter->second);
}
