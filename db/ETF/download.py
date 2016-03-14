import pandas as pd
from pandas.io.data import DataReader
from datetime import datetime

etfs = ['^GSPC','XLY','XLP','XLE','XLF','XLV','XLI','XLB','XLK','XLU']
adjclose = pd.DataFrame()
for ticker in etfs:
    data = DataReader(ticker, 'yahoo', datetime(1900,1,1), datetime.today())
    data.to_csv(ticker+'.csv')
    adj = data['Adj Close']; adj.name = ticker;
    adjclose = pd.concat([adjclose, adj], axis=1)
adjclose.to_csv('AdjClose.csv')
