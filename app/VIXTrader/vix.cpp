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
#include <string>

// Boost
#include <boost/program_options.hpp>

// Hudson
#include <EODDB.hpp>
#include <ReturnFactors.hpp>
#include <PositionFactors.hpp>
#include <PositionFactorsSet.hpp>
#include <BnHTrader.hpp>
#include <Report.hpp>
#include <PositionsReport.hpp>
#include <Bankroll.hpp>

#include "VIXTrader.hpp"

using namespace std;
using namespace boost::gregorian;
using namespace Series;

namespace po = boost::program_options;


int main(int argc, char* argv[])
{
    string begin_date, end_date;
    string spx_file, vix_file;
    
    try {
        
        /*
         * Extract simulation options
         */
        po::options_description desc("Allowed options");
        desc.add_options()
        ("help", "produce help message")
        ("spx_file",        po::value<string>(&spx_file),      "SPX series database")
        ("vix_file",        po::value<string>(&vix_file),      "VIX series database")
        ("begin_date",      po::value<string>(&begin_date),    "start of trading period (YYYY-MM-DD)")
        ("end_date",        po::value<string>(&end_date),      "end of trading period (YYYY-MM-DD)")
        ;
        
        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);
        
        if( vm.count("help") ) {
            cout << desc << endl;
            exit(0);
        }
        
        if( vm["spx_file"].empty() ||
           vm["vix_file"].empty() ||
           vm["begin_date"].empty() ||
           vm["end_date"].empty() ) {
            cout << desc << endl;
            exit(1);
        }
        
        cout << "SPX series file: " << spx_file << endl;
        cout << "VIX series file: " << vix_file << endl;
        
        /*
         * Load series data
         */
        const string spx_symbol = "SPX";
        const string vix_symbol = "VIX";
        
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
        
        cout << "Loading " << spx_file << " from " << to_simple_string(load_begin) << " to " << to_simple_string(load_end) << "..." << endl;
        Series::EODDB::instance().load(spx_symbol, spx_file, Series::EODDB::YAHOO, load_begin, load_end);
        
        cout << "Loading " << vix_file << " from " << to_simple_string(load_begin) << " to " << to_simple_string(load_end) << "..." << endl;
        Series::EODDB::instance().load(vix_symbol, vix_file, Series::EODDB::YAHOO, load_begin, load_end);
        
        const Series::EODSeries& spx_db = Series::EODDB::instance().get(spx_symbol);
        const Series::EODSeries& vix_db = Series::EODDB::instance().get(vix_symbol);
        
        cout << "SPX Records: " << spx_db.size() << endl;
        cout << "SPX Period: " << spx_db.period() << endl;
        cout << "SPX Total days: " << spx_db.duration().days() << endl;
        
        cout << "VIX Records: " << vix_db.size() << endl;
        cout << "VIX Period: " << vix_db.period() << endl;
        cout << "VIX Total days: " << vix_db.duration().days() << endl;
        
        /*
         * Initialize and run strategy
         */
        string initial_capital("10000");        
        VIXTrader trader(spx_db, vix_db);
        trader.run(initial_capital);
        PositionSet all_positions(trader.positions());
        
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
        ReturnFactors rf(all_positions);
        Report rp(rf);
        rp.print();
        
        /*
         * Positions excursion
         */
        Report::header("Positions excursion");
        
        PositionFactorsSet pf(all_positions);
        PositionsReport pr(pf);
        pr.print();
        
        /*
         * Bankroll
         */
        Report::header("Bankroll");
        Bankroll::instance().print();
        Bankroll::instance().print_capital();
        
    } catch( std::exception& ex ) {
        
        cerr << ex.what() << endl;
        exit(EXIT_FAILURE);
    }
    
    return 0;
}
