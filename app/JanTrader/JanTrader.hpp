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

#ifndef _JANTRADER_HPP_
#define _JANTRADER_HPP_

// STL
#include <string>
#include <vector>

// Boost
#include <boost/date_time/gregorian/gregorian.hpp>

// Hudson
#include <StrategyTrader.hpp>


class JanTrader: public StrategyTrader
{
public:
  JanTrader(const std::string& long_symbol,  const std::string& hedge_symbol);

  void run(int entry_offset, int exit_offset) throw(TraderException);

private:
  const std::string _long_symbol;
  const std::string _hedge_symbol;
};


#endif // _JANTRADER_HPP_
