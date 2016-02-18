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

#ifndef _DMYCLOSEDRIVER_HPP_
#define _DMYCLOSEDRIVER_HPP_

#ifdef WIN32
#pragma warning (disable:4290)
#endif

// STL
#include <fstream>
#include <string>

// Boost
#include <boost/date_time/gregorian/gregorian.hpp>

// Series
#include "FileDriver.hpp"
#include "DayPrice.hpp"

namespace Series
{
  /*!
    DMYCloseDriver is a simple file parser that reads lines in the following format:
    DD-MMM-YYYY, Close.
    All fields should be comma separated. The first line in the file is a descriptive
    of the columns and will be skipped by the parser.
    
    Some examples of day price entries are:
    31-Dec-1971,101.22
    26-Feb-1999,89.58
  */
  class DMYCloseDriver: public FileDriver
  {
  public:
	  DMYCloseDriver(void);
	  ~DMYCloseDriver(void);

    //! Open a file for parsing.
    /*!
      \param filename The path to the file containing the data series.
    */
	  virtual bool open(const std::string& filename);
	  //! Close the file.
	  virtual void close(void);
	  /*!
	    Parse one line in the file. Store contents in dp.
	    Throws DriverException if parsed values are invalid or have a wrong format.
	    \param dp EOD data storage.
	  */
	  virtual bool next(DayPrice& dp) throw(DriverException);
	  virtual bool eof(void);

  private:
	  enum FIELDS_POS {
	    DATE = 0,
      CLOSE
	  };

  private:
	  std::ifstream _infile;
	  std::string _line;
	  unsigned _linenum;
  };

} // namespace Series

#endif // _DMYCLOSEDRIVER_HPP_
