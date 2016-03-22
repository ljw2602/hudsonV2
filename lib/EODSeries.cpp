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
#include "EODSeries.hpp"
#include "EOWSeries.hpp"
#include "EOMSeries.hpp"

using namespace std;
using namespace boost::gregorian;


Series::EODSeries::EODSeries(const std::string& name):
_name(name),
_isLoaded(false)
{
}


size_t Series::EODSeries::load(FileDriver& driver, const std::string& filename)
{
    ThisMap::clear();
    
    if( !driver.open(filename) )
        return 0;
    
    DayPrice rec;
    while( !driver.eof() ) {
        
        try {
            
            if( driver.next(rec) == false )
                continue;
            
            if( ThisMap::insert(ThisMap::value_type(rec.key, rec)).second == false ) {
                cerr << "Duplicate record " << rec.key << endl;
                continue;
            }
            
        } catch( DriverException& e ) {
            cerr << e.what() << endl;
            continue;
        }
        
    }	// while not EOF
    
    driver.close();
    
    _isLoaded = true;
    
    return std::map<boost::gregorian::date, DayPrice>::size();
}


size_t Series::EODSeries::load(FileDriver& driver, const std::string& filename, const boost::gregorian::date& begin, const boost::gregorian::date& end)
{
    ThisMap::clear();
    
    if( !driver.open(filename) )
        return 0;
    
    DayPrice rec;
    while( !driver.eof() ) {
        
        try {
            
            if( driver.next(rec) == false ) // EOF
                continue;
            
            if( rec.key < begin || rec.key > end )
                continue;					// out of range
            
            if( ThisMap::insert(ThisMap::value_type(rec.key, rec)).second == false ) {
                cerr << "Duplicate record " << rec.key << endl;
                continue;
            }
            
        } catch( DriverException& e ) {
            cerr << e.what() << endl;
            continue;
        }
    }	// while not EOF
    
    driver.close();
    
    _isLoaded = true;
    
    return ThisMap::size();
}


boost::gregorian::date_period Series::EODSeries::period(void) const throw(EODSeriesException)
{
    if( empty() )
        throw EODSeriesException("Null series");
    
    return boost::gregorian::date_period(ThisMap::begin()->first, ThisMap::rbegin()->first);
}


boost::gregorian::date_duration Series::EODSeries::duration(void) const throw(EODSeriesException)
{
    if( empty() )
        throw EODSeriesException("Null series");
    
    return boost::gregorian::date_duration(ThisMap::rbegin()->first - ThisMap::begin()->first);
}


long Series::EODSeries::days(void) const
{
    if( empty() ) return 0;
    return boost::gregorian::date_duration(ThisMap::rbegin()->first - ThisMap::begin()->first).days();
}


Series::EODSeries::ThisMap::const_iterator Series::EODSeries::at_or_before(const boost::gregorian::date& k) const
{
    ThisMap::const_iterator iter;
    if( (iter = ThisMap::find(k)) != ThisMap::end() )
        return iter;
    
    return before(k, 1);
}


Series::EODSeries::ThisMap::const_iterator Series::EODSeries::before(const boost::gregorian::date& k, unsigned recs) const
{
    ThisMap::const_iterator iter;
    if( (iter = ThisMap::lower_bound(k)) == ThisMap::begin() && recs > 0 )
        return ThisMap::end();
    
    for( unsigned i = 0; i < recs; i++ )
        if( --iter == ThisMap::begin() )
            return ThisMap::end();
    
    return iter;
}


Series::EODSeries::ThisMap::const_iterator Series::EODSeries::after(const boost::gregorian::date& k, unsigned recs) const
{
    ThisMap::const_iterator iter;
    if( (iter = ThisMap::find(k)) == ThisMap::end() ) {
        if( (iter = ThisMap::upper_bound(k)) == ThisMap::end() ) {
            return ThisMap::end();				// k out of range
        } else {
            // returning from upper_bound(), we are already one record past the key
            if( recs ) --recs;
        }
    }
    
    for( unsigned i = 0; i < recs; i++ )
        if( ++iter == ThisMap::end() )
            return ThisMap::end();
    
    return iter;
}


Series::EODSeries::ThisMap::const_iterator Series::EODSeries::first_in_month(boost::gregorian::greg_year year, boost::gregorian::greg_month month) const
{
    // Call upper_bound() using the last calendar day for previous month to retrieve first record in requested month
    boost::gregorian::date request_date(year, month, 1); // first calendar day of requested month
    boost::gregorian::date key_date = request_date - boost::gregorian::date_duration(1); // last calendar day of previous month
    ThisMap::const_iterator iter = ThisMap::upper_bound(key_date); // first price record after last calendar day of prev month
    if( iter == ThisMap::end() )
        return iter;
    
    if( iter->first.month() != month ) // we're over the requested month
        return ThisMap::end();
    
    return iter;
}


