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

// STD
#include <sstream>

// Hudson
#include "Price.hpp"

using namespace std;
using namespace Series;


Price Price::get( const std::string& symbol, const boost::gregorian::date& dt, Series::EODDB::PriceType pt ) throw(PriceException)
{
  // Retrieve price type pt
  Series::EODSeries::const_iterator citer = Series::EODDB::instance().get(symbol).find(dt);
  if( citer == Series::EODDB::instance().get(symbol).end() ) {
    stringstream ss;
    ss << "Can't find " << dt << " price record in " << symbol << " series";
    throw PriceException(ss.str());
  }

  double priceval = 0;
  switch( pt ) {

    case EODDB::OPEN:
      priceval = citer->second.open;
      break;

    case EODDB::CLOSE:
      priceval = citer->second.close;
      break;

    case EODDB::ADJCLOSE:
      priceval = citer->second.adjclose;
      break;

    default:
      throw PriceException("Invalid price type");
  }

  return Price(priceval);
}


Price Price::last( const std::string& symbol, Series::EODDB::PriceType pt ) throw(PriceException)
{
  if( Series::EODDB::instance().get(symbol).empty() )
    throw PriceException("Empty series database");

  Series::DayPrice item = Series::EODDB::instance().get(symbol).last();

  double priceval = 0;
  switch ( pt ) {

    case EODDB::OPEN:
      priceval = item.open;

    case EODDB::CLOSE:
      priceval = item.close;

    case EODDB::ADJCLOSE:
      priceval = item.adjclose;

    default:
      throw PriceException("Invalid price type");
  }

  return Price(priceval);
}


Price::Price(double value):
  _value(value)
{
}


Price::Price(const Price& src)
{
  _value = src.value();
}


Price& Price::operator=(const Price& src)
{
  if( this != &src )
    _value = src.value();

  return *this;
}


double Price::value( void ) const throw(PriceException)
{
  if( _value <= 0 )
    throw PriceException("Invalid price");

  return _value;
}
