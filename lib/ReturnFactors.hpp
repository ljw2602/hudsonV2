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

#ifndef _RETURNFACTORS_HPP_
#define _RETURNFACTORS_HPP_

#ifdef WIN32
#pragma warning (disable:4290)
#endif

// STD
#include <stdexcept>
#include <cmath>

// STL
#include <vector>
#include <numeric>
#include <functional>

// Boost
#include <boost/date_time/gregorian/gregorian.hpp>

// Hudson
#include "PositionSet.hpp"


class ReturnFactorsException: public std::exception
{
public:
  ReturnFactorsException(const std::string& msg):
    _Str("ReturnFactorsException: ")
  {
    _Str += msg;
  }

  virtual ~ReturnFactorsException(void) throw() { }
  virtual const char *what(void) const throw() { return _Str.c_str(); }

protected:
  std::string _Str;
};

//! Calculate return factor statistics for a PositionSet.
/*!
  ReturnFactors calculates a number of P/L statistics for one or more Position(s). While ReturnFactors is used to analyze Position returns,
  PositionFactors calculates statistics on a single Position by extracting its daily factors.
  \see PositionFactors.
*/
class ReturnFactors
{
public:
  /*!
    \param sPositions The set of positions used to calculate these return factors.
  */
  ReturnFactors(const PositionSet& sPositions);

  //! Return the total number of positions passed in the constructor.
  int num(void) const;

  //! Return the return on investment.
  double roi(void) const;
  //! Return the average factors return.
  double avg(void) const;
  //! Return the factors return standard deviation.
  double stddev(void) const;
  //! Return the factors return skew.
  double skew(void) const;

  //! Return all positive positions.
  PositionSet pos(void) const;
  //! Return all negative positions.
  PositionSet neg(void) const;

  //! Return the Position with the best (highest) return factor.
  const Position& best(void) const throw(ReturnFactorsException);
  //! Return the Position with the worst (lowest) return factor.
  const Position& worst(void) const throw(ReturnFactorsException);

  //! Return the longest series of consecutive positive positions.
  PositionSet max_cons_pos(void) const throw(ReturnFactorsException);
  //! Return the longest series of consecutive negative positions.
  PositionSet max_cons_neg(void) const throw(ReturnFactorsException);

  //! Return the sequence of consecutive positions experience the lowest realized drawdown.
  PositionSet dd(void) const throw(ReturnFactorsException);

protected:
  struct variance_bf : public std::binary_function<double, double, double> {
    variance_bf(double mean): _mean(mean) { }

    // accumulate() doesn't accumulate when using a custom binary_function...
    double operator()(double x, double y) { return x + ::pow(y - _mean, 2); }

  private:
    double _mean;
  };

  struct skew_bf: public std::binary_function<double, double, double> {
    skew_bf(double mean): _mean(mean) { }

    double operator()(double x, double y) { return x + ::pow(y - _mean, 3); }

  private:
    double _mean;
  };

  struct PositionGt: public std::binary_function<PositionPtr, double, bool> {

    PositionGt(double curr_price): _curr_price(curr_price) { }

    bool operator()(PositionPtr pPos, double x) const { return pPos->factor() > x; }

  private:
    double _curr_price;
  };

  struct PositionLt: public std::binary_function<PositionPtr, double, bool> {

    PositionLt(double curr_price): _curr_price(curr_price) { }

    bool operator()(PositionPtr pPos, double x) const { return pPos->factor() < x; }

  private:
    double _curr_price;
  };

  struct PositionLtCmp: public std::binary_function<PositionPtr, PositionPtr, bool> {

    PositionLtCmp(void) { }

    bool operator()(const PositionPtr pos1, const PositionPtr pos2) const { return pos1->factor() < pos2->factor(); }
  };

  struct PositionSetSizeCmp: public std::binary_function<PositionSet, PositionSet, bool> {

    bool operator()(const PositionSet& pset1, const PositionSet& pset2) const { return pset1.size() < pset2.size(); }
  };

  struct PositionSetRealizedCmp: public std::binary_function<PositionSet, PositionSet, bool> {

    bool operator()(const PositionSet& pset1, const PositionSet& pset2) const { return pset1.realized() < pset2.realized(); }
  };

  PositionSet _dd(PositionSet::by_last_exec::iterator& start) const;

protected:
  PositionSet _sPositions;

  typedef std::vector<double> doubleVector;
  doubleVector _vFactors; // time-ordered position factors for fast array calculations

  double _fvalue;			// future value
  double _mean;				// factors average
  double _stddev;			// factors standard deviation
};

#endif // _RETURNFACTORS_HPP_
