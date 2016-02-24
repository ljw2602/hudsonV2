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

#include <StdAfx.h>

// STDLIB
#include <iostream>

// Boost
#include <boost/date_time/gregorian/gregorian.hpp>

// Hudson
#include "StrategyPosition.hpp"

using namespace std;
using namespace boost::gregorian;
using namespace Series;

// XXX Strategy assumes equal weighting of all underlying positions


StrategyPosition::StrategyPosition( Position::ID id, const std::string& symbol, const PositionPtr pPos ):
  Position(id, symbol)
{
  add(pPos);
}


void StrategyPosition::update(const ExecutionPtr pExe)
{
#ifdef DEBUG
  cout << "Synthetic Position " << _id << ", underlying Position " << pExe->position()->id() << ", adding execution " << pExe->id() << ", symbol " << pExe->position()->symbol() << endl;
#endif

  if( _sExecutions.insert(pExe).second == false )
    throw PositionException("Cannot insert execution in StrategyPosition set");
}


bool StrategyPosition::add( const PositionPtr pPos ) throw(PositionException)
{
  // Add position to list
  if( ! _sPositions.insert(pPos).second )
    return false;

  ExecutionSet execs = pPos->executions();
  for( ExecutionSet::const_iterator iter(execs.begin()); iter != execs.end(); ++iter ) {
#ifdef DEBUG
    cout << "Synthetic Position " << _id << ", underlying Position " << pPos->id() << ", adding execution " << (*iter)->id() << ", symbol " << (*iter)->position()->symbol() << endl;
#endif
    _sExecutions.insert(*iter);
  }

  // Register for Exection updates
  pPos->attach(this);

  return true;
}


void StrategyPosition::attach(ExecutionObserver* pObserver) throw(PositionException)
{
  throw PositionException("StrategyPosition does not contain any execution");
}


void StrategyPosition::detach(ExecutionObserver* pObserver) throw(PositionException)
{
  throw PositionException("StrategyPosition does not contain any execution");
}


Price StrategyPosition::avgEntryPrice( void ) const throw(PositionException)
{
  throw PositionException("StrategyPosition does not have single average entry price");
}


Price StrategyPosition::avgExitPrice( void ) const throw(PositionException)
{
  throw PositionException("StrategyPosition does not have single average exit price");
}


void StrategyPosition::buy( const boost::gregorian::date& dt, const Price& price, unsigned size ) throw(PositionException)
{
  throw PositionException("StrategyPosition can not be bought");
}


void StrategyPosition::buy( const boost::gregorian::date& dt, Series::EODDB::PriceType pt, unsigned size ) throw(PositionException)
{
  throw PositionException("StrategyPosition can not be bought");
}


void StrategyPosition::sell( const boost::gregorian::date& dt, const Price& price, unsigned size ) throw(PositionException)
{
  throw PositionException("StrategyPosition can not be sold");
}


void StrategyPosition::sell( const boost::gregorian::date& dt, Series::EODDB::PriceType pt, unsigned size ) throw(PositionException)
{
  throw PositionException("StrategyPosition can not be sold");
}


void StrategyPosition::sell_short( const boost::gregorian::date& dt, const Price& price, unsigned size ) throw(PositionException)
{
  throw PositionException("StrategyPosition can not be sold short");
}


void StrategyPosition::sell_short( const boost::gregorian::date& dt, Series::EODDB::PriceType pt, unsigned size ) throw(PositionException)
{
  throw PositionException("StrategyPosition con not be sold short");
}


void StrategyPosition::cover( const boost::gregorian::date& dt, const Price& price, unsigned size ) throw(PositionException)
{
  throw PositionException("StrategyPosition can not be covered");
}


void StrategyPosition::cover( const boost::gregorian::date& dt, Series::EODDB::PriceType pt, unsigned size ) throw(PositionException)
{
  throw PositionException("StrategyPosition can not be covered");
}


void StrategyPosition::close( const boost::gregorian::date& dt, const Price& price ) throw(PositionException)
{
  throw PositionException("StrategyPosition can not be closed at a specific price");
}


void StrategyPosition::close( const boost::gregorian::date& dt, Series::EODDB::PriceType pt ) throw(PositionException)
{
  for( PositionSet::const_iterator citer = _sPositions.begin(); citer != _sPositions.end(); ++citer )
    (*citer)->close(dt, pt);
}


double StrategyPosition::factor( EODDB::PriceType pt ) const throw(PositionException)
{
  double f_acc = 0;
  for( PositionSet::const_iterator citer = _sPositions.begin(); citer != _sPositions.end(); ++citer )
    f_acc += (*citer)->factor(pt);

  // Normalize to single position
  return (f_acc - _sPositions.size()) + 1;
}


double StrategyPosition::factor( const boost::gregorian::date& dt, EODDB::PriceType pt ) const throw(PositionException)
{
  double f_acc = 0;
  for( PositionSet::const_iterator citer = _sPositions.begin(); citer != _sPositions.end(); ++citer )
    f_acc += (*citer)->factor(dt, pt);

  // Normalize to single position
  return (f_acc - _sPositions.size()) + 1;
}