Series::EODSeries::ThisMap::const_iterator Series::EODSeries::last_in_month(boost::gregorian::greg_year year, boost::gregorian::greg_month month) const
{
    boost::gregorian::date request_date(year, month, 1);
    ThisMap::const_iterator iter = ThisMap::lower_bound(request_date);
    if( iter == ThisMap::end() )
        return iter;
    
    // Step-in until we reach next month bar
    while( iter != end() && iter->first.month() == month )
        ++iter;
    
    // Once we've reached next month first bar, go back one bar to return previous month last bar
    return --iter;
}


Series::EODSeries::ThisMap::const_iterator Series::EODSeries::first_in_week(boost::gregorian::greg_year year, boost::gregorian::greg_month month, boost::gregorian::greg_day day) const
{
    boost::gregorian::date request_date(year, month, day);
    boost::gregorian::date begin_of_week;	// Monday in requested date week
    
    // Look for previous Monday in requested week
    begin_of_week = ( request_date.day_of_week() == boost::gregorian::Monday ) ? request_date :
    boost::gregorian::first_day_of_the_week_before(boost::gregorian::Monday).get_date(request_date); // first Monday before request_date
    
    // Look for first available day in price database
    ThisMap::const_iterator iter = ThisMap::lower_bound(begin_of_week);
    if( iter == ThisMap::end() )
        return iter;
    
    // Make sure we're on the same week than requested date
    if( iter->first.week_number() == request_date.week_number() )
        return iter;
    
    // Different week, sorry no BOW for this request date
    return ThisMap::end();
}


Series::EODSeries::ThisMap::const_iterator Series::EODSeries::last_in_week(boost::gregorian::greg_year year, boost::gregorian::greg_month month, boost::gregorian::greg_day day) const
{
    boost::gregorian::date request_date(year, month, day);
    boost::gregorian::date end_of_week;
    
    // Look for first Friday starting from requested date (included)
    end_of_week = ( request_date.day_of_week() == boost::gregorian::Friday ) ? request_date :
    boost::gregorian::first_day_of_the_week_after(boost::gregorian::Friday).get_date(request_date);	// first Friday of request_date
    
    // Look for this Friday in price database
    ThisMap::const_iterator iter = ThisMap::lower_bound(end_of_week);
    if( iter == ThisMap::end() )
        return iter;
    
    // lower_bound() returns next week first record if Friday can't be found
    if( iter->first.day_of_week() == boost::gregorian::Friday )
        return iter;
    
    // We're on the next week. Go back one record to locate requested EOW
    --iter;
    if( iter->first.week_number() == request_date.week_number() )
        return iter;
    
    // Can't find EOW
    return ThisMap::end();
}


std::vector<double> Series::EODSeries::open( const_iterator iter, unsigned long num ) const
{
    vector<double> v;
    
    if( !_isLoaded || iter == end() )
        return v;
    
    // reverse iterator init skips the first element in collection. We must manually insert the current element.
    v.insert(v.begin(), iter->second.open);
    unsigned i = 1;
    for( const_reverse_iterator rev_iter(iter); i < num && rev_iter != rend(); ++rev_iter, ++i )
        v.insert(v.begin(), rev_iter->second.open);
    
    return v;
}


std::vector<double> Series::EODSeries::open( void ) const
{
    vector<double> v;
    
    for( const_iterator iter(begin()); iter != end(); ++iter )
        v.push_back(iter->second.open);
    
    return v;
}


std::vector<double> Series::EODSeries::open( const_iterator itbegin, const_iterator itend ) const
{
    vector<double> v;
    
    if( itbegin == itend || itbegin == end() )
        return v;
    
    for( const_iterator iter(itbegin); iter != itend; ++iter )
        v.push_back(iter->second.open);
    
    return v;
}


std::vector<double> Series::EODSeries::close( const_iterator iter, unsigned long num ) const
{
    vector<double> v;
    
    if( iter == end() )
        return v;
    
    // reverse iterator init skips the first element in collection. We must manually insert the current element.
    v.insert(v.begin(), iter->second.close);
    unsigned i = 1;
    for( const_reverse_iterator rev_iter(iter); i < num && rev_iter != rend(); ++rev_iter, ++i )
        v.insert(v.begin(), rev_iter->second.close);
    
    return v;
}


std::vector<double> Series::EODSeries::close( void ) const
{
    vector<double> v;
    
    for( const_iterator iter(begin()); iter != end(); ++iter )
        v.push_back(iter->second.close);
    
    return v;
}


