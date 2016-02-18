/*
* Copyright (C) 2007,2008 A. Giannetti
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

#ifndef _EXECUTION_HPP_
#define _EXECUTION_HPP_

#ifdef WIN32
#pragma warning (disable:4290)
#endif

// STL
#include <set>
#include <string>

// Boost
#include <boost/date_time/gregorian/gregorian.hpp>

// Hudson
#include "Price.hpp"


class Execution
{
public:
  enum Side { //! Execution type.
    BUY,
    SELL,
    SHORT,
    COVER
  };

  typedef unsigned long ID;
  static const ID NullID;

public:
  virtual ~Execution(void) { }

  //! Executions will be ordered by their ID.
  bool operator<(const Execution& e) const { return _id < e._id; }

  //! Execution Position
  std::string symbol(void) const { return _symbol; }

  //! Execution unique identifier.
  ID id(void) const { return _id; }
  boost::gregorian::date dt(void) const { return _dt; }
  Price price(void) const { return _price; }
  unsigned size(void) const { return _size; }

  //! Print all execution data.
  void print(void) const;
  
  //! Simple string describing the execution side.
  virtual std::string action(void) const = 0;
  //! Execution side as a numeric identifier.
  virtual Side side(void) const = 0;

protected:
  Execution(const std::string& symbol, ID id, const boost::gregorian::date& dt, const Price& price, unsigned size);

  const std::string _symbol;
  const ID _id;
  const boost::gregorian::date _dt;
  const Price _price;
  const unsigned _size;
};


class BuyExecution: public Execution
{
public:
  BuyExecution(const std::string& symbol, Execution::ID id, const boost::gregorian::date& dt, const Price& price, unsigned size);

  virtual std::string action(void) const { return "Bought"; }
  virtual Side side(void) const { return BUY; }
};


class SellExecution: public Execution
{
public:
  SellExecution(const std::string& symbol, Execution::ID id, const boost::gregorian::date& dt, const Price& price, unsigned size);

  virtual std::string action(void) const { return "Sold"; }
  virtual Side side(void) const { return SELL; }
};


class SellShortExecution: public Execution
{
public:
  SellShortExecution(const std::string& symbol, Execution::ID id, const boost::gregorian::date& dt, const Price& price, unsigned size);

  virtual std::string action(void) const { return "Shorted"; }
  virtual Side side(void) const { return SHORT; }
};


class CoverExecution: public Execution
{
public:
  CoverExecution(const std::string& symbol, Execution::ID id, const boost::gregorian::date& dt, const Price& price, unsigned size);

  virtual std::string action(void) const { return "Covered"; }
  virtual Side side(void) const { return COVER; }
};

#endif // _EXECUTION_HPP_
