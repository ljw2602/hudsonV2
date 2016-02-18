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

#ifndef _EOMREPORT_HPP_
#define _EOMREPORT_HPP_

#ifdef WIN32
#pragma warning (disable:4290)
#endif

// STL
#include <iostream>

// Hudson
#include "Report.hpp"
#include "EOMReturnFactors.hpp"

//! Print Month-to-Month factors statistics. \see EOMReturnFactors.
class EOMReport: public Report
{
public:
  EOMReport(const EOMReturnFactors& rf);

  //! Print Compounded Annualized Growth Rate.
  void cagr(void) const { std::cout << "CAGR: " << _eomrf.cagr()*100 << '%' << std::endl; }
  //! Print Geometric Standard Deviation of Monthly returns.
  void gsdm(void) const { std::cout << "GSDm: " << _eomrf.gsd()*100 << '%' << std::endl; }
  //! Print Sharpe ratio.
  void sharpe(void) const { std::cout << "Sharpe: " << _eomrf.sharpe() << std::endl; }

  //! Print all EOMReport statistics.
  void print(void) const;

private:
  const EOMReturnFactors& _eomrf;
};


#endif // _EOMREPORT_HPP_
