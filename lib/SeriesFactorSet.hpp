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

#ifndef _SERIESFACTORSET_HPP_
#define _SERIESFACTORSET_HPP_

#ifdef WIN32
#pragma warning (disable:4290)
#endif

// STL
#include <stdexcept>
#include <string>

// Boost
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/mem_fun.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/tag.hpp>

// Hudson
#include "SeriesFactor.hpp"


struct from_key {};
struct to_key {};

typedef boost::multi_index::multi_index_container<

  SeriesFactor,

  boost::multi_index::indexed_by<
    boost::multi_index::ordered_unique<boost::multi_index::identity<SeriesFactor> >,
    boost::multi_index::ordered_non_unique<boost::multi_index::tag<from_key>, boost::multi_index::const_mem_fun<SeriesFactor, const boost::gregorian::date&, &SeriesFactor::from_tm> >,
    boost::multi_index::ordered_non_unique<boost::multi_index::tag<to_key>, boost::multi_index::const_mem_fun<SeriesFactor, const boost::gregorian::date&, &SeriesFactor::to_tm> >
  >
> __SeriesFactorSet;


class SeriesFactorSetException: public std::exception
{
public:
  SeriesFactorSetException(const std::string& msg):
    _Str("SeriesFactorSetException: ")
  {
    _Str += msg;
  }

  virtual ~SeriesFactorSetException(void) throw() { }
  virtual const char *what() const throw() { return _Str.c_str(); }

protected:
  std::string _Str;
};

//! A collection of SeriesFactor objects.
/*!
  Calculates the overall return factor for all included SeriesFactor objects.
*/
class SeriesFactorSet: private __SeriesFactorSet
{
public:
  typedef __SeriesFactorSet::iterator iterator;
  typedef __SeriesFactorSet::const_iterator const_iterator;
  typedef __SeriesFactorSet::index<from_key>::type by_from;
  typedef __SeriesFactorSet::index<to_key>::type by_to;

  using __SeriesFactorSet::get;
  using __SeriesFactorSet::size;
  using __SeriesFactorSet::empty;
  using __SeriesFactorSet::begin;
  using __SeriesFactorSet::end;

public:
  SeriesFactorSet(void);

  //! Override insert() to calculate factor value after inserting a new SeriesFactor.
  bool insert(const SeriesFactor& sf);

  //! Return factor value.
  double factor(void) const throw(SeriesFactorSetException);

private:
  double _factor;
};

typedef SeriesFactorSet::by_from series_factor_by_begin_mark;
typedef SeriesFactorSet::by_to series_factor_by_end_mark;

#endif // _SERIESFACTORSET_HPP_
