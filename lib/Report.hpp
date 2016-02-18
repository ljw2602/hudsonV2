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

#ifndef _REPORT_HPP_
#define _REPORT_HPP_

#ifdef WIN32
#pragma warning (disable:4290)
#endif

// C++
#include <iostream>

// STL
#include <string>

// Hudson
#include "ReturnFactors.hpp"
#include "Position.hpp"

//! Print ReturnFactors statistics.
class Report
{
public:
  Report(const ReturnFactors& rf);

  //! Print report title.
  static void header(const std::string& title = "");
  //! Set decimal digits precision.
  static void precision(unsigned decimals = 2);
  //! Return decimal digits precision.
  static unsigned get_precision(void);

  //! Print total number of factors, including open position factors.
  void trades(void) const { std::cout << "Trades: " << _rf.num() << std::endl; }
  //! Print average trade factor.
  void avg_trade(void) const { std::cout << "Avg trade: " << _rf.avg()*100 << '%' << std::endl; }
  //! Print standard deviation of factors.
  void std_dev(void) const { std::cout << "Std dev: " <<  _rf.stddev()*100 << '%' << std::endl; }
  //! Print factors skew.
  void skew(void) const { std::cout << "Skew: " << _rf.skew()*100 << std::endl; }
  //! Print factors 2 standard deviations range.
  void std_dev2_range(void) const { double stddev2 = _rf.stddev()*2; std::cout << "2SD Range: " << (_rf.avg() - stddev2)*100 << '%' << " | " << (_rf.avg() + stddev2)*100 << '%' << std::endl; }
  //! Print factors 3 standard deviations range.
  void std_dev3_range(void) const { double stddev3 = _rf.stddev()*3; std::cout << "3SD Range: " << (_rf.avg() - stddev3)*100 << '%' << " | " << (_rf.avg() + stddev3)*100 << '%' << std::endl; }
  //! Print number of positive factors.
  void pos_trades(void) const { std::cout << "Pos trades: " << _rf_pos.num() << " (" << _pos_percent*100 << "%)" << std::endl; }
  //! Print number of negative factors.
  void neg_trades(void) const { std::cout << "Neg trades: " << _rf_neg.num() << " (" << _neg_percent*100 << "%)" << std::endl; }
  //! Print average positive factor.
  void avg_pos(void) const { std::cout << "Avg pos: " << _rf_pos.avg()*100 << '%' << std::endl; }
  //! Print average negative factor.
  void avg_neg(void) const { std::cout << "Avg neg: " << _rf_neg.avg()*100 << '%' << std::endl; }
  //! Print best factor.
  void best(void) const;
  //! Print worst factor.
  void worst(void) const;
  //! Print number of maximum consecutive positive factors.
  void max_cons_pos(void) const;
  //! Print number of maximum consecutive negative factors.
  void max_cons_neg(void) const;
  //! Print maximum realized drawdown.
  void max_dd(void) const;
  //! Print total % return.
  void roi(void) const { std::cout << "ROI: " << _rf.roi()*100 << '%' << std::endl; }

  //! Print all statistics.
  void print(void) const;

private:
  const ReturnFactors& _rf;
  const ReturnFactors _rf_pos;
  const ReturnFactors _rf_neg;

  double _pos_percent;
  double _neg_percent;
  
private:
  void _begin_end(const Position& pos) const;
  void _begin_end(const PositionSet& pset) const;
};

#endif // _REPORT_HPP_
