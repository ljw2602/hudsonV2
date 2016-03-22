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
#include <EODBalanceSet.hpp>

#include "Asset.hpp"
#include "ETFTrader.hpp"

using namespace std;
using namespace boost::gregorian;
using namespace Series;

namespace po = boost::program_options;

/*
 Symbol Sector                  Market Cap (as of 3/15/16)
 XLF	Financials              $16.61BN
 XLE	Energy                  $12.70BN
 XLK	Technology              $12.56BN
 XLV	Health Care             $11.89BN
 XLY	Consumer Discretionary  $10.49BN
 XLP	Consumer Staples        $10.09BN
 XLU	Utilities               $8.20BN
 XLI	Industrials             $6.57BN
 XLB	Materials               $2.17BN
*/


int main(int argc, char* argv[])
{
    string begin_date, end_date;
    string equity_file, treasury_file;  // benchmark
    string xly_file, xlp_file, xle_file, xlf_file, xlv_file, xli_file, xlb_file, xlk_file, xlu_file;
    
    try {
        
        /*
         * Extract simulation options
         */
        po::options_description desc("Allowed options");
        desc.add_options()
        ("help", "produce help message")
        ("equity_file",     po::value<string>(&equity_file),    "Equity benchmark series database")
        ("treasury_file",   po::value<string>(&treasury_file),  "Treasury benchmark series database")
        ("xly_file",        po::value<string>(&xly_file),       "XLY series database")
        ("xlp_file",        po::value<string>(&xlp_file),       "XLP series database")
        ("xle_file",        po::value<string>(&xle_file),       "XLE series database")
        ("xlf_file",        po::value<string>(&xlf_file),       "XLF series database")
        ("xlv_file",        po::value<string>(&xlv_file),       "XLV series database")
        ("xli_file",        po::value<string>(&xli_file),       "XLI series database")
        ("xlb_file",        po::value<string>(&xlb_file),       "XLB series database")
        ("xlk_file",        po::value<string>(&xlk_file),       "XLK series database")
        ("xlu_file",        po::value<string>(&xlu_file),       "XLU series database")
        ("begin_date",      po::value<string>(&begin_date),     "start of trading period (YYYY-MM-DD)")
        ("end_date",        po::value<string>(&end_date),       "end of trading period (YYYY-MM-DD)")
        ;
        
        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);
        
        if( vm.count("help") ) {
            cout << desc << endl;
            exit(0);
        }
        
        if( vm["equity_file"].empty() ||
           vm["treasury_file"].empty() ||
           vm["xly_file"].empty() ||
           vm["xlp_file"].empty() ||
           vm["xle_file"].empty() ||
           vm["xlf_file"].empty() ||
           vm["xlv_file"].empty() ||
           vm["xli_file"].empty() ||
           vm["xlb_file"].empty() ||
           vm["xlk_file"].empty() ||
           vm["xlu_file"].empty() ||
           vm["begin_date"].empty() ||
           vm["end_date"].empty() ) {
            cout << desc << endl;
            exit(1);
        }
        
        cout << "Equity benchmark series file: " << equity_file << endl;
        cout << "Treasury benchmark series file: " << treasury_file << endl;
        
        cout << "XLF (Financials) series file: " << xlf_file << endl;
        cout << "XLE (Energy) series file: " << xle_file << endl;
        cout << "XLK (Technology) series file: " << xlk_file << endl;
        cout << "XLV (Health Care) series file: " << xlv_file << endl;
        cout << "XLY (Consumer Discretionary) series file: " << xly_file << endl;
        cout << "XLP (Consumer Staples) series file: " << xlp_file << endl;
        cout << "XLU (Utilities) series file: " << xlu_file << endl;
        cout << "XLI (Industrials) series file: " << xli_file << endl;
        cout << "XLB (Materials) series file: " << xlb_file << endl;
        
        /*
         * Load series data
         */
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
        
        const Series::EODSeries& equity_db = EquityBenchmark(equity_file, "Equity", load_begin, load_end).getDB();
        const Series::EODSeries& treasury_db = TreasuryBenchmark(treasury_file, "Treasury", load_begin, load_end).getDB();
        
        const Series::EODSeries& xlf_db = ETF(xlf_file, "XLF", load_begin, load_end).getDB();
        const Series::EODSeries& xle_db = ETF(xle_file, "XLE", load_begin, load_end).getDB();
        const Series::EODSeries& xlk_db = ETF(xlk_file, "XLK", load_begin, load_end).getDB();
        const Series::EODSeries& xlv_db = ETF(xlv_file, "XLV", load_begin, load_end).getDB();
        const Series::EODSeries& xly_db = ETF(xly_file, "XLY", load_begin, load_end).getDB();
        const Series::EODSeries& xlp_db = ETF(xlp_file, "XLP", load_begin, load_end).getDB();
        const Series::EODSeries& xlu_db = ETF(xlu_file, "XLU", load_begin, load_end).getDB();
        const Series::EODSeries& xli_db = ETF(xli_file, "XLI", load_begin, load_end).getDB();
        const Series::EODSeries& xlb_db = ETF(xlb_file, "XLB", load_begin, load_end).getDB();
        
        /*
         * Initialize and run strategy
         */ 
        string initial_capital("1000");
        ETFTrader trader(equity_db, treasury_db, xly_db, xlp_db, xle_db, xlf_db, xlv_db, xli_db, xlb_db, xlk_db, xlu_db);

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
//
//        /*
//         * Positions excursion
//         */
//        Report::header("Positions excursion");
//        
//        PositionFactorsSet pf(all_positions);
//        PositionsReport pr(pf);
//        pr.print();
        
        /*
         * Bankroll
         */
        trader.summary();
        Report::header("Bankroll");
//        EODBalanceSet::instance().print();
        EODBalanceSet::instance().export_to_csv();

    } catch( std::exception& ex ) {
        
        cerr << ex.what() << endl;
        exit(EXIT_FAILURE);
    }
    
    return 0;
}
