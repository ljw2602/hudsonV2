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

// Hudson
#include "TA.hpp"

#include "VIXTrader.hpp"

using namespace std;
using namespace boost::gregorian;
using namespace Series;


VIXTrader::VIXTrader(const EODSeries& spx_db, const EODSeries& vix_db):
_spx_db(spx_db),
_vix_db(vix_db),
_invested_days(0)
{
}


void VIXTrader::run(void) throw(TraderException)
{
    TA ta;
    _invested_days = days(0);
    
    Series::EODSeries::const_iterator vix_iter(_vix_db.begin());
    advance(vix_iter, 100); // Skip first 100 bars to give enough room too BBANDS calculation
    for( ; vix_iter != _vix_db.end(); ++vix_iter ) {
        
        try {
            
            // Calculate current invested days
            if( ! _miPositions.open().empty() )
                _invested_days = _invested_days + days(1);
            
            // Calculate VIX BBANDS
            TA::BBRes resBBANDS3 = ta.BBANDS(_vix_db.close(vix_iter, 100), 100, 3, 3);
            TA::BBRes resBBANDS1 = ta.BBANDS(_vix_db.close(vix_iter, 100), 100, 1, 1);
            
            // Check buy signal
            if( _miPositions.open().empty() && vix_iter->second.close > resBBANDS3.upper_band[0] ) {
                // Buy tomorrow's close
                Series::EODSeries::const_iterator iter_entry = _spx_db.after(vix_iter->first);
                if( iter_entry == _spx_db.end() ) {
                    cerr << "Can't open position after " << vix_iter->first << endl;
                    continue;
                }
                
                buy(_spx_db.name(), iter_entry->first, Price(iter_entry->second.open),2);
            }
            
            // Check sell signal
            if( ! _miPositions.open().empty() && vix_iter->second.close < resBBANDS1.upper_band[0] ) {
                // Get next bar
                Series::EODSeries::const_iterator iter_exit = _spx_db.after(vix_iter->first);
                if( iter_exit == _spx_db.end() ) {
                    cerr << "Can't close position after " << vix_iter->first << endl;
                    continue;
                }
                
                // Close all open positions at tomorrow's close
                PositionSet ps = _miPositions.open();
                for( PositionSet::const_iterator pos_iter = ps.begin(); pos_iter != ps.end(); ++pos_iter ) {
                    PositionPtr pPos = (*pos_iter);
                    close(pPos->id(), iter_exit->first, Price(iter_exit->second.open));
                } // end of all open positions
            }
            
        } catch( std::exception& e ) {
            
            cerr << e.what() << endl;
            continue;
        }
        
    } // for each bar
}
