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

// C++
#include <stdexcept>

// STL
#include <iostream>
#include <string>
#include <sstream>

// Hudson
#include "Trader.hpp"
#include "Price.hpp"
#include "LongPosition.hpp"
#include "ShortPosition.hpp"
#include "StrategyPosition.hpp"


using namespace std;
using namespace boost::gregorian;
using namespace boost::multi_index;

Position::ID Trader::_pid = 0;


Trader::Trader(void)
{
}


// Buy a new position
Position::ID Trader::buy(const string& symbol, const date& dt, const Price& price, unsigned size) throw(TraderException)
{
  // Create new position
  PositionPtr pPos;

  // Buy position
  try {

    pPos = PositionPtr(new LongPosition(++_pid, symbol, dt, price, size));

  } catch( const exception& ex ) {

    throw TraderException(ex.what());
  }

  // Add new position to trader collection
  if( _miPositions.insert(pPos).first == _miPositions.end() )
    throw TraderException("Can't add new long position");

  // Return new position ID
  return _pid;
}


// Add buy execution to an existing position
void Trader::buy(Position::ID id, const boost::gregorian::date& dt, const Price& price, unsigned size) throw(TraderException)
{
  // Find existing position
  PositionSet::const_iterator iter = _miPositions.find(id, pos_comp_id());
  if( iter == _miPositions.end() )
    throw TraderException("Can't find position");

  // Add buy transaction to existing position
  PositionPtr pPos = *iter;
  try {

    pPos->buy(dt, price, size);

  } catch( const exception& ex ) {

    throw TraderException(ex.what());
  }
}


// Sell an existing long position
void Trader::sell(Position::ID id, const date& dt, const Price& price, unsigned size) throw(TraderException)
{
  // Find existing position
  PositionSet::const_iterator iter = _miPositions.find(id, pos_comp_id());
  if( iter == _miPositions.end() )
    throw TraderException("Can't find position");

  // Add sell transaction to existing position
  PositionPtr pPos = *iter;
  try {

    pPos->sell(dt, price, size);

  } catch( const exception& ex ) {

    throw TraderException(ex.what());
  }
}


Position::ID Trader::sell_short(const string& symbol, const date& dt, const Price& price, unsigned size) throw(TraderException)
{
  PositionPtr pPos; 

  try {

    pPos = PositionPtr(new ShortPosition(++_pid, symbol, dt, price, size));

  } catch( const exception& ex ) {

    throw TraderException(ex.what());
  }

  if( _miPositions.insert(pPos).first == _miPositions.end() )
  	throw TraderException("Can't add short position");

  return _pid;
}


void Trader::sell_short(Position::ID id, const date& dt, const Price& price, unsigned size) throw(TraderException)
{
  // Find existing position
  PositionSet::const_iterator iter = _miPositions.find(id, pos_comp_id());
  if( iter == _miPositions.end() )
	  throw TraderException("Can't find position");

  // Add short execution to existing position
  PositionPtr pPos = *iter;
  try {

    pPos->sell_short(dt, price, size);

  } catch( const exception& ex ) {

    throw TraderException(ex.what());
  }
}


void Trader::cover(Position::ID id, const date& dt, const Price& price, unsigned size) throw(TraderException)
{
  // Find existing position
  PositionSet::const_iterator iter = _miPositions.find(id, pos_comp_id());
  if( iter == _miPositions.end() )
	  throw TraderException("Can't find position");

  // Add cover execution to existing position
  PositionPtr pPos = *iter;
  try {

    pPos->cover(dt, price, size);

  } catch( const exception& ex ) {

    throw TraderException(ex.what());
  }
}


void Trader::close(Position::ID id, const date& dt, const Price& price) throw(TraderException)
{
  // Find existing position
  PositionSet::const_iterator iter = _miPositions.find(id, pos_comp_id());
  if( iter == _miPositions.end() )
	  throw TraderException("Can't find position");

  // Close position
  PositionPtr pPos = *iter;
  try {

    pPos->close(dt, price);

  } catch( const exception& ex ) {

    throw TraderException(ex.what());
  }
}


PositionSet Trader::positions( const std::string& symbol )
{
  PositionSet psSymbol;

  for( PositionSet::iterator iter = _miPositions.begin(); iter != _miPositions.end(); ++iter )
    if( symbol == (*iter)->symbol() )
      psSymbol.insert(*iter);

  return psSymbol;
}


PositionPtr Trader::get( Position::ID id ) const throw(TraderException)
{
  PositionSet::const_iterator citer = _miPositions.find(id, pos_comp_id());
  if( citer == _miPositions.end() ) {
    stringstream ss;
    ss << "Can't find position id " << id;
    throw TraderException(ss.str());
  }
  
  return *citer;
}
