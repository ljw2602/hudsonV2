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

#ifndef _POSITIONSET_HPP_
#define _POSITIONSET_HPP_

#ifdef WIN32
#pragma warning (disable:4290)
#endif

// STL
#include <string>

// Boost
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/mem_fun.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/tag.hpp>

// Hudson
#include "Position.hpp"
#include "Price.hpp"


struct symbol_key { };
struct first_exec_key { };
struct last_exec_key { };

typedef boost::multi_index::multi_index_container<

  PositionPtr,

  boost::multi_index::indexed_by<
    boost::multi_index::ordered_unique<boost::multi_index::identity<Position> >,
    boost::multi_index::ordered_non_unique<boost::multi_index::tag<symbol_key>, boost::multi_index::const_mem_fun<Position, std::string, &Position::symbol> >,
    boost::multi_index::ordered_non_unique<boost::multi_index::tag<first_exec_key>, boost::multi_index::const_mem_fun<Position, const ExecutionPtr, &Position::first_exec> >,
    boost::multi_index::ordered_non_unique<boost::multi_index::tag<last_exec_key>, boost::multi_index::const_mem_fun<Position, const ExecutionPtr, &Position::last_exec> >
  >
> __PositionSet;

/*!
  PositionSet is a multi index PositionPtr collection indexed by Position ID,
  symbol, first Position execution (opening transaction for first Position in time)
  or last Position execution (closing transaction for last Position in time).
  A Position can be retrieved by ID or by symbol. Example:
  
  Look for position by ID:
  PositionSet::iterator iter = positions.find(id, pos_comp_id());

  Look for position by symbol:
  typedef PositionSet::index<symbol_key>::type position_set_by_symbol;
  position_set_by_symbol::iterator it = _miPositions.get<symbol_key>().find("SP500");
  
  \see Position
*/
class PositionSet: private __PositionSet
{
public:
  typedef __PositionSet::iterator                     iterator;
  typedef __PositionSet::const_iterator               const_iterator;
  typedef __PositionSet::index<symbol_key>::type      by_symbol;
  typedef __PositionSet::index<first_exec_key>::type  by_first_exec;
  typedef __PositionSet::index<last_exec_key>::type   by_last_exec;

  using __PositionSet::get;
  using __PositionSet::key_extractor;
  using __PositionSet::size;
  using __PositionSet::empty;
  using __PositionSet::begin;
  using __PositionSet::end;
  using __PositionSet::insert;
  using __PositionSet::find;
  using __PositionSet::replace;

public:
  //! Return all closed positions.
  PositionSet closed(void) const;
  //! Return all open positions.
  PositionSet open(void) const;
  //! Return all long positions.
  //! \see LongPosition
  PositionSet longPos(void) const;
  //! Return all short positions.
  //! \see ShortPosition
  PositionSet shortPos(void) const;
  //! Return natural positions, ie not synthetic.
  PositionSet naturalPos(void) const;
  //! Return synthetic positions.
  //! \see StrategyPosition
  PositionSet stratPos(void) const;
  //! Return all closed positions for symbol.
  PositionSet closed(const std::string& symbol);
  //! Return all open positions for symbol.
  PositionSet open(const std::string& symbol);
  //! Return all closed positions factor.
  double realized(void) const;
  //! Return all open positions factor.
  double unrealized(void) const;

  //! Print all positions data.
  void print(void) const;
};


struct pos_comp_id
{
  bool operator()(Position::ID id, Position& p) const { return id < p.id(); }
  bool operator()(Position& p, Position::ID id) const { return p.id() < id; }
};

#endif // _POSITIONSET_HPP_
