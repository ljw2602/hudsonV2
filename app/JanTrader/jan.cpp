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

// STDLIB
#include <ctime>
#include <cstdlib>
#include <cmath>

// IO
#include <iostream>
#include <fstream>
#include <string>

// Boost
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/program_options.hpp>

// Hudson
#include <EODDB.hpp>
#include <EOMReturnFactors.hpp>
#include <EOMReport.hpp>
#include <PositionFactorsSet.hpp>
#include <PositionsReport.hpp>

// App
#include "JanTrader.hpp"

using namespace std;
using namespace boost::gregorian;

namespace po = boost::program_options;


int main(int argc, char* argv[])
{
  string begin_date, end_date;
  string long_dbfile, hedge_dbfile;
  string long_symbol, hedge_symbol;
  int entry_offset = 0;
  int exit_offset = 0;

  try {
    po::options_description desc("Allowed options");
    desc.add_options()
      ("help", "produce help message")
      ("long_dbfile", po::value<string>(&long_dbfile), "long price database")
      ("hedge_dbfile", po::value<string>(&hedge_dbfile), "hedge price database")
      ("long_symbol", po::value<string>(&long_symbol), "long ticker")
      ("hedge_symbol", po::value<string>(&hedge_symbol), "hedge ticker")
      ("begin_date", po::value<string>(&begin_date), "start of trading period (YYYY-MM-DD)")
      ("end_date", po::value<string>(&end_date), "end of trading period (YYYY-MM-DD)")
      ("entry_offset", po::value<int>(&entry_offset), "offset from canonical entry date")
      ("exit_offset", po::value<int>(&exit_offset), "offset from canonical exit date")
      ;

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if( vm.count("help") ) {
      cout << desc << endl;
      exit(EXIT_FAILURE);
    }

    if( vm["long_dbfile"].empty() || vm["hedge_dbfile"].empty() ||
      	vm["long_symbol"].empty() || vm["hedge_symbol"].empty() ||
	      vm["begin_date"].empty()  || vm["end_date"].empty() ) {
      cout << desc << endl;
      exit(EXIT_FAILURE);
    }

    cout << "Long db file: " << long_dbfile << endl;
    cout << "Hedge db file: " << hedge_dbfile << endl;
    cout << "Entry days offset: " << entry_offset << endl;
    cout << "Exit days offset: " << exit_offset << endl;

  } catch( exception& e) {
    cerr << "Error: " << e.what() << endl;
    exit(EXIT_FAILURE);
  }

  try {

    date load_begin(from_simple_string(begin_date));
    if( load_begin.is_not_a_date() ) {
      cerr << "Invalid begin date " << begin_date << endl;
      exit(EXIT_FAILURE);
    }

    date load_end(from_simple_string(end_date));
    if( load_end.is_not_a_date() ) {
      cerr << "Invalid end date " << end_date << endl;
      exit(EXIT_FAILURE);
    }

    cout << "Loading hedge db file " << hedge_dbfile << " from " << load_begin << " to " << load_end << "..." << endl;
    Series::EODDB::instance().load(hedge_symbol, hedge_dbfile, Series::EODDB::YAHOO, load_begin, load_end);

    cout << "Loading long db file " << long_dbfile << " from " << load_begin << " to " << load_end << "..." << endl;
    Series::EODDB::instance().load(long_symbol, long_dbfile, Series::EODDB::YAHOO, load_begin, load_end);

    const Series::EODSeries& long_db = Series::EODDB::instance().get(long_symbol);
    const Series::EODSeries& hedge_db = Series::EODDB::instance().get(hedge_symbol);

    cout << "Records: " << long_db.size() << endl;
    cout << "Period: " << long_db.period() << endl;
    cout << "Total days: " << long_db.days() << endl;

    cout << "Hedge Records: " << hedge_db.size() << endl;
    cout << "Hedge Period: " << hedge_db.period() << endl;
    cout << "Hedge Total days: " << hedge_db.days() << endl;
    
    JanTrader trader(long_symbol, hedge_symbol);
    trader.run(entry_offset, exit_offset); // canonical entry/exit dates (12/20 - 1/9)

    /*
    * Print closed positions
    */
    Report::header("Closed trades");
    trader.positions().stratPos().closed().print();

    /*
    * Print simulation reports
    */
    Report::header("Trade results");
    ReturnFactors rf(trader.positions().stratPos());
    Report rp(rf);
    rp.print();

    /*
    * Position stats
    */
    Report::header("Positions stats");
    PositionFactorsSet pf(trader.positions().stratPos().closed());
    PositionsReport pr(pf);
    pr.print();
    
  } catch ( std::exception& ex ) {
  
    cerr << ex.what() << endl;
    exit(EXIT_FAILURE);
  }
  
  return 0;
}
