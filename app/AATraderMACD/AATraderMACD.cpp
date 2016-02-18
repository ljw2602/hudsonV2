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
 
// STL
#include <cmath>

// Hudson
#include "AATraderMACD.hpp"

using namespace std;
using namespace boost::gregorian;
using namespace Series;


AATraderMACD::AATraderMACD(const EODSeries& spx_db, const EODSeries& tnx_db, const EODSeries& djc_db, const EODSeries& eafe_db, const EODSeries& reit_db):
  _spx_db(spx_db),
  _tnx_db(tnx_db),
  _djc_db(djc_db),
  _eafe_db(eafe_db),
  _reit_db(reit_db)
{
}


void AATraderMACD::run(void) throw(TraderException)
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

  TA::MACDRes spxMACD = ta.MACD(spx_monthly_db.close(), 12, 26, 9);
  TA::MACDRes tnxMACD = ta.MACD(tnx_monthly_db.close(), 12, 26, 9);
  TA::MACDRes djcMACD = ta.MACD(djc_monthly_db.close(), 12, 26, 9);
  TA::MACDRes eafeMACD = ta.MACD(eafe_monthly_db.close(), 12, 26, 9);
  TA::MACDRes reitMACD = ta.MACD(reit_monthly_db.close(), 12, 26, 9);

  // Shift series iterator to the beginning of SMA signals in SMA results vector
  advance(spx_iter, spxMACD.begIdx);
  advance(tnx_iter, tnxMACD.begIdx);
  advance(djc_iter, djcMACD.begIdx);
  advance(eafe_iter, eafeMACD.begIdx);
  advance(reit_iter, reitMACD.begIdx);
  
  trade(spx_monthly_db, spx_iter, spxMACD);
  trade(tnx_monthly_db, tnx_iter, tnxMACD);
  trade(djc_monthly_db, djc_iter, djcMACD);
  trade(eafe_monthly_db, eafe_iter, eafeMACD);
  trade(reit_monthly_db, reit_iter, reitMACD);
}


void AATraderMACD::trade(const EOMSeries& db, EOMSeries::const_iterator& iter, const TA::MACDRes& macd)
{
  for( int i = 0; iter != db.end(); ++iter, ++i ) {

    try {

      check_buy(db, iter, macd, i);
      check_sell(db, iter, macd, i);

    } catch( std::exception& e ) {

      cerr << e.what() << endl;
      continue;
    }
  }
}


void AATraderMACD::check_buy( const EOMSeries& db, EOMSeries::const_iterator& iter, const TA::MACDRes& macd, int i )
{
  // Buy on MACD cross and MACD Hist > 0.5%
  if( _miPositions.open(db.name()).empty() && macd.macd[i] > macd.macd_signal[i] && (::abs(macd.macd_hist[i]) / iter->second.close ) > 0.005 ) {
  
    //cout << db.name() << " MACD " << macd.macd[i] << " MACD Signal " << macd.macd_signal[i] << " MACD Hist " << macd.macd_hist[i] << endl;
    
    // Buy tomorrow's open
    EOMSeries::const_iterator iter_entry = db.after(iter->first);
    if( iter_entry == db.end() ) {
      cerr << "Warning: can't open " << db.name() << " position after " << iter->first << endl;
      return;
    }

    cout << "Buying on " << iter_entry->first << " at " << iter_entry->second.open << endl;
    buy(db.name(), iter_entry->first, Price(iter_entry->second.open));
  }
}


void AATraderMACD::check_sell( const EOMSeries& db, EOMSeries::const_iterator& iter, const TA::MACDRes& macd, int i )
{
  if( ! _miPositions.open(db.name()).empty() && macd.macd[i] < macd.macd_signal[i] && (::abs(macd.macd_hist[i]) / iter->second.close ) > 0.005 ) {
  
    //cout << db.name() << "MACD " << macd.macd[i] << " MACD Signal " << macd.macd_signal[i] << " MACD Hist " << macd.macd_hist[i] << endl;
    
    EOMSeries::const_iterator iter_exit = db.after(iter->first);
    if( iter_exit == db.end() ) {
      cerr << "Warning: can't close " << db.name() << " position after " << iter->first << endl;
      return;
    }

    // Close all open positions at tomorrow's open
    PositionSet ps = _miPositions.open(db.name());
    for( PositionSet::const_iterator pos_iter = ps.begin(); pos_iter != ps.end(); ++pos_iter ) {
      PositionPtr pPos = (*pos_iter);
      // Sell at tomorrow's open
      //cout << "Selling on " << iter_exit->first << " at " << iter_exit->second.open << endl;
      close(pPos->id(), iter_exit->first, Price(iter_exit->second.open));
    } // end of all open positions
  }
}
