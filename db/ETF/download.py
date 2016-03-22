import pandas as pd
import pandas_datareader.data as web
from datetime import datetime

etfs = ['^GSPC','XLY','XLP','XLE','XLF','XLV','XLI','XLB','XLK','XLU']
#adjclose = pd.DataFrame()
#for ticker in etfs:
#    data = web.DataReader(ticker, 'yahoo', datetime(1900,1,1), datetime.today())
#    data.to_csv(ticker+'.csv')
#    adj = data['Adj Close']; adj.name = ticker;
#    adjclose = pd.concat([adjclose, adj], axis=1)
#adjclose.to_csv('AdjClose.csv')

for ticker in etfs:
    data = web.DataReader(ticker, 'yahoo-actions', datetime(1900,1,1), datetime.today())
    data.to_csv(ticker+'_actions.csv')
