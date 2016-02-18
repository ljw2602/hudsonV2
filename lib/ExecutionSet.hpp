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

#ifndef _EXECUTIONSET_HPP_
#define _EXECUTIONSET_HPP_

#ifdef WIN32
#pragma warning (disable:4290)
#endif

// Boost
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/mem_fun.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/tag.hpp>

// Hudson
#include "Execution.hpp"
#include "ExecutionNotifier.hpp"

struct date_key { };
struct side_key { };

typedef boost::multi_index::multi_index_container<

  ExecutionPtr,

  boost::multi_index::indexed_by<
    boost::multi_index::ordered_unique<boost::multi_index::identity<Execution> >,
    boost::multi_index::ordered_non_unique<boost::multi_index::tag<date_key>, boost::multi_index::const_mem_fun<Execution, boost::gregorian::date, &Execution::dt> >,
    boost::multi_index::ordered_non_unique<boost::multi_index::tag<side_key>, boost::multi_index::const_mem_fun<Execution, Execution::Side, &Execution::side> >
    >
  > __ExecutionSet;


/*!
  ExecutionSet is a multi index collection containing a set of transactions, indexed by Execution ID,
  execution date and execution side.
  \see Execution
*/
class ExecutionSet: private __ExecutionSet, public ExecutionNotifier
{
public:
  typedef __ExecutionSet::iterator iterator;
  typedef __ExecutionSet::const_iterator const_iterator;

  using __ExecutionSet::size;
  using __ExecutionSet::empty;
  using __ExecutionSet::begin;
  using __ExecutionSet::end;
  using __ExecutionSet::insert;
  using __ExecutionSet::find;

public:
  ExecutionSet(void);
  ~ExecutionSet(void) { }

  //! Add a long transaction.
  Execution::ID buy(const std::string& symbol, const boost::gregorian::date& dt, const Price& price, unsigned size);
  //! Add a sell transaction. 
  Execution::ID sell(const std::string& symbol, const boost::gregorian::date& dt, const Price& price, unsigned size);
  //! Add a short transaction.
  Execution::ID sell_short(const std::string& symbol, const boost::gregorian::date& dt, const Price& price, unsigned size);
  //! Add a cover transaction.
  Execution::ID cover(const std::string& symbol, const boost::gregorian::date& dt, const Price& price, unsigned size);

  //! Return the first Execution by date.
  //! \note Execution is a pure virtual class, return type must be a reference or a pointer
  const ExecutionPtr first_by_date(void) const { return *(get<date_key>().begin()); }
  //! Return the last Execution by date.
  //! \note Execution is a pure virtual class, return type must be a reference or a pointer
  const ExecutionPtr last_by_date(void) const { return *(get<date_key>().rbegin()); }

  //! Print all Execution data in this collection.
  void print(void) const;

private:
  //! Unique execution identifier
  static Execution::ID _eid;

private:
  typedef __ExecutionSet::index<side_key>::type by_side;
  typedef __ExecutionSet::index<date_key>::type by_date;  
};

#endif // _EXECUTIONSET_HPP_
