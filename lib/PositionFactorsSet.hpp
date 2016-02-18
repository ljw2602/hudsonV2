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

#ifndef _POSITIONFACTORSSET_HPP_
#define _POSITIONFACTORSSET_HPP_

#ifdef WIN32
#pragma warning (disable:4290)
#endif

// STL
#include <vector>
#include <numeric>
#include <functional>

// Hudson
#include "PositionSet.hpp"

#include "SeriesFactorSet.hpp"


class PositionFactorsSetException: public std::exception
{
public:
  PositionFactorsSetException(const std::string& msg):
    _Str("PositionFactorsSetException: ")
  {
    _Str += msg;
  }

  virtual ~PositionFactorsSetException(void) throw() { }
  virtual const char *what(void) const throw() { return _Str.c_str(); }

protected:
  std::string _Str;
};


/*!
  A collection of PositionFactors. Calculates peak/drawdown Position statistics for a set of PositionFactors.
  While PositionFactors analyzes factors for a single Position, PositionFactorsSet look at the data for a number of
  PositionFactors objects and extract some significant values, such as the highest peak among all the included
  PositionFactors or the total bars longest drawdown.
  \see ExcursionResults.
*/
class PositionFactorsSet
{
public:
  /*!
    ExcursionResults includes calculated data for the PositionSet passed into the PositionFactorsSet constructor. high and consecutive
    data assumes different meanings for values returned by a favorable() or adverse() calls.
    For favorable data, high is the best unrealized Position peak, consecutive is the longest unrealized Position peak.
    For adverse data, high is the worst unrealized Position drawdown, consecutive is the longest unrealized Position drawdown.
    \see favorable()
    \see adverse()
  */
  struct ExcursionResults 
  {
    ExcursionResults(void);

    double avg; //! Average excursion.
    SeriesFactorSet high; //! Best/Worst unrealized Position peak/drawdown.
    SeriesFactorSet consecutive; //! Longest/Shortest unrealized Position peak/drawdown.
  };

public:
  /*!
    \param sPositions The set of Positions that will be used to calculate peak/drawdown statistics.
  */
  PositionFactorsSet(const PositionSet& sPositions);

  //! Returns the total number of positions included in the analysis.
  unsigned num(void) const { return (unsigned)_sPositions.size(); }

  //! Returns favorable (peak) unrealized excursion statistics.
  ExcursionResults favorable(void) const throw(PositionFactorsSetException);
  
  //! Returns adverse (drawdown) unrealized excursion statistics.
  ExcursionResults adverse(void) const throw(PositionFactorsSetException);

private:
  struct SeriesFactorSetSizeLtCmp: public std::binary_function<SeriesFactorSet, SeriesFactorSet, bool>
  {
    bool operator()(const SeriesFactorSet& sfs1, const SeriesFactorSet& sfs2) const { return sfs1.size() < sfs2.size(); }
  };

  struct SeriesFactorSetFactorLtCmp: public std::binary_function<SeriesFactorSet, SeriesFactorSet, bool>
  {
    bool operator()(const SeriesFactorSet& sfs1, const SeriesFactorSet& sfs2) const { return sfs1.factor() < sfs2.factor(); }
  };

private:
  const PositionSet _sPositions;
};

#endif // _POSITIONSFACTORS_HPP_
