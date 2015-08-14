#ifndef STOCKZH_H
#define STOCKZH_H
#include <QString>

class StockZH
{
public:
    StockZH();

    QString getSymbol();
    QString toString();


private:
    QString symbol;
    QString market;
    QString name;
    float net_value;            //当前值
    float daily_gain;           //日收益 百分比
    float monthly_gain;         //月收益 百分比
    float total_gain;           //总收益 百分比
    float annualized_gain;      //年收益
    bool hasexist;              //":"true"
};

#endif // STOCKZH_H
