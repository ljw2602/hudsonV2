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

// Hudson
#include "ShortPosition.hpp"
#include "EODDB.hpp"

using namespace std;
using namespace boost::gregorian;
using namespace Series;


ShortPosition::ShortPosition(ID id, const string& symbol, const date& dt, const Price& price, unsigned size) throw(PositionException):
  Position(id, symbol),
  _shorts(0),
  _covers(0),
  _avgShortPrice(0),
  _avgCoverPrice(0)
{
  if( size == 0 )
	  throw PositionException("Invalid size");

  if( dt.is_not_a_date() )
	  throw PositionException("Invalid date");

  if( _sExecutions.sell_short(_symbol, dt, price, size) == Execution::NullID )
	  throw PositionException("Invalid execution");

  _avgShortPrice = ((_avgShortPrice * _shorts) + (price * size)) / (double)(_shorts + size);

  _size += size;
  _shorts += size;
}


void ShortPosition::sell_short(const date& dt, const Price& price, unsigned size) throw(PositionException)
{
  if( closed() )
	  throw PositionException("Position is closed");

  if( size == 0 )
	  throw PositionException("Invalid size");

  if( dt.is_not_a_date() )
	  throw PositionException("Invalid date");

  if( _sExecutions.sell_short(_symbol, dt, price, size) == Execution::NullID )
	  throw PositionException("Invalid execution");

  _avgShortPrice = ((_avgShortPrice * _shorts) + (price * size)) / (double)(_shorts + size);

  _size += size;
  _shorts += size;
}


void ShortPosition::cover(const date& dt, const Price& price, unsigned size) throw(PositionException)
{
  if( closed() )
	  throw PositionException("Position is closed");

  if( size == 0 || size > _size )
	  throw PositionException("Invalid size");

  if( dt.is_not_a_date() )
	  throw PositionException("Invalid date");

  if( _sExecutions.cover(_symbol, dt, price, size) == Execution::NullID )
	  throw PositionException("Invalid execution");

  _avgCoverPrice = ((_avgCoverPrice * _covers) + (price * size)) / (_covers + size);

  _size -= size;
  _covers += size;
}


void ShortPosition::close(const date& dt, const Price& price) throw(PositionException)
{
  cover(dt, price, _size);
}


void ShortPosition::buy(const boost::gregorian::date& dt, const Price& price, unsigned size) throw(PositionException)
{
  throw PositionException("Can't buy short position");
}


void ShortPosition::buy( const boost::gregorian::date& dt, EODDB::PriceType pt, unsigned size ) throw(PositionException)
{
  throw PositionException("Can't buy short position");
}


void ShortPosition::sell(const boost::gregorian::date& dt, const Price& price, unsigned size) throw(PositionException)
{
  throw PositionException("Short position");
}


void ShortPosition::sell( const boost::gregorian::date& dt, EODDB::PriceType pt, unsigned size ) throw(PositionException)
{
  throw PositionException("Can't sell short position");
}


double ShortPosition::factor( EODDB::PriceType pt ) const throw(PositionException)
{
  if( !isValid() )
    throw PositionException("Invalid position state");

  return closed() ? (_avgShortPrice / _avgCoverPrice) : (_avgShortPrice / Price::last(_symbol, pt).value());
}


double ShortPosition::factor( const boost::gregorian::date& dt, EODDB::PriceType pt ) const throw(PositionException)
{
  if( !avgEntryPrice().isValid() )
    throw PositionException("Invalid average sell short price");

  if( dt <= first_exec()->dt() )
    throw PositionException("Requested date after first execution date");

  return _avgShortPrice / Price::get(_symbol, dt, pt).value();
}


double ShortPosition::factor( const boost::gregorian::date_period& dp, EODDB::PriceType pt ) const throw(PositionException)
{
  // Verify that input period is contained within position holding period
  if( ! hold_period().contains(dp) )
    throw PositionException("Requested factor period is out of Position range");

  return Price::get(_symbol, dp.begin(), pt) / Price::get(_symbol, dp.end(), pt);
}


