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
#include "EOWSeries.hpp"


using namespace std;
using namespace boost::gregorian;


Series::EOWSeries::EOWSeries(const std::string& name):
  _name(name)
{
}


boost::gregorian::date_period Series::EOWSeries::period(void) const
{
  return boost::gregorian::date_period(ThisMap::begin()->first, ThisMap::rbegin()->first);
}


boost::gregorian::date_duration Series::EOWSeries::duration(void) const
{
  return boost::gregorian::date_duration(ThisMap::rbegin()->first - ThisMap::begin()->first);
}


long Series::EOWSeries::days(void) const
{
  return boost::gregorian::date_duration(ThisMap::rbegin()->first - ThisMap::begin()->first).days();
}


Series::EOWSeries::ThisMap::const_iterator Series::EOWSeries::at_or_before(const boost::gregorian::date& k) const
{
  ThisMap::const_iterator iter;
  if( (iter = ThisMap::find(k)) != ThisMap::end() )
    return iter;

  return before(k, 1);
}


Series::EOWSeries::ThisMap::const_iterator Series::EOWSeries::before(const boost::gregorian::date& k, unsigned recs) const
{
  ThisMap::const_iterator iter;
  if( (iter = ThisMap::lower_bound(k)) == ThisMap::begin() && recs > 0 )
    return ThisMap::end();

  for( unsigned i = 0; i < recs; i++ )
    if( --iter == ThisMap::begin() )
      return ThisMap::end();

  return iter;
}


Series::EOWSeries::ThisMap::const_iterator Series::EOWSeries::after(const boost::gregorian::date& k, unsigned recs) const
{
  ThisMap::const_iterator iter;
  if( (iter = ThisMap::find(k)) == ThisMap::end() ) {
    if( (iter = ThisMap::upper_bound(k)) == ThisMap::end() ) {
      return ThisMap::end();				// k out of range
    } else {
      // returning from upper_bound(), we are already one record past the key
      if( recs ) --recs;
    }
  }

  for( unsigned i = 0; i < recs; i++ )
    if( ++iter == ThisMap::end() )
      return ThisMap::end();

  return iter;
}


std::vector<double> Series::EOWSeries::open( const_iterator iter, unsigned long num ) const
{
  vector<double> v;

  if( iter == end() )
    return v;

  // reverse iterator init skips the first element in collection. We must manually insert the current element.
  v.insert(v.begin(), iter->second.open);
  unsigned i = 1;
  for( const_reverse_iterator rev_iter(iter); i < num && rev_iter != rend(); ++rev_iter, ++i )
    v.insert(v.begin(), rev_iter->second.open);

  return v; 
}


std::vector<double> Series::EOWSeries::open( void ) const
{
  vector<double> v;

  for( const_iterator iter(begin()); iter != end(); ++iter )
    v.push_back(iter->second.open);

  return v;
}


std::vector<double> Series::EOWSeries::open( const_iterator itbegin, const_iterator itend ) const
{
  vector<double> v;

  if( itbegin == itend || itbegin == end() )
    return v;

  for( const_iterator iter(itbegin); iter != itend; ++iter )
    v.push_back(iter->second.open);

  return v;
}


std::vector<double> Series::EOWSeries::close( const_iterator iter, unsigned long num ) const
{
  vector<double> v;

  if( iter == end() )
    return v;

  // reverse iterator init skips the first element in collection. We must manually insert the current element.
  v.insert(v.begin(), iter->second.close);
  unsigned i = 1;
  for( const_reverse_iterator rev_iter(iter); i < num && rev_iter != rend(); ++rev_iter, ++i )
    v.insert(v.begin(), rev_iter->second.close);

  return v;
}


std::vector<double> Series::EOWSeries::close( void ) const
{
  vector<double> v;

  for( const_iterator iter(begin()); iter != end(); ++iter )
    v.push_back(iter->second.close);

  return v;
}


std::vector<double> Series::EOWSeries::close( const_iterator itbegin, const_iterator itend ) const
{
  vector<double> v;

  if( itbegin == itend || itbegin == end() )
    return v;

  for( const_iterator iter(itbegin); iter != itend; ++iter )
    v.push_back(iter->second.close);

  return v;
}


