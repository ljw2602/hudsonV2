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

// STL
#include <vector>
#include <algorithm>

// Hudson
#include "PositionFactorsSet.hpp"
#include "PositionFactors.hpp"

using namespace std;


PositionFactorsSet::PositionFactorsSet( const PositionSet& sPositions ):
  _sPositions(sPositions)
{
}


PositionFactorsSet::ExcursionResults PositionFactorsSet::favorable(void) const throw(PositionFactorsSetException)
{
  if ( _sPositions.empty() )
    throw PositionFactorsSetException("Empty position set");

  ExcursionResults ret;
  std::vector<SeriesFactorSet> vSFSHighest, vSFSConsecutive;

  // Calculate average favorable position excursion
  for( PositionSet::const_iterator iter = _sPositions.begin(); iter != _sPositions.end(); ++iter ) {
    PositionFactors pf(*(*iter));
    vSFSHighest.push_back(pf.bfe()); // Best favorable excursion for this position
    vSFSConsecutive.push_back(pf.max_cons_pos()); // Maximum consecutive favorable bars for this position
  }

  // Calculate average favorable excursion
  double acc = 0;
  for( unsigned i = 0; i < vSFSHighest.size(); i++ )
    acc += vSFSHighest[i].factor();

  ret.avg = (acc ? (acc/vSFSHighest.size()) - 1 : 0);

  // Set best favorable excursion
  ret.high = *max_element(vSFSHighest.begin(), vSFSHighest.end(), SeriesFactorSetFactorLtCmp());

  // Set longest favorable excursion
  ret.consecutive = *max_element(vSFSConsecutive.begin(), vSFSConsecutive.end(), SeriesFactorSetSizeLtCmp());

  return ret;
}


PositionFactorsSet::ExcursionResults PositionFactorsSet::adverse(void) const throw(PositionFactorsSetException)
{
  if ( _sPositions.empty() )
    throw PositionFactorsSetException("Empty position set");

  ExcursionResults ret;
  std::vector<SeriesFactorSet> vSFSHighest, vSFSConsecutive;

  // Calculate average adverse position excursion
  for( PositionSet::const_iterator iter = _sPositions.begin(); iter != _sPositions.end(); ++iter ) {
    PositionFactors pf(*(*iter));
    vSFSHighest.push_back(pf.wae()); // Worst adverse excursion for this position
    vSFSConsecutive.push_back(pf.max_cons_neg()); // Maximum consecutive adverse bars for this position
  }

  // Calculate average adverse excursion
  double acc = 0;
  for( unsigned i = 0; i < vSFSHighest.size(); i++ )
    acc += vSFSHighest[i].factor();

  ret.avg = (acc ? (acc/vSFSHighest.size()) - 1 : 0);

  // Set worst adverse excursion
  ret.high = *min_element(vSFSHighest.begin(), vSFSHighest.end(), SeriesFactorSetFactorLtCmp());

  // Set longest adverse excursion
  ret.consecutive = *max_element(vSFSConsecutive.begin(), vSFSConsecutive.end(), SeriesFactorSetSizeLtCmp());

  return ret;
}


PositionFactorsSet::ExcursionResults::ExcursionResults( void ):
  avg(0)
{
}
