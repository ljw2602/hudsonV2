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

#include "StdAfx.h"

// STL
#include <iostream>

// Hudson
#include "PortfolioReport.hpp"

using namespace std;


PortfolioReport::PortfolioReport( const PortfolioReturns& pr ):
  _pr(pr)
{

}


void PortfolioReport::print(void)
{
  streamsize curr_precision = cout.precision();
  ios_base::fmtflags curr_flags = cout.flags();
  
  cout.precision(2);
  cout.setf(ios::fixed);
  
  roi();
  cagr();
  gsd();
  sharpe();
  
  cout.precision(curr_precision);
  cout.flags(curr_flags);
}