double StrategyPosition::factor( const boost::gregorian::date_period& dp, EODDB::PriceType pt ) const throw(PositionException)
{
  double f_acc = 0;
  for( PositionSet::const_iterator citer = _sPositions.begin(); citer != _sPositions.end(); ++citer )
    f_acc += (*citer)->factor(dp, pt);

  // Normalize to single position
  return (f_acc - _sPositions.size()) + 1;
}


double StrategyPosition::factor( const boost::gregorian::date::month_type& month, const boost::gregorian::date::year_type& year, EODDB::PriceType pt ) const throw(PositionException)
{
  double f_acc = 0;
  for( PositionSet::const_iterator citer = _sPositions.begin(); citer != _sPositions.end(); ++citer )
    f_acc += (*citer)->factor(month, year);

  // Normalize to single position
  return (f_acc - _sPositions.size()) + 1;
}


SeriesFactorSet StrategyPosition::factors(Series::EODDB::PriceType pt) const throw(PositionException)
{
#ifdef DEBUG
  cout << "Strategy Position " << _id << " first exec " << first_exec()->dt() << ", last exec " << last_exec()->dt() << endl;
  cout << "Number of legs: " << _sPositions.size() << endl;
#endif
  date last_dt = (closed() ? last_exec()->dt() : EODDB::instance().get(_symbol).rbegin()->first);

  date_period dp(first_exec()->dt(), last_dt);
  return factors(dp, pt);
}


SeriesFactorSet StrategyPosition::factors( const boost::gregorian::date& dt, Series::EODDB::PriceType pt ) const throw(PositionException)
{
  date_period dp(first_exec()->dt(), dt);
  return factors(dp, pt);
}


SeriesFactorSet StrategyPosition::factors( const boost::gregorian::date_period& dp, EODDB::PriceType pt ) const throw(PositionException)
{
  // Accumulate all factors with the same begin/end date for all underlying positions
  SeriesFactorMultiSetFrom sfsAll;

  // Add all SeriesFactor for all underlying positions indexed by first execution date
  for( PositionSet::const_iterator citer = _sPositions.begin(); citer != _sPositions.end(); ++citer ) {
    SeriesFactorSet sfs = (*citer)->factors(dp, pt);
    for( SeriesFactorSet::const_iterator sfs_citer = sfs.begin(); sfs_citer != sfs.end(); ++sfs_citer ) {
#ifdef DEBUG
      cout << "Inserting position " << (*citer)->id() << " factor from " << (*sfs_citer).from_tm() << ", to " << (*sfs_citer).to_tm() << endl;
#endif
      sfsAll.insert(*sfs_citer);
    }
  }

  return _matchFactors(sfsAll);
}


SeriesFactorSet StrategyPosition::_matchFactors(const SeriesFactorMultiSetFrom& sfsAll) const
{
  SeriesFactorSet sfsStrategy;

  // Extract current factor
  for( SeriesFactorMultiSetFrom::const_iterator citer = sfsAll.begin(); citer != sfsAll.end(); ) {

    // Get current factor
    SeriesFactor currentFactor = *citer;
    double acc = currentFactor.factor();

#ifdef DEBUG
    cout << "Analyzing Position daily factor from " << currentFactor.from_tm() << " to " << currentFactor.to_tm()
	 << " factor " << currentFactor.factor() << endl;
#endif

    // Accumulate all the following factors with the same from/to date
    SeriesFactorMultiSetFrom::const_iterator citer_next = citer;
    while( ++citer_next != sfsAll.end() && citer_next->from_tm() == currentFactor.from_tm() && citer_next->to_tm() == currentFactor.to_tm() ) {
#ifdef DEBUG
      cout << "Found daily factor with same from/to dates, factor " << citer_next->factor() << endl;
#endif
      acc *= citer_next->factor();
    }

    // Add cumulated factor
    sfsStrategy.insert(SeriesFactor(currentFactor.from_tm(), currentFactor.to_tm(), acc));

    // Start from first SeriesFactor with different from/to date
    citer = citer_next;
  }

  return sfsStrategy;
}


bool StrategyPosition::open( void ) const
{
  for( PositionSet::const_iterator citer = _sPositions.begin(); citer != _sPositions.end(); ++citer )
    if( (*citer)->open() )
      return true;
      
  return false;
}


bool StrategyPosition::closed( void ) const
{
  return !open();
}


void StrategyPosition::print( void ) const
{
  cout << "Synthetic " << _symbol << " (" << _id << ")" << " - " << "Factor " << factor() << " (" << (factor()-1)*100 << "%): " << endl;

  for( PositionSet::const_iterator citer = _sPositions.begin(); citer != _sPositions.end(); ++citer ) {
    cout << '\t';
    (*citer)->print();
    cout << endl;
  }
}
