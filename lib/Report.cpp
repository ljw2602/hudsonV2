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

// STL
#include <iostream>

// Hudson
#include "Report.hpp"

using namespace std;


void Report::header(const std::string& title /* =  */)
{
  cout << endl << title << endl << "--" << endl;
}


void Report::precision(unsigned decimals)
{
  cout.precision(2);
  cout.setf(ios::fixed);
}


unsigned Report::get_precision(void)
{
  return cout.precision();
}


Report::Report(const ReturnFactors& rf):
  _rf(rf),
  _rf_pos(rf.pos()),
  _rf_neg(rf.neg()),
  _pos_percent(0),
  _neg_percent(0)
{
  if( _rf.num() == 0 ) return;
  if( _rf_pos.num() ) _pos_percent = _rf_pos.num() / (double)rf.num();
  if( _rf_neg.num() ) _neg_percent = _rf_neg.num() / (double)rf.num();
}


void Report::print(void) const
{
  streamsize curr_precision = cout.precision();
  ios_base::fmtflags curr_flags = cout.flags();

  cout.precision(2);
  cout.setf(ios::fixed);

  trades();
  avg_trade();
  std_dev();
  skew();
  std_dev2_range();
  std_dev3_range();
  pos_trades();
  neg_trades();
  avg_pos();
  avg_neg();
  best();
  worst();
  max_cons_pos();
  max_cons_neg();
  max_dd();

  cout << endl;
  roi();

  cout.precision(curr_precision);
  cout.flags(curr_flags);
}


void Report::best(void) const
{
  if( _rf.num() == 0 )
    return; // avoid exception during report

  const Position& pos = _rf.best();
  cout << "Best: " << (pos.factor()-1)*100 << '%';
  _begin_end(pos);
}


void Report::worst(void) const
{
  if( _rf.num() == 0 )
    return; // avoid exception during report

  const Position& pos = _rf.worst();
  cout << "Worst: " << (pos.factor()-1)*100 << '%';
  _begin_end(pos);
}


void Report::max_cons_pos( void ) const
{
  if( _rf.num() == 0 )
    return; // avoid exception during report

  PositionSet pset = _rf.max_cons_pos();
  cout << "Max cons pos: ";
  if( pset.empty() ) {
    cout << 0 << endl;
    return;
  }

  cout << (int)pset.size();
  _begin_end(pset);
}


void Report::max_cons_neg( void ) const
{
  if( _rf.num() == 0 )
    return; // avoid exception during report

  PositionSet pset = _rf.max_cons_neg();
  cout << "Max cons neg: ";
  if( pset.empty() ) {
    cout << 0 << endl;
    return;
  }
  
  cout << (int)pset.size();
  _begin_end(pset);
}


void Report::max_dd( void ) const
{
  if( _rf.num() == 0 )
    return; // avoid exception during report

  PositionSet pset = _rf.dd();
  cout << "Max drawdown: ";
  if( pset.empty() ) {
    cout << 0 << endl;
    return;
  }

  ReturnFactors ddrf(pset);
  cout << ddrf.roi()*100 << '%';
  _begin_end(pset);
}


void Report::_begin_end( const Position& pos ) const
{
  // Print begin/end transactions
  cout << " [" << pos.first_exec()->dt();
  
  if( pos.closed() )
    cout << '/' << pos.last_exec()->dt();
  else
    cout << " (Open)";
    
  cout << ']' << std::endl;
}


void Report::_begin_end( const PositionSet& pset ) const
{
  PositionPtr pFirstPos = *(pset.get<last_exec_key>().begin());
  PositionPtr pLastPos = *(pset.get<last_exec_key>().rbegin());

  cout << " [" << pFirstPos->first_exec()->dt() << '/';
  
  if( pLastPos->closed() )
    cout << pLastPos->last_exec()->dt();
  else
    cout << " (Open)";
    
  cout << ']' << endl;
}
