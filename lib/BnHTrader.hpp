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

#ifndef _BNHTRADER_HPP_
#define _BNHTRADER_HPP_

// STL
#include <vector>

// Hudson
#include "EODSeries.hpp"
#include "Trader.hpp"

//! Buy and hold trader.
/*!
  BnHTrader is a simple trading simulation that only executes two trades: buy at the open on the
  first bar in the historical data series and sell at the last bar adjusted close.
  The main purpose of this class is to calculate benchmark statistics for a specific symbol.
  \see ReturnFactors
  \see Report
*/
class BnHTrader: public Trader
{
  typedef Series::EODSeries DB;

public:
  /*!
    Build a Buy and Hold trader.
    \param db historical data series.
  */
  BnHTrader(const DB& db);

  //! Run the trader and execute transactions.
  void run(void);

private:
  const DB& _db;
};

#endif // _BNHTRADER_HPP_
