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
#include "EOMTrader.hpp"
#include "BnHTrader.hpp"
#include "Report.hpp"

using namespace std;
using namespace boost::gregorian;
using namespace Series;

namespace po = boost::program_options;

typedef Series::DaySeries<Series::DayPrice> DB;


int main(int argc, char* argv[])
{
  string begin_date, end_date;
  string dbfile;
  int entry_offset, exit_offset;

  try {
	po::options_description desc("Allowed options");
	desc.add_options()
	  ("help", "produce help message")
	  ("series_file", po::value<string>(&dbfile), "series database")
	  ("begin_date", po::value<string>(&begin_date), "start of trading period (YYYY-MM-DD)")
	  ("end_date", po::value<string>(&end_date), "end of trading period (YYYY-MM-DD)")
	  ("max_entry_offset", po::value<int>(&entry_offset)->default_value(5), "max entry offset before EOM")
	  ("max_exit_offset", po::value<int>(&exit_offset)->default_value(5), "max exit offset after EOM")
	  ;

	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, desc), vm);
	po::notify(vm);

	if( vm.count("help") ) {
	  cout << desc << endl;
	  exit(0);
	}

	if( vm["series_file"].empty() ||
		vm["begin_date"].empty() || vm["end_date"].empty() ) {
	  cout << desc << endl;
	  exit(EXIT_FAILURE);
	}

  } catch( exception& e) {
	cerr << "Error: " << e.what() << endl;
	exit(EXIT_FAILURE);
  }

  YahooDriver yd;
  DB db("myseries", yd);

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

	cout << "Loading " << dbfile << " from " << to_simple_string(load_begin) << " to " << to_simple_string(load_end) << "..." << endl;
	if( db.load(dbfile, load_begin, load_end) <= 0 ) {
	  cerr << "No records found" << endl;
	  exit(EXIT_FAILURE);
	}

  } catch( DriverException& e ) {
	cerr << "Driver error: " << e.what() << endl;
	exit(EXIT_FAILURE);

  } catch( out_of_range& e ) {
	cerr << "Out of range error: " << e.what() << endl;
	exit(EXIT_FAILURE);

  } catch( exception& e ) {
	cerr << "Error: " << e.what() << endl;
	exit(EXIT_FAILURE);
  }

  EOMTrader trader(db);

  cout.precision(4);

  cout << "Entry, Exit, Avg, Std Dev, Pos, Neg, % Pos, % Neg, Avg Pos, Avg Neg, Best, Worst, Cons Pos, Cons Neg, ROI, CAGR, GSDm" << endl;
  for( int i = 1; i <= entry_offset; i++ ) {
	for( int j = 1; j <= exit_offset; j++ ) {
	  trader.run(i, j);

	  ReturnFactors rf(trader.positions().closed().factors(), db.duration().days(), 12);
	  ReturnFactors rf_pos(rf.pos(), db.duration().days(), 12);
	  ReturnFactors rf_neg(rf.neg(), db.duration().days(), 12);

	  double pos_percent = rf_pos.num() / (double)rf.num();
	  double neg_percent = rf_neg.num() / (double)rf.num();

	  cout << i << ',' << j << ',';
	  cout << rf.avg() << ',' << rf.stddev() << ',';
	  cout << rf_pos.num() << ',' << rf_neg.num() << ',' << pos_percent << ',' << neg_percent << ',';
	  cout << rf_pos.avg() << ',' << rf_neg.avg() << ',';
	  cout << rf.best() << ',' << rf.worst() << ',' << rf.max_cons_pos() << ',' << rf.max_cons_neg() << ',';
	  cout << rf.roi() << ',' << rf.cagr() << ',' << rf.gsd() << endl;
	}
  }

  return 0;
}