double ShortPosition::factor( const boost::gregorian::date::month_type& month, const boost::gregorian::date::year_type& year, EODDB::PriceType pt ) const throw(PositionException)
{
  // Generate monthly begin/end dates
  date period_month(year, month, 1);

  date begin_mark = period_month - days(1); // Last day in previous month
  date end_mark = period_month.end_of_month();

  // If position was opened after end_mark or was closed before begin_mark, period is out of range
  if( first_exec()->dt() > end_mark || (closed() && last_exec()->dt() < begin_mark) )
    throw PositionException("Position executions are not included in given range");

  // Get symbol series
  const Series::EODSeries& db = Series::EODDB::instance().get(_symbol);

  // Extract begin of period price
  double begin_price = 0;
  // If position was opened before begin mark, then use begin_mark price (previous month last close)
  if( first_exec()->dt() <= begin_mark ) {
    EODSeries::const_iterator citer = db.at_or_before(begin_mark);
    if( citer == db.end() )
      throw PositionException("Can't get begin-period price");

    begin_price = Price::get(_symbol, citer->first, pt).value();
#ifdef DEBUG
    //cout << "Position opened before or at previous EOM mark price, using " << citer->first << " adjclose" << endl;
#endif

    // Else if position was opened after begin mark, use position opening price
  } else if( first_exec()->dt() > begin_mark && first_exec()->dt() <= end_mark ) {
    begin_price = avgEntryPrice().value();
#ifdef DEBUG
    //cout << "Position opened after previous EOM mark price, using position avg entry price" << endl;
#endif

    // Should never end up here 
  } else {
    throw PositionException("Position factor(): Internal error on first execution check");
  }

  // Extract end of period price
  double end_price = 0;
  // If position is open or was closed after end-month mark, use end-month price
  if( open() || last_exec()->dt() > end_mark ) {
    EODSeries::const_iterator citer = db.at_or_before(end_mark);
    if( citer == db.end() )
      throw PositionException("Can't get end-period price");

    end_price = Price::get(_symbol, citer->first, pt).value();
#ifdef DEBUG
    //cout << "Position still open or closed after EOM mark price, using " << em_mark->first << " adjclose" << endl;
#endif

    // Else if position closing execution is before end-month mark, use execution price
  } else if( last_exec()->dt() <= end_mark ) {
    end_price = avgExitPrice().value();
#ifdef DEBUG
    //cout << "Position closed before EOM mark price, using position avg exit price" << endl;
#endif

    // Should never finish here
  } else {
    throw PositionException("Position factor(): Internal error on last execution check");
  }

  // Return monthly factors
  return begin_price / end_price;
}


SeriesFactorSet ShortPosition::factors(Series::EODDB::PriceType pt) const throw(PositionException)
{
#ifdef DEBUG
  cout << "Short Position " << _id << " first exec " << first_exec()->dt() << ", last exec " << last_exec()->dt() << endl;
  cout << "Symbol " << _symbol << " end of database: " << EODDB::instance().get(_symbol).rbegin()->first << endl;
  cout << "Is Position closed: " << closed() << endl;
#endif

  date last_dt = (closed() ? last_exec()->dt() : EODDB::instance().get(_symbol).rbegin()->first);

  date_period dp(first_exec()->dt(), last_dt);
  return factors(dp, pt);
}


SeriesFactorSet ShortPosition::factors( const boost::gregorian::date& dt, Series::EODDB::PriceType pt ) const throw(PositionException)
{
  date_period dp(first_exec()->dt(), dt);
  return factors(dp, pt);
}


SeriesFactorSet ShortPosition::factors( const boost::gregorian::date_period& dp, EODDB::PriceType pt /*= EODDB::PriceType::ADJCLOSE*/ ) const throw(PositionException)
{
  SeriesFactorSet sfs;

#ifdef DEBUG
  cout << "Extracting daily factors for short position " << _id << " (" << _symbol << ") from " << dp.begin() << " to " << dp.last() << endl;
#endif

  if( ! hold_period().contains(dp) )
    throw PositionException("Requested period is out of range");

  const EODSeries& series = EODDB::instance().get(_symbol);

  EODSeries::const_iterator citer = series.after(dp.begin(), 0);
  if( citer == series.end() )
    throw PositionException("Can't find begin of period in series");

  date prev_date = citer->first;

  for( EODSeries::const_iterator citer = series.after(dp.begin()); citer != series.end(); ++citer ) {

    // If we're over the end of requested period or if the position is closed and we're over the last execution date, then we're done
    if( (*citer).first > dp.last() || (closed() && (*citer).first > last_exec()->dt()) )
      break;

    double f = factor(date_period(prev_date, citer->first), pt);
    sfs.insert(SeriesFactor(prev_date, citer->first, f));

#ifdef DEBUG
    cout << "Previous date: " << prev_date << ", current date: " << citer->first << ", factor: " << f << endl;
#endif

    prev_date = citer->first;
  }

  return sfs;
}

