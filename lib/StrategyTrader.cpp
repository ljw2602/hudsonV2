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

// Hudson
#include "StrategyTrader.hpp"
#include "StrategyPosition.hpp"

using namespace std;
using namespace boost::gregorian;
using namespace boost::multi_index;
using namespace Series;


Position::ID StrategyTrader::strategy( const std::string& symbol, PositionPtr pPos ) throw(TraderException)
{
  PositionPtr pStratPos; // The new StrategyPosition

  try {

    pStratPos = PositionPtr(new StrategyPosition(++_pid, symbol, pPos));

  } catch (const std::exception& ex) {

    throw TraderException(ex.what());
  }

  // Add new position to global collection
  if( _miPositions.insert(pStratPos).first == _miPositions.end() )
    throw TraderException("Can't add new strategy position");

  // Return new position ID
  return _pid; 
}


Position::ID StrategyTrader::strategy_buy( const std::string& strat_symbol, const std::string& pos_symbol, const boost::gregorian::date& dt, const Price& price, unsigned size /*= 1*/ ) throw(TraderException)
{
  Position::ID long_pos_id = buy(pos_symbol, dt, price, size);
  PositionSet::const_iterator long_pos_citer = _miPositions.find(long_pos_id, pos_comp_id());
  if( long_pos_citer == _miPositions.end() )
    throw TraderException("Can't find newly created LongPosition after strategy buy");
    
  PositionPtr pLongPos = *long_pos_citer;
  return strategy(strat_symbol, pLongPos);
}


Position::ID StrategyTrader::strategy_buy( Position::ID strat_id, const std::string& pos_symbol, const boost::gregorian::date& dt, const Price& price, unsigned size /*= 1*/ ) throw(TraderException)
{
  // Get StrategyPosition pointer
  PositionSet::const_iterator strat_pos_citer = _miPositions.find(strat_id, pos_comp_id());
  if( strat_pos_citer == _miPositions.end() )
    throw TraderException("Can't find strategy position");
    
  PositionPtr pStratPos = *strat_pos_citer;
  
  // Execute new LongPosition opening transaction
  Position::ID long_pos_id = buy(pos_symbol, dt, price, size);

  // Add new LongPosition to StrategyPosition
  PositionSet::const_iterator long_pos_citer = _miPositions.find(long_pos_id, pos_comp_id());
  if( long_pos_citer == _miPositions.end() )
    throw TraderException("Can't find newly create long position");
    
  PositionPtr pLongPos = *long_pos_citer;
  pStratPos->add(pLongPos);
  
  // Return new LongPosition id
  return long_pos_id;
}


Position::ID StrategyTrader::strategy_sell_short(const std::string& strat_symbol, const std::string& pos_symbol, const boost::gregorian::date& dt, const Price& price, unsigned size /*= 1*/) throw(TraderException)
{
  Position::ID short_pos_id = sell_short(pos_symbol, dt, price, size);
  PositionSet::const_iterator short_pos_citer = _miPositions.find(short_pos_id, pos_comp_id());
  if( short_pos_citer == _miPositions.end() )
    throw TraderException("Can't find newly created ShortPosition after strategy sell short");

  PositionPtr pShortPos = *short_pos_citer;
  return strategy(strat_symbol, pShortPos);
}


Position::ID StrategyTrader::strategy_sell_short( Position::ID strat_id, const std::string& pos_symbol, const boost::gregorian::date& dt, const Price& price, unsigned size /*= 1*/ ) throw(TraderException)
{
  // Get StrategyPosition pointer
  PositionSet::const_iterator strat_pos_citer = _miPositions.find(strat_id, pos_comp_id());
  if( strat_pos_citer == _miPositions.end() )
    throw TraderException("Can't find strategy position");

  PositionPtr pStratPos = *strat_pos_citer;

  // Execute new ShortPosition opening transaction
  Position::ID short_pos_id = sell_short(pos_symbol, dt, price, size);
  PositionSet::const_iterator short_pos_citer = _miPositions.find(short_pos_id, pos_comp_id());
  if( short_pos_citer == _miPositions.end() )
    throw TraderException("Can't find newly created short position");

  PositionPtr pShortPos = *short_pos_citer;

  pStratPos->add(pShortPos);
  
  return short_pos_id;
}


void StrategyTrader::strategy_close( Position::ID strat_id, const boost::gregorian::date& dt, const EODDB::PriceType pt ) throw(TraderException)
{
  // Find existing position
  PositionSet::const_iterator iter = _miPositions.find(strat_id, pos_comp_id());
  if( iter == _miPositions.end() )
    throw TraderException("Can't find position");

  // Close position
  PositionPtr pPos = *iter;
  try {

    pPos->close(dt, pt);

  } catch( const std::exception& ex ) {

    throw TraderException(ex.what());
  }

  // Update existing position
  if( _miPositions.replace(iter, pPos) == false )
    throw TraderException("Can't update position");
}
