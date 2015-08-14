#ifndef STOCKZH_H
#define STOCKZH_H
#include <QString>

class StockZH
{
public:
    StockZH();
    StockZH(QString symbol,
            QString name,
            float net_value,            //当前值
            float daily_gain,           //日收益 百分比
            float monthly_gain,         //月收益 百分比
            float total_gain,           //总收益 百分比
            float annualized_gain,      //年收益
            bool hasexist  );
    QString getSymbol();
    QString toString();

    /*
    void setSymbol();
    void setMarket();
    void setName();
    void setNet_value();            //当前值
    void setDaily_gain();           //日收益 百分比
    void setMonthly_gain();         //月收益 百分比
    void setTotal_gain();           //总收益 百分比
    void setAnnualized_gain();      //年收益
    void  setHasexist();
    */


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
