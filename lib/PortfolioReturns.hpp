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

#ifndef _PORTFOLIORETURNS_HPP_
#define _PORTFOLIORETURNS_HPP_

#ifdef WIN32
#pragma warning (disable:4290)
#endif

// STL
#include <vector>

// Hudson
#include "EOMReturnFactors.hpp"

class PortfolioReturnsException: public std::exception
{
public:
  PortfolioReturnsException(const std::string& msg):
      _Str("PortfolioReturnsException: ")
      {
        _Str += msg;
      }

      virtual ~PortfolioReturnsException(void) throw() { }
      virtual const char *what(void) const throw() { return _Str.c_str(); }

protected:
  std::string _Str;
};


//! Aggregate EOMReturns for multiple securities and calculate Portfolio statistics.
class PortfolioReturns
{
public:
  PortfolioReturns(void);
  
  //! Returns Return On Investments.
  double roi(void) const;
  //! Returns Compounded Annualized Growth Rate.
  double cagr(void) const;
  //! Returns Geometric Standard Deviation of Monthly Returns.
  double gsd(void) const;
  //! Returns Sharpe Ratio
  double sharpe(void) const;
  
  //! Add EOMReturnFactors for a specific symbol.
  /*!
    A weight coefficient in the range 0-1 should be specified for each EOMReturnFactors. Portfolio
    return statistics will be calculated according to the weight specified for each EOMReturnFactors object.
    If all objects inserted have 0 weight, then all EOMReturnFactors are assumed to have the same weight.
    \param rf Month-to-Month returns for a specific symbol.
    \param weight The symbol weight in the portfolio. Valid values are in 0-1 range.
  */
  void add(EOMReturnFactors* rf, double weight = 0) throw(PortfolioReturnsException);
  
  //! Returns the number of EOMReturnFactors included in this PortfolioReturns.
  unsigned series(void) const { return (unsigned)_vRF.size(); }
  
protected:
  struct EOMRFWeight
  {
    EOMRFWeight(EOMReturnFactors* pEOMRF_, double w_);
    
    EOMReturnFactors* pEOMRF;
    double w;
  };
  
  std::vector<EOMRFWeight> _vRF;
  double _accWeight;
};

#endif // _PORTFOLIORETURNS_HPP_
