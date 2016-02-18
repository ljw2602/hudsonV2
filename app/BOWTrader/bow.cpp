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

// Posix
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
#include <PositionFactorsSet.hpp>
#include <PositionsReport.hpp>
#include <BnHTrader.hpp>
#include <EOMReport.hpp>

#include "BOWTrader.hpp"

using namespace std;
using namespace boost::gregorian;

namespace po = boost::program_options;


int main(int argc, char* argv[])
{
  string begin_date, end_date;
  string dbfile, symbol;
  unsigned entry_offset, exit_offset;
  char entry_oc, exit_oc;

  try {
    po::options_description desc("Allowed options");
    desc.add_options()
      ("help", "produce help message")
      ("series_file",  po::value<string>(&dbfile), "series database")
      ("symbol",       po::value<string>(&symbol), "symbol")
      ("begin_date",   po::value<string>(&begin_date), "start of trading period")
      ("end_date",     po::value<string>(&end_date), "end of trading period")
      ("entry_offset", po::value<unsigned>(&entry_offset)->default_value(0), "offset entry from BOW (+days)")
      ("exit_offset",  po::value<unsigned>(&exit_offset)->default_value(1), "offset exit from BOW (+days)")
      ("entry_oc",     po::value<char>(&entry_oc)->default_value('c'), "entry open/close")
      ("exit_oc",      po::value<char>(&exit_oc)->default_value('c'), "exit open/close")
      ;

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if( vm.count("help") ) {
      cout << desc << endl;
      exit(0);
    }

    if( vm["series_file"].empty() ||
	vm["symbol"].empty() ||
	vm["begin_date"].empty() ||
	vm["end_date"].empty() ) {
      cout << desc << endl;
      exit(EXIT_FAILURE);
    }

    if( entry_offset > exit_offset ) {
      cerr << "entry_offset > exit_offset" << endl;
      exit(EXIT_FAILURE);
    }

    if( entry_offset > 6 || exit_offset > 6 ) {
      cerr << "entry/exit offset too large" << endl;
      exit(EXIT_FAILURE);
    }

    cout << "Series file: " << dbfile << endl;

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

    cout << "Loading " << dbfile << " from " << to_simple_string(load_begin) << " to " << to_simple_string(load_end) << "..." << endl;
    Series::EODDB::instance().load(symbol, dbfile, Series::EODDB::YAHOO, load_begin, load_end);

    const Series::EODSeries& db = Series::EODDB::instance().get(symbol);

    cout << "Records: " << db.size() << endl;
    cout << "Period: " << db.period() << endl;
    cout << "Total days: " << db.duration().days() << endl;

    /*
     * Initialize and run strategy
     */
    BOWTrader trader(symbol, db);
    trader.run(entry_offset, entry_oc, exit_offset, exit_oc);

    /*
     * Print open/closed positions
     */
    Report::header("Closed trades");
    trader.positions().closed().print();

    Report::header("Open trades");
    trader.positions().open().print();

    /*
     * Print simulation reports
     */
    Report::header("Trade results");
    ReturnFactors rf(trader.positions());
    Report rp(rf);
    rp.print();

    /*
     * Positions excursion
     */
    Report::header("Positions excursion");
    PositionFactorsSet pf(trader.positions().closed());
    PositionsReport pr(pf);
    pr.print();

    // BnH
    Report::header("BnH");
    BnHTrader bnh(db);
    bnh.run();
    EOMReturnFactors bnh_rf(bnh.positions(), load_begin, load_end);
    EOMReport bnh_rp(bnh_rf);
    bnh_rp.print();

  } catch( Series::DriverException& e ) {
    cerr << "Driver error: " << e.what() << endl;
    exit(EXIT_FAILURE);

  } catch( out_of_range& e ) {
    cerr << "Can't get begin/end dates: " << e.what() << endl;
    exit(EXIT_FAILURE);

  } catch( exception& e ) {
    cerr << "Error: " << e.what() << endl;
    exit(EXIT_FAILURE);
  }

  return 0;
}
