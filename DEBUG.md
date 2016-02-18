- lib/Price.cpp : error in switch statement 

```cpp
Price Price::last( const std::string& symbol, Series::EODDB::PriceType pt ) throw(PriceException)
{
    if( Series::EODDB::instance().get(symbol).empty() )
        throw PriceException("Empty series database");
    
    Series::DayPrice item = Series::EODDB::instance().get(symbol).last();
    
    double priceval = 0;
    switch ( pt ) {
            
        case EODDB::OPEN:
            priceval = item.open;
            break;  // <-- this line was omitted causing PriceException when last() is called
            
        case EODDB::CLOSE:
            priceval = item.close;
            break;  // <-- this line was omitted causing PriceException when last() is called
            
        case EODDB::ADJCLOSE:
            priceval = item.adjclose;
            break;  // <-- this line was omitted causing PriceException when last() is called
            
        default:
            throw PriceException("Invalid price type");
    }
    
    return Price(priceval);
}
```

- next 
