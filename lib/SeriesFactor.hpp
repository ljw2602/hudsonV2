/*
* Copyright (C) 2007,2008 A. Giannetti
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

#ifndef _SERIESFACTOR_HPP_
#define _SERIESFACTOR_HPP_

// Boost
#include <boost/date_time/gregorian/gregorian.hpp>

/*!
* Single daily factor. SeriesFactor and SeriesFactorSet are used in PositionFactors to calculate single-position
* unrealized return statistics.
* Includes the opening/close date and time used to calculate this factor value.
* \see PositionFactors.
* \see SeriesFactorSet.
*/
class SeriesFactor
{
public:
  /*! Initialize series factor.
  * \param from Factor date/time begin.
  * \param to Factor date/time end.
  * \param f Factor value.
  */
  SeriesFactor(const boost::gregorian::date& from, const boost::gregorian::date& to, double f);

  bool operator<(const SeriesFactor& sf) const { return _f < sf._f; }

  //! Factor calculation start date/time.
  const boost::gregorian::date& from_tm(void) const { return _from; }
  //! Factor calculation end date/time.
  const boost::gregorian::date& to_tm(void) const { return _to; }
  //! Match the same start/end time
  const bool match(const SeriesFactor& sf2) const { return _from == sf2.from_tm() && _to == sf2.to_tm(); } 
  
  //! Factor value.
  double factor(void) const { return _f; }

private:
  boost::gregorian::date _from;
  boost::gregorian::date _to;
  double _f;
};

#endif // _SERIESFACTOR_HPP_
