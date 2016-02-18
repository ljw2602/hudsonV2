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
#include <TA.hpp>

#include "EOMTrader.hpp"

using namespace std;
using namespace boost::gregorian;
using namespace Series;


EOMTrader::EOMTrader(const string& symbol, const DB& db):
  _symbol(symbol),
  _db(db),
  _invested_days(0)
{
}


void EOMTrader::run(unsigned entry_days, unsigned exit_days) throw(TraderException)
{
  _invested_days = days(0);

  date my_first_entry;
  date my_last_exit;

  for( month_iterator miter(_db.begin()->first); miter < _db.rbegin()->first; ++miter ) {

    if( (*miter).month() == _db.rbegin()->first.month() && (*miter).year() == _db.rbegin()->first.year() )
      // We're on the last month of database record, we don't have enough records to find next month exit date
      continue;

    DB::const_iterator last_tradeday_iter = _db.last_in_month((*miter).year(), (*miter).month());
    if( last_tradeday_iter == _db.end() ) {
      cerr << "Can't find last trade day at date " << (*miter) << endl;
      continue;
    }

    DB::const_iterator entry_iter = _db.before(last_tradeday_iter->first, entry_days - 1);
    if( entry_iter == _db.end() ) {
      cerr << "Can't find actual entry date on " << last_tradeday_iter->first << endl;
      continue;
    }

    DB::const_iterator exit_iter = _db.after(last_tradeday_iter->first, exit_days);
    if( exit_iter == _db.end() ) {
      cerr << "Can't find actual exit date on " << last_tradeday_iter->first << endl;
      continue;
    }

    try {

      Position::ID id = buy(_symbol, entry_iter->first, Price(entry_iter->second.close));
      close(id, exit_iter->first, Price(exit_iter->second.close));

    } catch( std::exception& e ) {
      cerr << e.what() << endl;
      continue;
    }

    _invested_days = _invested_days + date_period(entry_iter->first, exit_iter->first).length();

    if( my_first_entry.is_not_a_date() ) my_first_entry = entry_iter->first;
    my_last_exit = exit_iter->first;

  }	// for each month in price database

  _first_entry = my_first_entry;
  _last_exit = my_last_exit;
}
