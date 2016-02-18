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

#ifndef _SERIES_MONTHPRICE_HPP_
#define _SERIES_MONTHPRICE_HPP_

#ifdef WIN32
#pragma warning (disable:4290)
#endif

// Boost
#include <boost/date_time/gregorian/gregorian.hpp>


namespace Series
{

  //! EOM price record.
  /*!
  Basic price record used in EOM series.
  \see EOMSeries.
  */
  class MonthPrice
  {
  public:
    boost::gregorian::date key; //! The end of the period, ie the end of the month.

    boost::gregorian::date begin; //! First day in monthly period.
    boost::gregorian::date end; //! Last day in monthly period.
    
    double high; //! High price.
    double low; //! Low price.
    double open; //! Open price.
    double close; //! Close price.
    double adjclose; //! Adjusted closed. This is the adjusted price to account for split and dividends.
    unsigned long volume; //! Day volume.
  };

} // namespace Series

#endif // _SERIES_MONTHPRICE_HPP_