std::vector<double> Series::EOWSeries::adjclose( const_iterator iter, unsigned long num ) const
{
  vector<double> v;

  if( iter == end() )
    return v;

  // reverse iterator init skips the first element in collection. We must manually insert the current element.
  v.insert(v.begin(), iter->second.adjclose);
  unsigned i = 1;
  for( const_reverse_iterator rev_iter(iter); i < num && rev_iter != rend(); ++rev_iter, ++i )
    v.insert(v.begin(), rev_iter->second.adjclose);

  return v;
}


std::vector<double> Series::EOWSeries::adjclose( void ) const
{
  vector<double> v;

  for( const_iterator iter(begin()); iter != end(); ++iter )
    v.push_back(iter->second.adjclose);

  return v;
}


std::vector<double> Series::EOWSeries::adjclose( const_iterator itbegin, const_iterator itend ) const
{
  vector<double> v;

  if( itbegin == itend || itbegin == end() )
    return v;

  for( const_iterator iter(itbegin); iter != itend; ++iter )
    v.push_back(iter->second.adjclose);

  return v;
}


std::vector<double> Series::EOWSeries::high( const_iterator iter, unsigned long num ) const
{
  vector<double> v;

  if( iter == end() )
    return v;

  // reverse iterator init skips the first element in collection. We must manually insert the current element.
  v.insert(v.begin(), iter->second.high);
  unsigned i = 1;
  for( const_reverse_iterator rev_iter(iter); i < num && rev_iter != rend(); ++rev_iter, ++i )
    v.insert(v.begin(), rev_iter->second.high);

  return v;
}


std::vector<double> Series::EOWSeries::high( void ) const
{
  vector<double> v;

  for( const_iterator iter(begin()); iter != end(); ++iter )
    v.push_back(iter->second.high);

  return v;
}


std::vector<double> Series::EOWSeries::high( const_iterator itbegin, const_iterator itend ) const
{
  vector<double> v;

  if( itbegin == itend || itbegin == end() )
    return v;

  for( const_iterator iter(itbegin); iter != itend; ++iter )
    v.push_back(iter->second.high);

  return v;
}


std::vector<double> Series::EOWSeries::low( const_iterator iter, unsigned long num ) const
{
  vector<double> v;

  if( iter == end() )
    return v;

  // reverse iterator init skips the first element in collection. We must manually insert the current element.
  v.insert(v.begin(), iter->second.low);
  unsigned i = 1;
  for( const_reverse_iterator rev_iter(iter); i < num && rev_iter != rend(); ++rev_iter, ++i )
    v.insert(v.begin(), rev_iter->second.low);

  return v;
}


std::vector<double> Series::EOWSeries::low( void ) const
{
  vector<double> v;

  for( const_iterator iter(begin()); iter != end(); ++iter )
    v.push_back(iter->second.low);

  return v;
}


std::vector<double> Series::EOWSeries::low( const_iterator itbegin, const_iterator itend ) const
{
  vector<double> v;

  if( itbegin == itend || itbegin == end() )
    return v;

  for( const_iterator iter(itbegin); iter != itend; ++iter )
    v.push_back(iter->second.low);

  return v;
}


std::vector<double> Series::EOWSeries::volume( const_iterator iter, unsigned long num ) const
{
  vector<double> v;

  if( iter == end() )
    return v;

  // reverse iterator init skips the first element in collection. We must manually insert the current element.
  v.insert(v.begin(), iter->second.volume);
  unsigned i = 1;
  for( const_reverse_iterator rev_iter(iter); i < num && rev_iter != rend(); ++rev_iter, ++i )
    v.insert(v.begin(), rev_iter->second.volume);

  return v;
}


std::vector<double> Series::EOWSeries::volume( void ) const
{
  vector<double> v;

  for( const_iterator iter(begin()); iter != end(); ++iter )
    v.push_back(iter->second.volume);

  return v;
}


std::vector<double> Series::EOWSeries::volume( const_iterator itbegin, const_iterator itend ) const
{
  vector<double> v;

  if( itbegin == itend || itbegin == end() )
    return v;

  for( const_iterator iter(itbegin); iter != itend; ++iter )
    v.push_back(iter->second.volume);

  return v;
}
