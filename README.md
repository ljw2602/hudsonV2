# hudson v.2
- Required packages: boost, ta-lib, and gsl (installed under /usr/local)
- Installation: read archive/INSTALL
- Description of enhancement (details in Wiki):
  0. Backtesting runs in chronological order
    * v\.1 runs asset by asset, so the backtester sometimes travels back in time (weak form of look-ahead violation) --> unrealistic
    * v\.2 runs in chronological order, and throws an error if an incoming order/request violates the order
  1. Dollar account
    * v\.1 does not have dollar account concept, and only considers the return of each position from the enter & exit prices
    * It implicitly assumes to have infinite capital and be able to buy infinite shares of asset --> unrealistic
    * v\.2 introduces dollar account concept, tracking daily cash balance and portfolio value
    * It mimics your brockerage account balance which enables to calculate more interesting statistics such as beta and alpha
  2. Improved statistics
    * 
  2. Position size
  3. Better class design
  4. Debug v.1
