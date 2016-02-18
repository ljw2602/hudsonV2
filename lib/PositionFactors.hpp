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

#ifndef _POSITIONFACTORS_HPP_
#define _POSITIONFACTORS_HPP_

#ifdef WIN32
#pragma warning (disable:4290)
#endif

// CSTD
#include <cmath>

// STL
#include <stdexcept>

// Boost
#include <boost/date_time/gregorian/gregorian.hpp>

// Hudson
#include "Position.hpp"
#include "SeriesFactor.hpp"
#include "SeriesFactorSet.hpp"


class PositionFactorsException: public std::exception
{
  public:
    PositionFactorsException(const std::string& msg):
      _Str("PositionFactorsException: ")
    {
      _Str += msg;
    }

    virtual ~PositionFactorsException(void) throw() { }
    virtual const char *what() const throw() { return _Str.c_str(); }

  protected:
    std::string _Str;
};


/*!
* Position daily factors. Calculates Position unrealized peak/drawdown statistics.
*/
class PositionFactors
{
public:
  /*!
  * Initialize position daily factors for this position.
  * \param pos Position that must be analyzed.
  */
  PositionFactors(const Position& pos, Series::EODDB::PriceType = Series::EODDB::ADJCLOSE);

  //! Maximum consecutive positive daily factors.
  SeriesFactorSet max_cons_pos(void) const;
  //! Maximum consecutive negative daily factors.
  SeriesFactorSet max_cons_neg(void) const;

  //! Best daily peak.
  SeriesFactorSet bfe(void) const throw(PositionFactorsException);
  //! Worst daily drawdown.
  SeriesFactorSet wae(void) const throw(PositionFactorsException);

private:
  const Position& _pos;
  const Series::EODDB::PriceType _pt;

  // Must index SeriesFactor by date to speed up excursion calculations
  struct SeriesFactorToTmCmp: public std::binary_function<SeriesFactor, SeriesFactor, bool>
  {
    bool operator()(const SeriesFactor& sf1, const SeriesFactor& sf2) const { return sf1.to_tm() < sf2.to_tm(); }
  };

  struct SeriesFactorFromTmCmp: public std::binary_function<SeriesFactor, SeriesFactor, bool>
  {
    bool operator()(const SeriesFactor& sf1, const SeriesFactor& sf2) const { return sf1.from_tm() < sf2.from_tm(); }
  };

  typedef std::set<SeriesFactor, SeriesFactorToTmCmp>   SF_TOTM;
  typedef std::set<SeriesFactor, SeriesFactorFromTmCmp> SF_FROMTM;

  SF_TOTM   _sf_totm;
  SF_FROMTM _sf_fromtm;    

private:
  struct PeriodFactor
  {
    PeriodFactor(double f = 1);
    bool isValid(void) const;

    double factor;
    boost::gregorian::date from_tm;
    boost::gregorian::date to_tm;
  };

  PeriodFactor _wae(SF_TOTM::const_iterator& start) const;
  PeriodFactor _bfe(SF_TOTM::const_iterator& start) const;
};


#endif // _POSITIONFACTORS_HPP_
