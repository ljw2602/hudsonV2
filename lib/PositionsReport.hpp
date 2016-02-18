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

#ifndef _POSITIONSREPORT_HPP_
#define _POSITIONSREPORT_HPP_

#ifdef WIN32
#pragma warning (disable:4290)
#endif

// C++
#include <iostream>

// STL
#include <string>

// Hudson
#include "PositionFactorsSet.hpp"
#include "SeriesFactor.hpp"

//! Positions report.
class PositionsReport
{
public:
  /*!
    PositionReport prints unrealized factors statistics for a set of positions.
    \param pf Daily factors data for a set of Positions.
    \see PositionFactorsSet.
  */
  PositionsReport(const PositionFactorsSet& pf);

  //! Print favorable, or unrealized peak, statistics.
  void favorable(void) const;
  //! Print adverse, or unrealized drawdown, statistics.
  void adverse(void) const;

  //! Print favorable and adverse statistics.
  void print(void) const;

private:
  const PositionFactorsSet& _pf;
};

#endif // _POSITIONSREPORT_HPP_
