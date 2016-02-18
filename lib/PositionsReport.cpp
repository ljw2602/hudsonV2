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
#include <iostream>

// Hudson
#include "PositionsReport.hpp"
#include "SeriesFactorSet.hpp"
#include "SeriesFactor.hpp"

using namespace std;


PositionsReport::PositionsReport( const PositionFactorsSet& pf ):
  _pf(pf)
{
}


void PositionsReport::print( void ) const
{
  streamsize curr_precision = cout.precision();
  ios_base::fmtflags curr_flags = cout.flags();

  cout.precision(2);
  cout.setf(ios::fixed);

  adverse();
  favorable();

  cout.precision(curr_precision);
  cout.flags(curr_flags);
}


void PositionsReport::favorable(void) const
{
  if( _pf.num() == 0 )
    return; // avoid exception in report

  std::cout << "Favorable Excursion" << std::endl;

  PositionFactorsSet::ExcursionResults er = _pf.favorable();

  // Average favorable excursion
  std::cout << "Average FE: " << er.avg*100 << '%' << std::endl;
  
  // Longest favorable excursion
  cout << "Longest FE: ";
  if( er.consecutive.empty() ) {
    cout << 0 << endl;
  } else {
    const SeriesFactor& sf_begin = *(er.consecutive.get<to_key>().begin());
    const SeriesFactor& sf_end   = *(er.consecutive.get<to_key>().rbegin());

    cout << (int)er.consecutive.size() << " [" << sf_begin.from_tm() << '/' << sf_end.to_tm() << ']' << endl;
  }

  // Best FA
  const SeriesFactorSet& sfs = er.high;
  const SeriesFactor& sf_begin = *(er.high.get<from_key>().begin());
  const SeriesFactor& sf_end = *(er.high.get<to_key>().rbegin());

  std::cout << "Best FE: " << (sfs.factor()-1)*100 << '%';
  std::cout << " [" << sf_begin.from_tm() << '/' << sf_end.to_tm() << ']' << endl;
}


void PositionsReport::adverse(void) const
{
  if( _pf.num() == 0 )
    return; // avoid exception in report

  std::cout << "Adverse Excursion" << std::endl;

  PositionFactorsSet::ExcursionResults er = _pf.adverse();

  // Average adverse excursion
  std::cout << "Avg AE: " << er.avg*100 << '%' << std::endl;
  
  // Consecutive adverse
  cout << "Longest AE: ";
  if( er.consecutive.empty() ) {
    cout << 0 << endl;
  } else {
    const SeriesFactor& sf_begin = *(er.consecutive.get<to_key>().begin());
    const SeriesFactor& sf_end   = *(er.consecutive.get<to_key>().rbegin());

    cout << (int)er.consecutive.size() << " [" << sf_begin.from_tm() << '/' << sf_end.to_tm() << ']' << endl;
  }

  // Worst adverse excursion
  const SeriesFactorSet& sfs = er.high;
  const SeriesFactor& sf_begin = *(er.high.get<from_key>().begin());
  const SeriesFactor& sf_end = *(er.high.get<to_key>().rbegin());

  std::cout << "Worst AE: " << (sfs.factor()-1)*100 << '%';
  std::cout << " [" << sf_begin.from_tm() << '/' << sf_end.to_tm() << ']' << endl;
}
