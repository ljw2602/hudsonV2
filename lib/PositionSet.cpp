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

#include "StdAfx.h"

// STL
#include <iostream>
#include <functional>
#include <algorithm>

// Hudson
#include "PositionSet.hpp"

using namespace std;


void PositionSet::print(void) const
{
  for( const_iterator iter = begin(); iter != end(); ++iter ) {
    (*iter)->print();
    cout << endl;
  }
}


PositionSet PositionSet::closed(void) const
{
  PositionSet closedPos;

  for( const_iterator iter = begin(); iter != end(); ++iter )
    if( (*iter)->closed() )
      closedPos.insert(*iter);

  return closedPos;
}


PositionSet PositionSet::closed( const std::string& symbol )
{
  PositionSet closedPos;

  PositionSet::by_symbol::const_iterator symbol_key_end = get<symbol_key>().upper_bound(symbol);

  for( PositionSet::by_symbol::const_iterator iter = get<symbol_key>().lower_bound(symbol);
        iter != symbol_key_end; ++iter )
    if( (*iter)->closed() )
      closedPos.insert(*iter);

  return closedPos;
}


PositionSet PositionSet::open(void) const
{
  PositionSet openPos;

  for( const_iterator iter = begin(); iter != end(); ++iter )
    if( (*iter)->open() )
      openPos.insert(*iter);

  return openPos;
}


PositionSet PositionSet::open( const std::string& symbol )
{
  PositionSet openPos;

  PositionSet::by_symbol::const_iterator symbol_key_end = get<symbol_key>().upper_bound(symbol);

  for( PositionSet::by_symbol::const_iterator iter = get<symbol_key>().lower_bound(symbol);
        iter != symbol_key_end; ++iter )
    if( (*iter)->open() )
      openPos.insert(*iter);

  return openPos;
}


double PositionSet::realized(void) const
{
  PositionSet closedPositions = closed();
  if( closedPositions.empty() )
    return 1;

  double acc = 1;
  for( const_iterator iter = closedPositions.begin(); iter != closedPositions.end(); ++iter )
    acc *= (*iter)->factor();

  return acc;
}


double PositionSet::unrealized(void) const
{
  PositionSet openPositions = open();
  if( openPositions.empty() )
    return 1;

  double acc = 1;
  for( const_iterator iter = openPositions.begin(); iter != openPositions.end(); ++iter )
    acc *= (*iter)->factor();

  return acc;
}


PositionSet PositionSet::longPos( void ) const
{
  PositionSet sPos;

  for( const_iterator iter = begin(); iter != end(); ++iter )
    if( (*iter)->type() == Position::LONG )
      sPos.insert(*iter);

  return sPos;
}


PositionSet PositionSet::shortPos( void ) const
{
  PositionSet sPos;

  for( const_iterator iter = begin(); iter != end(); ++iter )
    if( (*iter)->type() == Position::SHORT )
      sPos.insert(*iter);

  return sPos;
}


PositionSet PositionSet::stratPos( void ) const
{
  PositionSet sPos;

  for( const_iterator iter = begin(); iter != end(); ++iter )
    if( (*iter)->type() == Position::STRATEGY )
      sPos.insert(*iter);

  return sPos;
}


PositionSet PositionSet::naturalPos(void) const
{
  PositionSet sPos;

  for( const_iterator iter = begin(); iter != end(); ++iter )
    if( (*iter)->type() != Position::STRATEGY )
      sPos.insert(*iter);

  return sPos;
}
