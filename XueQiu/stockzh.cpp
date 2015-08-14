#include "stockzh.h"

StockZH::StockZH()
{

}

StockZH::StockZH(QString symbol,
        QString name,
        float net_value,            //当前值
        float daily_gain,           //日收益 百分比
        float monthly_gain,         //月收益 百分比
        float total_gain,           //总收益 百分比
        float annualized_gain,      //年收益
        bool hasexist  )
{
     this->symbol = symbol;
    this->name=name;
    this->net_value=net_value;             //当前值
    this->daily_gain=daily_gain;           //日收益 百分比
    this->monthly_gain=monthly_gain;       //月收益 百分比
    this->total_gain=total_gain;           //总收益 百分比
    this->annualized_gain=annualized_gain; //年收益
    this->hasexist=hasexist;
}

QString StockZH::toString()
{
    return this->name;
}

QString StockZH::getSymbol()
{
    return this->symbol;
}

