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

// STDLIB
#include <cmath>

// STL
#include <algorithm>

// Hudson
#include "PositionFactors.hpp"
#include "Position.hpp"
#include "SeriesFactorSet.hpp"
#include "EODDB.hpp"

using namespace std;
using namespace boost::gregorian;
using namespace Series;


PositionFactors::PositionFactors( const Position& pos, Series::EODDB::PriceType pt ):
  _pos(pos),
  _pt(pt)
{
  SeriesFactorSet sfsAll = _pos.factors(pt);

  for( SeriesFactorSet::const_iterator citer = sfsAll.begin(); citer != sfsAll.end(); ++citer ) {
    // Initialize SeriesFactorSet objects indexed both by from_time and to_time to calculate bfe() and wae().
    // This is for performance reasons. multi_index secondary key access is slower than primary key.
    _sf_fromtm.insert(*citer);
    _sf_totm.insert(*citer);
  }
}


SeriesFactorSet PositionFactors::max_cons_pos(void) const
{
  SeriesFactorSet maxSFS;

  SF_TOTM::const_iterator iter = _sf_totm.begin();
  while( iter != _sf_totm.end() ) {
    // Skip negative factors
    if( (*iter).factor() <= 1 ) {
      ++iter;
      continue;
    }

    // Positive factor, look for positive sequence
    SeriesFactorSet sfset;
    while( iter != _sf_totm.end() && (*iter).factor() > 1 ) {
      sfset.insert(*iter);
      ++iter;
    }

    // Store new last consecutive-positive if better than previous one
    if( sfset.size() > maxSFS.size() )
      maxSFS = sfset;
  }

  return maxSFS;
}


SeriesFactorSet PositionFactors::max_cons_neg(void) const
{
  SeriesFactorSet maxSFS;

  SF_TOTM::const_iterator iter = _sf_totm.begin();
  while ( iter != _sf_totm.end() ) {
    if( (*iter).factor() >= 1 ) {
      ++iter;
      continue;
    }

    // Negative factor, look for negative sequence
    SeriesFactorSet sfset;
    while( iter != _sf_totm.end() && (*iter).factor() < 1 ) {
      sfset.insert(*iter);
      ++iter;
    }

    // Store new last consecutive-negative if better than previous one
    if( sfset.size() > maxSFS.size() )
      maxSFS = sfset;
  }

  return maxSFS;
}


SeriesFactorSet PositionFactors::wae( void ) const throw(PositionFactorsException)
{
  // Look for position worst excursion starting from each entry in position daily series

  PeriodFactor worst_pf; // Worst found so far
  bool never_set = true; // Whether worst found has been set once already

  // For each SeriesFactor bar
  for( SF_TOTM::const_iterator iter = _sf_totm.begin(); iter != _sf_totm.end(); ++iter ) {

    // Calculate worst adverse excursion from this point to the end
    PeriodFactor pf = _wae(iter); // return drawdown from this bar

    // Compate last worst adverse excursion to previous set
    if( never_set || pf.factor < worst_pf.factor ) {
      worst_pf = pf;
      never_set = false;
#ifdef DEBUG
      cout << "WAE set from " << worst_pf.from_tm << " to " << worst_pf.to_tm << " (" << worst_pf.factor << ")" << endl;
#endif
    }
  }

  if( !worst_pf.isValid() ) {
    stringstream ss;
    ss << "Can't get position " << _pos.id() << " worst adverse excursion period";
    throw PositionFactorsException(ss.str());
  }

#ifdef DEBUG
  //cout << "Found worst excursion from " << worst_pf.from_tm << " to " << worst_pf.to_tm << endl;
#endif

  // Locate beginning of stored worst adverse excursion in SeriesFactor set
  SeriesFactor sf(worst_pf.from_tm, worst_pf.to_tm, worst_pf.factor);
  SF_FROMTM::const_iterator src_begin = _sf_fromtm.find(sf);
  if( src_begin == _sf_fromtm.end() ) {
    stringstream ss;
    ss << "Can't get position " << _pos.id() << " beginning of worst adverse excursion period" << endl;
    throw PositionFactorsException(ss.str());
  }

  // Initialize a new SeriesFactorSet with SeriesFactor included in worst_pf range
  SeriesFactorSet sfs;
#ifdef DEBUG
  //cout << "Adding SeriesFactor from " << (*src_begin).from_tm() << " to " << worst_pf.to_tm << endl;
#endif
  for( SF_FROMTM::const_iterator iter(src_begin); iter != _sf_fromtm.end() && (*iter).to_tm() <= worst_pf.to_tm; ++iter )
    sfs.insert(*iter);

  return sfs; // Return worst drawdown series factor set for this position
}