std::vector<double> Series::EODSeries::close( const_iterator itbegin, const_iterator itend ) const
{
    vector<double> v;
    
    if( itbegin == itend || itbegin == end() )
        return v;
    
    for( const_iterator iter(itbegin); iter != itend; ++iter )
        v.push_back(iter->second.close);
    
    return v;
}


std::vector<double> Series::EODSeries::adjclose( const_iterator iter, unsigned long num ) const
{
    vector<double> v;
    
    if( iter == end() )
        return v;
    
    // reverse iterator init skips the first element in collection. We must manually insert the current element.
    v.insert(v.begin(), iter->second.adjclose);
    unsigned i = 1;
    for( const_reverse_iterator rev_iter(iter); i < num && rev_iter != rend(); ++rev_iter, ++i )
        v.insert(v.begin(), rev_iter->second.adjclose);
    
    return v;
}


std::vector<double> Series::EODSeries::adjclose( void ) const
{
    vector<double> v;
    
    for( const_iterator iter(begin()); iter != end(); ++iter )
        v.push_back(iter->second.adjclose);
    
    return v;
}


std::vector<double> Series::EODSeries::adjclose( const_iterator itbegin, const_iterator itend ) const
{
    vector<double> v;
    
    if( itbegin == itend || itbegin == end() )
        return v;
    
    for( const_iterator iter(itbegin); iter != itend; ++iter )
        v.push_back(iter->second.adjclose);
    
    return v;
}


std::vector<double> Series::EODSeries::high( const_iterator iter, unsigned long num ) const
{
    vector<double> v;
    
    if( iter == end() )
        return v;
    
    // reverse iterator init skips the first element in collection. We must manually insert the current element.
    v.insert(v.begin(), iter->second.high);
    unsigned i = 1;
    for( const_reverse_iterator rev_iter(iter); i < num && rev_iter != rend(); ++rev_iter, ++i )
        v.insert(v.begin(), rev_iter->second.high);
    
    return v;
}


std::vector<double> Series::EODSeries::high( void ) const
{
    vector<double> v;
    
    for( const_iterator iter(begin()); iter != end(); ++iter )
        v.push_back(iter->second.high);
    
    return v;
}


std::vector<double> Series::EODSeries::high( const_iterator itbegin, const_iterator itend ) const
{
    vector<double> v;
    
    if( itbegin == itend || itbegin == end() )
        return v;
    
    for( const_iterator iter(itbegin); iter != itend; ++iter )
        v.push_back(iter->second.high);
    
    return v;
}


std::vector<double> Series::EODSeries::low( const_iterator iter, unsigned long num ) const
{
    vector<double> v;
    
    if( iter == end() )
        return v;
    
    // reverse iterator init skips the first element in collection. We must manually insert the current element.
    v.insert(v.begin(), iter->second.low);
    unsigned i = 1;
    for( const_reverse_iterator rev_iter(iter); i < num && rev_iter != rend(); ++rev_iter, ++i )
        v.insert(v.begin(), rev_iter->second.low);
    
    return v;
}


std::vector<double> Series::EODSeries::low( void ) const
{
    vector<double> v;
    
    for( const_iterator iter(begin()); iter != end(); ++iter )
        v.push_back(iter->second.low);
    
    return v;
}


std::vector<double> Series::EODSeries::low( const_iterator itbegin, const_iterator itend ) const
{
    vector<double> v;
    
    if( itbegin == itend || itbegin == end() )
        return v;
    
    for( const_iterator iter(itbegin); iter != itend; ++iter )
        v.push_back(iter->second.low);
    
    return v;
}


std::vector<double> Series::EODSeries::volume( const_iterator iter, unsigned long num ) const
{
    vector<double> v;
    
    if( iter == end() )
        return v;
    
    // reverse iterator init skips the first element in collection. We must manually insert the current element.
    v.insert(v.begin(), iter->second.volume);
    unsigned i = 1;
    for( const_reverse_iterator rev_iter(iter); i < num && rev_iter != rend(); ++rev_iter, ++i )
        v.insert(v.begin(), rev_iter->second.volume);
    
    return v;
}


std::vector<double> Series::EODSeries::volume( void ) const
{
    vector<double> v;
    
    for( const_iterator iter(begin()); iter != end(); ++iter )
        v.push_back(iter->second.volume);
    
    return v;
}


std::vector<double> Series::EODSeries::volume( const_iterator itbegin, const_iterator itend ) const
{
    vector<double> v;
    
    if( itbegin == itend || itbegin == end() )
        return v;
    
    for( const_iterator iter(itbegin); iter != itend; ++iter )
        v.push_back(iter->second.volume);
    
    return v;
}


