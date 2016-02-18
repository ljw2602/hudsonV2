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

// CSTD
#include <cmath>

// Hudson
#include "EOMReturnFactors.hpp"
#include "PositionSet.hpp"
#include "EODDB.hpp"

using namespace std;
using namespace Series;
using namespace boost::gregorian;


EOMReturnFactors::EOMReturnFactors( const PositionSet& sPositions,
                                    const boost::gregorian::date& begin, const boost::gregorian::date& end, double rf_rate ):
  ReturnFactors(sPositions),
  _begin(begin),
  _end(end),
  _rf_rate(rf_rate),
  _mmean(0),
  _mstddev(0),
  _msharpe(0),
  _days(0),
  _years(0)
{
  _days = end - begin;
  _years = (_days.days() == 0 ? 0 : _days.days()/(double)365);
  
  _calculateM2M();
}


double EOMReturnFactors::cagr(void) const
{
  return _sPositions.empty() ? 0 : std::pow(_fvalue, 1/_years) - 1;
}


double EOMReturnFactors::gsd( void ) const
{
  if( _sPositions.empty() )
    return 0;

  double lfavg = accumulate<doubleVector::const_iterator, double>( _vLogMFactors.begin(), _vLogMFactors.end(), 0 ) / _vLogMFactors.size();
  double lfstddev = ::sqrt( accumulate<doubleVector::const_iterator, double>( _vLogMFactors.begin(), _vLogMFactors.end(), 0, variance_bf(lfavg) ) / (_vLogMFactors.size() - 1) );
  return ::pow( (double)10, lfstddev * ::sqrt((double)12) ) - 1;
}


double EOMReturnFactors::sharpe( void ) const
{
  if( _sPositions.empty() )
    return 0;
    
  return _msharpe;
}


void EOMReturnFactors::_calculateM2M(void)
{
  // For each month in selected period
  date prev_em_mark(_begin);
  date em_mark;
  for( month_iterator miter(_begin); miter <= _end; ++miter ) {

    em_mark = miter->end_of_month();
    date_period month_period(prev_em_mark, em_mark);
    //cout << "Calculating M2M factor for period " << month_period << endl;

    // For each position
    double f_acc = 1;
    bool cash = true;
    for( PositionSet::const_iterator piter(_sPositions.begin()); piter != _sPositions.end(); ++piter ) {

      PositionPtr pPos = *piter;
      // Skip positions closed before prev_em_mark or opened after em_mark
      if( ! month_period.intersects(pPos->hold_period()) ) {
        //cout << "Skipping position " << pPos->id() << ", holding period " << pPos->hold_period() << ", current monthly period " << month_period << endl;
        continue;
      }

      // Calculate monthly factor for this position
      double f = pPos->factor(miter->month(), miter->year());
      //cout << "Monthly factor for position " << pPos->id() << " " << pPos->symbol() << " (" << miter->month() << "): " << f << endl;

      f_acc *= f;
      cash = false;
    } // End of positions for this month

    if( cash )
      f_acc = 1 + (_rf_rate/100/12);
      
    //cout << em_mark << ", factor " << f_acc << endl;
    _vMFactors.push_back(f_acc);
    _vLogMFactors.push_back(::log10(f_acc));
    _mDateMFactors.insert(DATEMFACTOR::value_type(em_mark, f_acc));

    prev_em_mark = em_mark;
  } // for each month in selected period

  _mmean = accumulate<doubleVector::const_iterator, double>( _vMFactors.begin(), _vMFactors.end(), 0 ) / _vMFactors.size();
  _mstddev = ::sqrt( accumulate<doubleVector::const_iterator, double>( _vMFactors.begin(), _vMFactors.end(), 0, variance_bf(_mmean) ) / (_vMFactors.size() - 1) );
  _msharpe = (((_mmean-1)*12) - (_rf_rate/100)) / (_mstddev*::sqrt((double)12));
}