PositionFactors::PeriodFactor PositionFactors::_wae(SF_TOTM::const_iterator& start) const
{
  // Return value defaults
  PeriodFactor pf;
  pf.from_tm = (*start).from_tm(); // Begin of excursion analysis

  // Calculate worst excursion from 'start'.
  SeriesFactorSet sfs;
  bool never_set = true;
  for( SF_TOTM::const_iterator iter = start; iter != _sf_totm.end(); ++iter ) {

    sfs.insert(*iter); // current position factors series

    // Check lower excursion
    if( never_set || sfs.factor() < pf.factor ) {
      // Store lower excursion
      pf.factor = sfs.factor();
      pf.to_tm = (*iter).to_tm();
      never_set = false;
    }
  }

  // Return worst drawdown factor including begin/end time marks
  return pf;
}


SeriesFactorSet PositionFactors::bfe(void) const throw(PositionFactorsException)
{
  // Look for worst position excursion period starting from each entry in daily series

  // For each SeriesFactor bar
  PeriodFactor best_pf;
  bool never_set = true;
  for( SF_TOTM::const_iterator iter = _sf_totm.begin(); iter != _sf_totm.end(); ++iter ) {

    // Calculate best favorable excursion from this point
    PeriodFactor pf = _bfe(iter); // return best favorable excursion from this bar

    // Store best favorable excursion till now
    if( never_set || pf.factor > best_pf.factor ) {
      best_pf = pf;
      never_set = false;
    }
  }

  if( !best_pf.isValid() ) {
    stringstream ss;
    ss << "Can't get position " << _pos.id() << " best favorable excursion period";
    throw PositionFactorsException(ss.str());
  }

  // Locate beginning of stored best excursion
  SeriesFactor sf(best_pf.from_tm, best_pf.to_tm, best_pf.factor);
  SF_FROMTM::const_iterator src_begin = _sf_fromtm.find(sf);
  if( src_begin == _sf_fromtm.end() ) {
    stringstream ss;
    ss << "Can't get position " << _pos.id() << " beginning of best favorable excursion period" << endl;
    throw PositionFactorsException(ss.str());
  }

  // Initialize SeriesFactorSet with SeriesFactor(s) included in best_pf range
  SeriesFactorSet sfs;
  for( SF_FROMTM::const_iterator iter(src_begin); (*iter).to_tm() <= best_pf.to_tm && iter != _sf_fromtm.end(); ++iter )
    sfs.insert(*iter);

  return sfs; // Return worst drawdown for this position
}


PositionFactors::PeriodFactor PositionFactors::_bfe(SF_TOTM::const_iterator& start) const
{
  // Return value defaults
  PeriodFactor pf;
  pf.from_tm = (*start).from_tm(); // Begin of excursion no matter what

  // Calculate best excursion from 'start'. We can't use multi_index secondary index for performance reasons.
  SeriesFactorSet sfs;
  bool never_set = true;
  for( SF_TOTM::const_iterator iter = start; iter != _sf_totm.end(); ++iter ) {
    sfs.insert(*iter);

    // Check higher excursion
    if( never_set || sfs.factor() > pf.factor ) {
      // Store higher excursion
      pf.factor = sfs.factor();
      pf.to_tm = (*iter).to_tm();
      never_set = false;
    }
  }

  return pf;
}


PositionFactors::PeriodFactor::PeriodFactor( double f ):
  factor(f)
{
}


bool PositionFactors::PeriodFactor::isValid( void ) const
{
  return (from_tm.is_not_a_date() || to_tm.is_not_a_date()) ? false : true;
}
