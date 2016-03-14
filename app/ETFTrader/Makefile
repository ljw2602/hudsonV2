#
# Copyright (C) 2007, Alberto Giannetti
#
# This file is part of Hudson.
#
# Hudson is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# Hudson is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with Hudson.  If not, see <http://www.gnu.org/licenses/>.
#

CXX=g++

BIN=etf

#LIBS=-lhudson -lboost_date_time -lboost_program_options -lta_common -lta_func -lgsl -lgslcblas
LIBS=-lhudson -lboost_date_time -lboost_program_options -lta_lib -lgsl -lgslcblas

LIBPATH= \
	-L../../lib \
	-L/usr/local/lib

RPATH= \
	-R../../lib \
	-R/usr/local/lib

INCLUDES= \
	-I. \
	-I../../lib \
	-I/usr/local/include \
	-I/usr/local/include/ta-lib

CFLAGS=-g
CPPFLASG=
LDFLAGS=

all: $(BIN)
bin: $(BIN)

%.o: %.cpp
	$(CXX) -c $(INCLUDES) $(DEFS) $(CPPFLAGS) $(CFLAGS) $< -o $@

etf: $(LIB) etf.o ETFTrader.o
	$(CXX) -o $@ $(LDFLAGS) etf.o ETFTrader.o $(LIBPATH) $(LIBS)

clean:
	\rm -f *.o *.d core

clobber:
	\rm -f *.o *.d core $(BIN)