Series::EOWSeries Series::EODSeries::weekly( void ) const
{
    EOWSeries weekly_series(name()); // empty series
    
    // Iterate through all the weeks in db
    for( week_iterator witer(begin()->first); (*witer) <= rbegin()->first; ++witer ) {
        
        // Find first and last entry for this week
        const_iterator first_in_week_iter = first_in_week((*witer).year(), (*witer).month(), (*witer).day());
        if( first_in_week_iter == end() ) {
            cerr << "Warning: can't find series bar (BOW) for week of " << (*witer) << " in " << _name << endl;
            continue;
        }
        
        // Last entry for this week
        const_iterator last_in_week_iter = last_in_week((*witer).year(), (*witer).month(), (*witer).day());
        if( last_in_week_iter == end() ) {
            cerr << "Warning: can't find series bar (EOW) for week of " << (*witer) << " in " << _name << endl;
            continue;
        }
        
        // Initialize this weekly series (O,L,H,C,V)
        WeekPrice dp;
        dp.key = last_in_week_iter->first; // Key is EOW
        dp.begin = first_in_week_iter->first;
        dp.end = last_in_week_iter->first;
        dp.open = first_in_week_iter->second.open; // Open on first day of the week
        dp.close = last_in_week_iter->second.close; // Close on last day of the week
        dp.adjclose = last_in_week_iter->second.adjclose; // Adj. close on last day of the week
        dp.high = 0;
        dp.low = 0;
        dp.volume = 0;
        
        // Store all highs, lows, volume to determine weekly values
        vector<double> highs;
        vector<double> lows;
        vector<unsigned long> volumes;
        for( const_iterator iter(first_in_week_iter); iter != end() && iter->first <= last_in_week_iter->first; ++iter ) {
            highs.push_back(iter->second.high);
            lows.push_back(iter->second.low);
            volumes.push_back(iter->second.volume);
        }
        
        dp.high = (highs.size() > 1 ? *max_element(highs.begin(), highs.end()) : first_in_week_iter->second.high );
        dp.low = (highs.size() > 1 ? *min_element(lows.begin(), lows.end()) : first_in_week_iter->second.low );
        
        if( !volumes.empty() )
            dp.volume = accumulate<vector<unsigned long>::const_iterator, unsigned long>(volumes.begin(), volumes.end(), 0);
        
        weekly_series.insert(EOWSeries::value_type(dp.key, dp));
    }
    
    return weekly_series;
}


Series::EOMSeries Series::EODSeries::monthly( void ) const
{
    EOMSeries monthly_series(name());
    
    // Iterate through all the months in db
    for( month_iterator miter(begin()->first); (*miter) <= rbegin()->first; ++miter ) {
        
        // Find first and last entry for this month
        const_iterator first_in_month_iter = first_in_month((*miter).year(), (*miter).month());
        if( first_in_month_iter == end() ) {
            cerr << "Warning: can't find series bar (BOM) for month of " << (*miter) << " in " << _name << endl;
            continue;
        }
        
        // Last entry for this month
        const_iterator last_in_month_iter = last_in_month((*miter).year(), (*miter).month());
        if( last_in_month_iter == end() ) {
            cerr << "Warning: can't find series bar (EOM) for month of " << (*miter) << " in " << _name << endl;
            continue;
        }
        
        // Initialize monthly series (O,L,H,C,V)
        MonthPrice dp;
        dp.key = last_in_month_iter->first; // Monthly series key is last bar in month
        dp.begin = first_in_month_iter->first;
        dp.end = last_in_month_iter->first;
        dp.open = first_in_month_iter->second.open; // Open on first day of the month
        dp.close = last_in_month_iter->second.close; // Close on last day of the month
        dp.adjclose = last_in_month_iter->second.adjclose; // Adj. close on last day of the month
        dp.high = 0;
        dp.low = 0;
        dp.volume = 0;
        
        // Store all highs, lows, volume to determine monthly values
        vector<double> highs;
        vector<double> lows;
        vector<unsigned long> volumes;
        for( const_iterator iter(first_in_month_iter); iter != end() && iter->first <= last_in_month_iter->first; ++iter ) {
            highs.push_back((*iter).second.high);
            lows.push_back((*iter).second.low);
            volumes.push_back((*iter).second.volume);
        }
        
        dp.high = (highs.size() > 1 ? *max_element(highs.begin(), highs.end()) : first_in_month_iter->second.high );
        dp.low = (highs.size() > 1 ? *min_element(lows.begin(), lows.end()) : first_in_month_iter->second.low );
        
        if( !volumes.empty() )
            dp.volume = accumulate<vector<unsigned long>::const_iterator, unsigned long>(volumes.begin(), volumes.end(), 0);
        
        monthly_series.insert(EOMSeries::value_type(dp.key, dp));
    }
    
    return monthly_series;
}
