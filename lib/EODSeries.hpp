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

#ifndef _SERIES_EODSERIES_HPP_
#define _SERIES_EODSERIES_HPP_

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
#include "DayPrice.hpp"
#include "FileDriver.hpp"
#include "EOWSeries.hpp"
#include "EOMSeries.hpp"


namespace Series
{
  class EODSeriesException: public std::exception
  {
  public:
    EODSeriesException(const std::string& msg):
      _Str("EODSeriesException: ")
    {
      _Str += msg;
    }

    virtual ~EODSeriesException(void) throw() { }
    virtual const char *what(void) const throw() { return _Str.c_str(); }
    
  protected:
    std::string _Str;
  };

  /*!
    EODSeries is an STL map collection indexed by date. The collection is meant to store
    historical EOD data ordered by time.
  */
  class EODSeries: public std::map<boost::gregorian::date, Series::DayPrice>
  {
    typedef std::map<boost::gregorian::date, DayPrice> ThisMap;

  public:
    /*!
      Build a new data series.
      \param name The name of this EOD series collection.
    */
    EODSeries(const std::string& name);

    //! Returns data series name.
    std::string name(void) const { return _name; }
    
    //! Returns true if data was loaded from a file. False otherwise.
    bool isLoaded(void) const { return _isLoaded; }

    /*!
      Load data series from a file using a specific driver.
      \param driver The parser that will be called to read lines from the file.
      \param filename The path to the file containing the data series.
      \see YahooDriver
      \see DMYCloseDriver
    */
    std::size_t load(FileDriver& driver, const std::string& filename); // load all records returned by the driver
    
    /*!
      Load data series from a file for a specific time period.
      \param driver The parser that will be called to read lines from the file.
      \param filename The path to the file containing the data series.
      \param begin Beginning of requested EOD series.
      \param end End of requested EOD series.
      \see YahooDriver
      \see DMYCloseDriver
    */
    std::size_t load(FileDriver& driver, const std::string& filename, const boost::gregorian::date& begin, const boost::gregorian::date& end); // load date range

    //! Returns the loaded period.
    boost::gregorian::date_period period(void) const throw(EODSeriesException);

    //! Returns the loaded period in date_duration format.
    boost::gregorian::date_duration duration(void) const throw(EODSeriesException);

    //! Returns the loaded period in days.
    long days(void) const;

    //! Returns weekly series.
    /*!
      This function should be called after loading EOD daily series. The method will recalculate H/L/O/C values for the week.
      \see DayPrice.
    */
    EOWSeries weekly(void) const;
    
    //! Returns monthly series.
    /*!
      This function should be called after loading EOD daily or weekly series. The method will recalculate all H/L/O/C values
      for the month.
      \see DayPrice.
    */  
    EOMSeries monthly(void) const;

    //! Extract all open prices from current loaded series preserving the original time order.
    std::vector<double> open(void) const;
    
    //! Extract all close prices from current loaded series preserving the original time order.
    std::vector<double> close(void) const;
    
    //! Extract all adjusted close prices from current loaded series preserving the original time order.
    std::vector<double> adjclose(void) const;
    
    //! Extract all high prices from current loaded series preserving the original time order.
    std::vector<double> high(void) const;
    
    //! Extract all low prices from current loaded series preserving the original time order.
    std::vector<double> low(void) const;
    
    //! Extract all volume values from current loaded series preserving the original time order.
    std::vector<double> volume(void) const;

    //! Extract all open prices from iter included backwards num elements
    /*!
      \param iter An iterator that points to the first item in the series that should be extracted.
      \param num The number of items that should be extracted in reverse order.
    */
    std::vector<double> open(const_iterator iter, unsigned long num) const;
    
    //! Extract all close prices from iter included backwards num elements
    /*!
      \param iter An iterator that points to the first item in the series that should be extracted.
      \param num The number of items that should be extracted in reverse order.
    */
    std::vector<double> close(const_iterator iter, unsigned long num) const;
    
    //! Extract all adjusted close prices from iter included backwards num elements
    /*!
      \param iter An iterator that points to the first item in the series that should be extracted.
      \param num The number of items that should be extracted in reverse order.
    */
    std::vector<double> adjclose(const_iterator iter, unsigned long num) const;
    
    //! Extract all high prices from iter included backwards num elements
    /*!
      \param iter An iterator that points to the first item in the series that should be extracted.
      \param num The number of items that should be extracted in reverse order.
    */
    std::vector<double> high(const_iterator iter, unsigned long num) const;
    
    //! Extract all low prices from iter included backwards num elements
    /*!
      \param iter An iterator that points to the first item in the series that should be extracted.
      \param num The number of items that should be extracted in reverse order.
    */
    std::vector<double> low(const_iterator iter, unsigned long num) const;
    
    //! Extract all volume values from iter included backwards num elements
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
    
    //! Returns the first data series entry for the specified year/month.
    /*!
      \param year The selected year.
      \param month The selected month.
    */
    ThisMap::const_iterator first_in_month(boost::gregorian::greg_year year, boost::gregorian::greg_month month) const;

    //! Returns the last data series entry for the specified year/month.
    /*!
      \param year The selected year.
      \param month The selected month.
    */    
    ThisMap::const_iterator last_in_month(boost::gregorian::greg_year year, boost::gregorian::greg_month month) const;
    
    //! Returns the last data series entry of the week for the specified year/month/day.
    /*!
      The returned iterator could point to a Monday or whatever first day of the week is available in the data series.
      \param year The selected year.
      \param month The selected month.
      \param day The selected day. This parameter effectively selects a specific week within the year/month specified by the other two parameters.
    */
    ThisMap::const_iterator first_in_week(boost::gregorian::greg_year year, boost::gregorian::greg_month month, boost::gregorian::greg_day day) const;

    //! Returns the last data series entry of the week for the specified year/month/day.
    /*!
      The returned iterator could point to a Friday or whatever last day of the week is available in the data series.
      \param year The selected year.
      \param month The selected month.
      \param day The selected day. This parameter effectively selects a specific week within the year/month specified by the other two parameters.
    */    
    ThisMap::const_iterator last_in_week(boost::gregorian::greg_year year, boost::gregorian::greg_month month, boost::gregorian::greg_day day) const;

    //! Return last EOD record in series.
    Series::DayPrice last(void) const { return (*rbegin()).second; }

  private:
    std::string _name;
    bool _isLoaded;
  };

} // namespace Series

#endif // _SERIES_EODSERIES_HPP_
