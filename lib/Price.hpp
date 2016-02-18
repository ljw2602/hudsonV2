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

#ifndef _PRICE_HPP_
#define _PRICE_HPP_

#ifdef WIN32
#pragma warning (disable:4290)
#endif

// STDLIB
#include <stdexcept>

// STL
#include <string>

// Boost
#include <boost/date_time/gregorian/gregorian.hpp>

// Hudson
#include "EODDB.hpp"


class PriceException: public std::exception
{
public:
  PriceException(const std::string& msg):
    _Str("PriceException: ")
  {
    _Str += msg;
  }

  virtual ~PriceException(void) throw() { }
  virtual const char *what() const throw() { return _Str.c_str(); }

protected:
  std::string _Str;
};


class Price
{
public:
  //! Return a price for a specific symbol/date.
  /*!
  \param symbol The name of the database series where the lookup series will be performed.
  \param dt The index used to lookup the price in the database series.
  \param pt The price type.
  \return The Price matching the specified date in the symbol series database.
  */
  static Price get(const std::string& symbol, const boost::gregorian::date& dt, Series::EODDB::PriceType pt) throw(PriceException);

  /*!
  \param symbol The name of the database series used to extract the last price.
  \param pt The price type.
  \return The last available Price of type pt in symbol series.
  */
  static Price last(const std::string& symbol, Series::EODDB::PriceType pt) throw(PriceException);
  
public:
  explicit Price(double value);
  Price(const Price& src);

  Price& operator=(const Price& src);

  bool isValid(void) const { return _value > 0; }
  double value(void) const throw(PriceException);

protected:
  double _value;
};

inline double operator-(const Price& first, const Price& second) { return first.value() - second.value(); }
inline double operator+(const Price& first, const Price& second) { return first.value() + second.value(); }
inline double operator*(const Price& first, const Price& second) { return first.value() * second.value(); }
inline double operator/(const Price& first, const Price& second) { return first.value() / second.value(); }

inline double operator-(const Price& first, double second) { return first.value() - second; }
inline double operator+(const Price& first, double second) { return first.value() + second; }
inline double operator*(const Price& first, double second) { return first.value() * second; }
inline double operator/(const Price& first, double second) { return first.value() / second; }

#endif // _PRICE_HPP_
