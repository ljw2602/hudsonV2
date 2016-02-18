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

#include "AATrader.hpp"

using namespace std;
using namespace boost::gregorian;
using namespace Series;


AATrader::AATrader(const EODSeries& spx_db, const EODSeries& tnx_db, const EODSeries& djc_db, const EODSeries& eafe_db, const EODSeries& reit_db):
  _spx_db(spx_db),
  _tnx_db(tnx_db),
  _djc_db(djc_db),
  _eafe_db(eafe_db),
  _reit_db(reit_db)
{
}


void AATrader::run(void) throw(TraderException)
{
  TA ta;

  const EOMSeries spx_monthly_db = _spx_db.monthly();
  const EOMSeries tnx_monthly_db = _tnx_db.monthly();
  const EOMSeries djc_monthly_db = _djc_db.monthly();
  const EOMSeries eafe_monthly_db = _eafe_db.monthly();
  const EOMSeries reit_monthly_db = _reit_db.monthly();

  EOMSeries::const_iterator spx_iter(spx_monthly_db.begin());
  EOMSeries::const_iterator tnx_iter(tnx_monthly_db.begin());
  EOMSeries::const_iterator djc_iter(djc_monthly_db.begin());
  EOMSeries::const_iterator eafe_iter(eafe_monthly_db.begin());
  EOMSeries::const_iterator reit_iter(reit_monthly_db.begin());

  TA::SMARes spxSMA = ta.SMA(spx_monthly_db.close(), 10);
  TA::SMARes tnxSMA = ta.SMA(tnx_monthly_db.close(), 10);
  TA::SMARes djcSMA = ta.SMA(djc_monthly_db.close(), 10);
  TA::SMARes eafeSMA = ta.SMA(eafe_monthly_db.close(), 10);
  TA::SMARes reitSMA = ta.SMA(reit_monthly_db.close(), 10);

  // Shift series iterator to the beginning of SMA signals in SMA results vector
  advance(spx_iter, spxSMA.begIdx);
  advance(tnx_iter, tnxSMA.begIdx);
  advance(djc_iter, djcSMA.begIdx);
  advance(eafe_iter, eafeSMA.begIdx);
  advance(reit_iter, reitSMA.begIdx);
  
  trade(spx_monthly_db, spx_iter, spxSMA);
  trade(tnx_monthly_db, tnx_iter, tnxSMA);
  trade(djc_monthly_db, djc_iter, djcSMA);
  trade(eafe_monthly_db, eafe_iter, eafeSMA);
  trade(reit_monthly_db, reit_iter, reitSMA);
}


void AATrader::trade(const EOMSeries& db, EOMSeries::const_iterator& iter, const TA::SMARes& sma)
{
  for( int i = 0; iter != db.end(); ++iter, ++i ) {

    try {

      check_buy(db, iter, sma, i);
      check_sell(db, iter, sma, i);

    } catch( std::exception& e ) {

      cerr << e.what() << endl;
      continue;
    }
  }
}


void AATrader::check_buy( const EOMSeries& db, EOMSeries::const_iterator& iter, const TA::SMARes& sma, int i )
{
  if( _miPositions.open(db.name()).empty() && iter->second.close > sma.ma[i] ) {

    // Buy tomorrow's open
    EOMSeries::const_iterator iter_entry = db.after(iter->first);
    if( iter_entry == db.end() ) {
      cerr << "Warning: can't open " << db.name() << " position after " << iter->first << endl;
      return;
    }

    //cout << "Buying on " << iter_entry->first << " at " << iter_entry->second.open << endl;
    buy(db.name(), iter_entry->second.begin, Price(iter_entry->second.open));
  }
}


void AATrader::check_sell( const EOMSeries& db, EOMSeries::const_iterator& iter, const TA::SMARes& sma, int i )
{
  if( ! _miPositions.open(db.name()).empty() && iter->second.close < sma.ma[i] ) {

    EOMSeries::const_iterator iter_exit = db.after(iter->first);
    if( iter_exit == db.end() ) {
      cerr << "Warning: can't close " << db.name() << " position after " << iter->first << endl;
      return;
    }

    // Close all open positions at tomorrow's close
    PositionSet ps = _miPositions.open(db.name());
    for( PositionSet::const_iterator pos_iter = ps.begin(); pos_iter != ps.end(); ++pos_iter ) {
      PositionPtr pPos = (*pos_iter);
      // Sell at tomorrow's open
      //cout << "Selling on " << iter_exit->first << " at " << iter_exit->second.open << endl;
      close(pPos->id(), iter_exit->second.begin, Price(iter_exit->second.open));
    } // end of all open positions
  }
}

