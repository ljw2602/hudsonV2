/*
 * Copyright (C) 2007,2008 A. Giannetti
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

// STL
#include <iostream>
#include <iomanip>

// Hudson
#include "Price.hpp"
#include "Position.hpp"
#include "EODDB.hpp"

using namespace std;
using namespace boost::gregorian;
using namespace Series;


Position::Position(ID id, const string& symbol):
_id(id),
_symbol(symbol),
_size(0)
{
}


void Position::print(void) const
{
    cout << _symbol << " [Pos ID: " << id() << "]" << ":";
    _sExecutions.print();
    
    if( open() )
        cout << " (" << Series::EODDB::instance().get(_symbol).last().close << ") ";
    
    cout << " - " << "Factor " << factor() << " (" << (factor()-1)*100 << "%)";
}


boost::gregorian::date_period Position::hold_period( void ) const throw(PositionException)
{
    if( _sExecutions.empty() )
        throw PositionException("No executions");
    
    return (closed() ?
            boost::gregorian::date_period(_sExecutions.first_by_date()->dt(), _sExecutions.last_by_date()->dt()) :
            boost::gregorian::date_period(_sExecutions.first_by_date()->dt(), Series::EODDB::instance().get(_symbol).rbegin()->first));
}
