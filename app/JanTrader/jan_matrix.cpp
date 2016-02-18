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

// Series
#include "YahooDriver.hpp"
#include "DaySeries.hpp"
#include "DayPrice.hpp"
#include "ReturnFactors.hpp"
#include "Report.hpp"
#include "JanTrader.hpp"

using namespace std;
using namespace boost::gregorian;
using namespace Series;

namespace po = boost::program_options;


typedef Series::DaySeries<Series::DayPrice> DB;


int main(int argc, char* argv[])
{
  string begin_date, end_date;
  string long_dbfile, hedge_dbfile;
  string long_symbol, hedge_symbol;
  int entry_low_offset, entry_high_offset;
  int exit_low_offset, exit_high_offset;

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
	  ("entry_low_offset", po::value<int>(&entry_low_offset)->default_value(-5), "entry low offset")
	  ("entry_high_offset", po::value<int>(&entry_high_offset)->default_value(5), "entry high offset")
	  ("exit_low_offset", po::value<int>(&exit_low_offset)->default_value(-5), "exit low offset")
	  ("exit_high_offset", po::value<int>(&exit_high_offset)->default_value(5), "exit high offset")
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

  } catch( exception& e) {
	cerr << "Error: " << e.what() << endl;
	exit(EXIT_FAILURE);
  }

  YahooDriver yd;
  DB long_db(long_symbol, yd);
  DB hedge_db(hedge_symbol, yd);

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
	if( hedge_db.load(hedge_dbfile, load_begin, load_end) <= 0) {
	  cerr << "No records found in " << hedge_dbfile << endl;
	  exit(EXIT_FAILURE);
	}

	cout << "Loading long db file " << long_dbfile << " from " << load_begin << " to " << load_end << "..." << endl;
	if( long_db.load(long_dbfile, load_begin, load_end) <= 0 ) {
	  cerr << "No records found in" << long_dbfile << endl;
	  exit(EXIT_FAILURE);
	}

  } catch( std::out_of_range& e ) {
	cerr << "Can't get begin/end dates: " << e.what() << endl;
	exit(EXIT_FAILURE);

  } catch( Series::DriverException& e ) {
	cerr << "Driver error: " << e.what() << endl;
	exit(EXIT_FAILURE);
  }

  JanTrader trader(long_db, hedge_db);

  cout.precision(4);

  cout << "Entry, Exit, Avg, Std Dev, Pos, Neg, % Pos, % Neg, Avg Pos, Avg Neg, Best, Worst, Cons Pos, Cons Neg" << endl;
  for( int i = entry_low_offset; i <= entry_high_offset; i++ ) {
	for( int j = exit_low_offset; j <= exit_high_offset; j++ ) {

	  trader.run(i, j);

	  ReturnFactors rf(trader.factors(), long_db.duration().days(), 12);
	  ReturnFactors rf_pos(rf.pos(), long_db.duration().days(), 12);
	  ReturnFactors rf_neg(rf.neg(), long_db.duration().days(), 12);

	  double pos_percent = rf_pos.num() / (double)rf.num();
	  double neg_percent = rf_neg.num() / (double)rf.num();

	  cout << i << ',' << j << ',';
	  cout << rf.avg() << ',' << rf.stddev() << ',';
	  cout << rf_pos.num() << ',' << rf_neg.num() << ',' << pos_percent << ',' << neg_percent << ',';
	  cout << rf_pos.avg() << ',' << rf_neg.avg() << ',';
	  cout << rf.best() << ',' << rf.worst() << ',' << rf.max_cons_pos() << ',' << rf.max_cons_neg() << ',';
	  cout << endl;
	}
  }

  return 0;
}
