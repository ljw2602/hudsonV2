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
#include "SeriesFactorSet.hpp"


SeriesFactorSet::SeriesFactorSet( void ):
  _factor(1)
{
}


bool SeriesFactorSet::insert( const SeriesFactor& sf )
{
  if( ! __SeriesFactorSet::insert(sf).second )
    return false;

  // Cache new overall factor for performance
  _factor *= sf.factor();

  return true;
}


double SeriesFactorSet::factor( void ) const throw(SeriesFactorSetException)
{
  if( empty() )
    throw SeriesFactorSetException("Invalid factor for empty set");

  return _factor;
}
