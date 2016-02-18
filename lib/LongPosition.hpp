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

#ifndef _LONGPOSITION_HPP_
#define _LONGPOSITION_HPP_

#ifdef WIN32
#pragma warning (disable:4290)
#endif

// Hudson
#include "Price.hpp"
#include "Position.hpp"

//! A new long position.
/*!
  LongPosition automatically generates a new buy Execution for a specific symbol/price/size. It runs a number of consistency checks to
  validate the price, date, and the specified execution ID. Only buy() and sell() transactions can be executed, other transaction types
  will generate an exception.
*/
class LongPosition: public Position
{
public:
  /*!
    All Execution parameters passed to the constructor are relative to the opening Execution.
      
    \param id A unique Execution ID.
    \param symbol Security for this position.
    \param dt Opening execution date.
    \param price Opening execution price.
    \param size Opening execution size.
    \see Execution.
  */
  LongPosition(ID id, const std::string& symbol, const boost::gregorian::date& dt, const Price& price, unsigned size) throw(PositionException);
  virtual ~LongPosition(void) { }

  virtual Type type(void) const { return LONG; }
  virtual std::string type_str(void) const { return "Long"; }

  //! The number of buy executions on this position.
  unsigned buys(void) const { return _buys; }
  //! The number of sell executions on this position.
  unsigned sells(void) const { return _sells; }
  
  //! Always throws an exception. LongPosition are not composite positions.
  //! \see StrategyPosition.
  virtual bool add(const PositionPtr pPos) throw(PositionException) { throw PositionException("Can't add position to long position"); }

  //! Average buy price. 
  virtual Price avgEntryPrice(void) const throw(PositionException) { return Price(_avgBuyPrice); }
  //! Average sell price.
  virtual Price avgExitPrice(void) const throw(PositionException) { return Price(_avgSellPrice); }

  //! Current return factor: average sell price divided by average buy price.
  virtual double factor(Series::EODDB::PriceType pt = Series::EODDB::ADJCLOSE) const throw(PositionException);
  //! Return factor from avgEntryPrice() to dt using PriceType pt.
  virtual double factor(const boost::gregorian::date& dt, Series::EODDB::PriceType pt = Series::EODDB::ADJCLOSE) const throw(PositionException);
  //! Return factor for period dp using PriceType pt.
  virtual double factor(const boost::gregorian::date_period& dp, Series::EODDB::PriceType pt = Series::EODDB::ADJCLOSE) const throw(PositionException);
  //! Return monthly factor for month/year period.
  virtual double factor(const boost::gregorian::date::month_type& month, const boost::gregorian::date::year_type& year, Series::EODDB::PriceType pt = Series::EODDB::ADJCLOSE) const throw(PositionException);

  //! Return daily factors. If the position is closed, return daily factors from the opening execution to the closing execution. If the Position is open,
  //! return daily factors from the first execution to the last available entry in the database.
  virtual SeriesFactorSet factors(Series::EODDB::PriceType pt = Series::EODDB::ADJCLOSE) const throw(PositionException);
  //! Return daily series factors until dt using PriceType pt.
  virtual SeriesFactorSet factors(const boost::gregorian::date& dt, Series::EODDB::PriceType pt = Series::EODDB::ADJCLOSE) const throw(PositionException);
  //! Return all daily factors for the period dp using PriceType pt.
  virtual SeriesFactorSet factors(const boost::gregorian::date_period& dp, Series::EODDB::PriceType pt = Series::EODDB::ADJCLOSE) const throw(PositionException);

  //! Add BuyExecution at specific price.
  virtual void buy(const boost::gregorian::date& dt, const Price& price, unsigned size) throw(PositionException);
  //! Add BuyExecution at database PriceType.
  virtual void buy(const boost::gregorian::date& dt, Series::EODDB::PriceType pt, unsigned size) throw(PositionException) { buy(dt, Price::get(_symbol, dt, pt), size); }

  //! Add SellExecution at specific price.
  virtual void sell(const boost::gregorian::date& dt, const Price& price, unsigned size) throw(PositionException);
  //! Add SellExecution at database PriceType.
  virtual void sell(const boost::gregorian::date& dt, Series::EODDB::PriceType pt, unsigned size) throw(PositionException) { sell(dt, Price::get(_symbol, dt, pt), size); }
  
  //! Throw an exception. LongPosition can not be sold short.
  virtual void sell_short(const boost::gregorian::date& dt, const Price& price, unsigned size) throw(PositionException);
  //! Throw an exception. LongPosition can not be sold short.
  virtual void sell_short(const boost::gregorian::date& dt, Series::EODDB::PriceType pt, unsigned size) throw(PositionException);
    
  //! Throw an exception. LongPosition can not be covered.
  virtual void cover(const boost::gregorian::date& dt, const Price& price, unsigned size) throw(PositionException);
  //! Throw an exception. LongPosition can not be covered.
  virtual void cover(const boost::gregorian::date& dt, Series::EODDB::PriceType pt, unsigned size) throw(PositionException);
    
  //! Close any open size by adding a SellExecution (s).
  virtual void close(const boost::gregorian::date& dt, const Price& price) throw(PositionException);
  //! Close any open size on dt at PriceType.
  virtual void close(const boost::gregorian::date& dt, Series::EODDB::PriceType pt) throw(PositionException) { close(dt, Price::get(_symbol, dt, pt)); }

private:
  unsigned _buys;
  unsigned _sells;

  double _avgBuyPrice;
  double _avgSellPrice;
};

#endif // _LONGPOSITION_HPP_
