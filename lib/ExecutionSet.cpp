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
#include "ExecutionSet.hpp"

using namespace std;

Execution::ID ExecutionSet::_eid = 0;

#include "Trader.hpp"
ExecutionSet::ExecutionSet(void)
{
}


void ExecutionSet::print(void) const
{
  for( by_date::iterator iter = get<date_key>().begin(); iter != get<date_key>().end(); ++iter ) {
	  cout << " ";
	  (*iter)->print();
  }
}


Execution::ID ExecutionSet::buy(const std::string& symbol, const boost::gregorian::date& dt, const Price& price, unsigned size)
{
  ExecutionPtr pExe(new BuyExecution(symbol, ++_eid, dt, price, size));
  if( insert(pExe).second == false )
    return Execution::NullID;

  notify(pExe); // Notify all observers about new execution

  return _eid;
}


Execution::ID ExecutionSet::sell(const std::string& symbol, const boost::gregorian::date& dt, const Price& price, unsigned size)
{
  ExecutionPtr pExe(new SellExecution(symbol, ++_eid, dt, price, size));
  if( insert(pExe).second == false )
    return Execution::NullID;

  notify(pExe);

  return _eid;
}


Execution::ID ExecutionSet::sell_short(const std::string& symbol, const boost::gregorian::date& dt, const Price& price, unsigned size)
{
  ExecutionPtr pExe(new SellShortExecution(symbol, ++_eid, dt, price, size));
  if( insert(pExe).second == false )
    return Execution::NullID;

  notify(pExe);

  return _eid;
}


Execution::ID ExecutionSet::cover(const std::string& symbol, const boost::gregorian::date& dt, const Price& price, unsigned size)
{
  ExecutionPtr pExe(new CoverExecution(symbol, ++_eid, dt, price, size));
  if( insert(pExe).second == false )
    return Execution::NullID;

  notify(pExe);

  return _eid;
}
