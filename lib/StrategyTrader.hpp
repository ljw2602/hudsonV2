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

#ifndef _STRATEGYTRADER_HPP_
#define _STRATEGYTRADER_HPP_

#ifdef WIN32
#pragma warning (disable:4290)
#endif

// STL
#include <string>

// Boost
#include <boost/date_time/gregorian/gregorian.hpp>

// Hudson
#include "Trader.hpp"
#include "EODDB.hpp"


class StrategyTrader: public Trader
{
public:
  StrategyTrader(void) { }
  virtual ~StrategyTrader(void) { }
  
  /*!
  \brief Creates a new StrategyPosition.
  \return The new StrategyPosition identifier.
  \param symbol The name of the new StrategyPosition being added.
  \param pPos The first position added to the strategy.
  \see StrategyPosition.
  */
  Position::ID strategy(const std::string& symbol, PositionPtr pPos) throw(TraderException);
  
  /*!
  \brief Create a new StrategyPosition by opening a new LongPosition.
  \return The new StrategyPosition identifier.
  \param strat_symbol The new StrategyPosition name.
  \param pos_symbol The new LongPosition name.
  \param dt Transaction date.
  \param price Transaction Price.
  \param size Transaction size.
  */
  Position::ID strategy_buy(const std::string& strat_symbol, const std::string& pos_symbol, const boost::gregorian::date& dt, const Price& price, unsigned size = 1) throw(TraderException);  
  /*!
  \brief Add a new LongPosition to an existing StrategyPosition.
  \param strat_id The StrategyPosition identifier.
  \param pos_symbol The new LongPosition name.
  \param dt New LongPosition transaction date.
  \param price New LongPosition transaction Price.
  \param size New LongPosition size.
  */
  Position::ID strategy_buy(Position::ID strat_id, const std::string& pos_symbol, const boost::gregorian::date& dt, const Price& price, unsigned size = 1) throw(TraderException);
  
  /*!
  \brief Create a new StrategyPosition by opening a new ShortPosition.
  \return The new StrategyPosition identifier.
  \param symbol The name of the new ShortPosition.
  \param dt Transaction date.
  \param price Transaction Price.
  \param size Transaction size.
  */ 
  Position::ID strategy_sell_short(const std::string& strat_symbol, const std::string& pos_symbol, const boost::gregorian::date& dt, const Price& price, unsigned size = 1) throw(TraderException);  
  /*!
  \brief Add a new ShortPosition to an existing StrategyPosition.
  \param strat_id The StrategyPosition identifier.
  \param pos_symbol The new ShortPosition name.
  \param dt The new ShortPosition transaction date.
  \param price The new ShortPosition transaction Price.
  \param size The new ShortPosition size.
  */
  Position::ID strategy_sell_short(Position::ID strat_id, const std::string& pos_symbol, const boost::gregorian::date& dt, const Price& price, unsigned size = 1) throw(TraderException);
  
  /*!
  \brief Close all the underlying Position.
  \param strat_id StrategyPosition identifier.
  \param dt Close transaction date.
  \param pt Close transaction PriceType.
  */
  void strategy_close(Position::ID strat_id, const boost::gregorian::date& dt, const Series::EODDB::PriceType pt) throw(TraderException);
};

#endif // _STRATEGYTRADER_HPP_
