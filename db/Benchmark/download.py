import pandas as pd
import pandas_datareader.data as web
from datetime import datetime

# 1. Treasury yield curve from FRED

maturity = ['DGS1MO','DGS3MO','DGS6MO','DGS1','DGS2','DGS3','DGS5','DGS7','DGS10','DGS20','DGS30']
res = web.DataReader(maturity, 'fred', datetime(1900,1,2), datetime.today())
res.dropna(how="all",axis=0,inplace=True)
res.index.name = "Date"
res.to_csv('treasury_benchmark.csv')

# 2. Equity from Yahoo

data = web.DataReader('^GSPC', 'yahoo', datetime(1900,1,2), datetime.today())

res = pd.DataFrame()
res['Return'] = data['Close'].pct_change(1) * 100
res.dropna(how="all",axis=0,inplace=True)
res.index.name = "Date"
res.to_csv('equity_benchmark.csv')
