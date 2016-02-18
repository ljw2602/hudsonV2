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

#ifndef _SERIES_EOWSERIES_HPP_
#define _SERIES_EOWSERIES_HPP_

#ifdef WIN32
#pragma warning (disable:4290)
#endif

// STL
#include <iostream>
#include <map>
#include <string>
#include <vector>

// Boost
#include <boost/date_time/gregorian/gregorian.hpp>

// Series
#include "WeekPrice.hpp"


namespace Series
{

/*!
  EOWSeries is an STL map collection indexed by date.
*/
class EOWSeries: public std::map<boost::gregorian::date, Series::WeekPrice>
{
  typedef std::map<boost::gregorian::date, WeekPrice> ThisMap;

public:
  /*!
  Build a new data series.
  \param name The name of this EOD series collection.
  */
  EOWSeries(const std::string& name);

  //! Returns data series name.
  std::string name(void) const { return _name; }

  //! Return series period.
  boost::gregorian::date_period period(void) const;
  //! Return series period in date_duration.
  boost::gregorian::date_duration duration(void) const;
  //! Return series period in days.
  long days(void) const;

  //! Extract all open prices from series preserving original time order.
  std::vector<double> open(void) const;

  //! Extract all close prices from series preserving original time order.
  std::vector<double> close(void) const;

  //! Extract all adjusted close prices from series preserving original time order.
  std::vector<double> adjclose(void) const;

  //! Extract all high prices from series preserving original time order.
  std::vector<double> high(void) const;

  //! Extract all low prices from series preserving original time order.
  std::vector<double> low(void) const;

  //! Extract all volume values from series preserving original time order.
  std::vector<double> volume(void) const;

  //! Extract all open prices starting from iter backwards num elements.
  /*!
  \param iter An iterator that points to the first item in the series that should be extracted.
  \param num The number of items that should be extracted in reverse order.
  */
  std::vector<double> open(const_iterator iter, unsigned long num) const;

  //! Extract all close prices starting from iter backwards num elements.
  /*!
  \param iter An iterator that points to the first item in the series that should be extracted.
  \param num The number of items that should be extracted in reverse order.
  */
  std::vector<double> close(const_iterator iter, unsigned long num) const;

  //! Extract all adjusted close prices starting from iter backwards num elements.
  /*!
  \param iter An iterator that points to the first item in the series that should be extracted.
  \param num The number of items that should be extracted in reverse order.
  */
  std::vector<double> adjclose(const_iterator iter, unsigned long num) const;

  //! Extract all high prices starting from iter backwards num elements.
  /*!
  \param iter An iterator that points to the first item in the series that should be extracted.
  \param num The number of items that should be extracted in reverse order.
  */
  std::vector<double> high(const_iterator iter, unsigned long num) const;

  //! Extract all low prices starting from iter backwards num elements.
  /*!
  \param iter An iterator that points to the first item in the series that should be extracted.
  \param num The number of items that should be extracted in reverse order.
  */
  std::vector<double> low(const_iterator iter, unsigned long num) const;

  //! Extract all volume values starting from iter backwards num elements.
  /*!
  \param iter An iterator that points to the first item in the series that should be extracted.
  \param num The number of items that should be extracted in reverse order.
  */
  std::vector<double> volume(const_iterator iter, unsigned long num) const;

  //! Extract all open price from (itbegin to itend]
  std::vector<double> open(const_iterator itbegin, const_iterator itend) const;

  //! Extract all close price from (itbegin to itend]   
  std::vector<double> close(const_iterator itbegin, const_iterator itend) const;

  //! Extract all adjclose price from (itbegin to itend]  
  std::vector<double> adjclose(const_iterator itbegin, const_iterator itend) const;

  //! Extract all high price from (itbegin to itend]
  std::vector<double> high(const_iterator itbegin, const_iterator itend) const;

  //! Extract all low price from (itbegin to itend]
  std::vector<double> low(const_iterator itbegin, const_iterator itend) const;

  //! Extract all volume values from (itbegin to itend]
  std::vector<double> volume(const_iterator itbegin, const_iterator itend) const;

  //! Returns an iterator to the data item at or (if not found) before the specified date.
  /*!
  \param k The selected date. This should be included in the data series period. If the date is not included in
  the period, end() is returned.
  \see before().
  */
  ThisMap::const_iterator at_or_before(const boost::gregorian::date& k) const;

  //! Returns an iterator to recs items before a specific date.
  /*!
  \param k The reference date.
  \param recs The number of items from the reference date pointing to the wanted entry.
  */
  ThisMap::const_iterator before(const boost::gregorian::date& k, unsigned recs = 1) const;

  //! Returns an iterator to recs items after a specific date.
  /*!
  \param k The reference date.
  \param recs The number of items from the reference date pointing to the wanted entry.
  */    
  ThisMap::const_iterator after(const boost::gregorian::date& k, unsigned recs = 1) const;

  //! Return last EOW record in series.
  Series::WeekPrice last(void) const { return (*rbegin()).second; }

private:
  std::string _name;
};

} // namespace Series

#endif // _SERIES_EOWSERIES_HPP_
