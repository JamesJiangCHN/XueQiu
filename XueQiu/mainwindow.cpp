#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    mNetManager = new QNetworkAccessManager(this);
    QObject::connect(mNetManager, SIGNAL(finished(QNetworkReply*)),
                 this, SLOT(finishedSlot(QNetworkReply*)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pb_login_clicked()
{
   login( ui->le_userName->text(), ui->le_pwd->text());
}

void MainWindow::login(QString userName, QString pwd)
{
    sendFlag = FLAG_LOGIN;
    mUrl = QUrl("http://xueqiu.com/user/login");
    // make password to md5
    QByteArray byte_array;
    QString md5;

    byte_array.append(pwd);
    md5 = QCryptographicHash::hash(byte_array, QCryptographicHash::Md5).toHex();
    qDebug() <<md5 << endl;

    QString postString = "areacode=86&password=";
    postString.append(md5);
    postString.append("&remember_me=on&");
    QRegExp tRegExp("^((13[0-9])|(15[0-9])|(17[0-9])|(18[0-9]))\\d{8}$");
    //QRegExpValidator tValidator = new QRegExpValidator(tRegExp);
    if(tRegExp.exactMatch(userName))
    {
        postString.append("telephone=");
    }
    else
    {
        postString.append("username=");
    }

    postString.append(userName);
    byte_array = postString.toUtf8();

    mNetRequest.setUrl(mUrl);
    mNetRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    mNetRequest.setHeader(QNetworkRequest::UserAgentHeader,
                           "Mozilla/5.0 (Windows NT 6.1; WOW64) \
                            AppleWebKit/537.36 (KHTML, like Gecko) \
                            Chrome/41.0.2272.118 UBrowser/5.2.3285.46 Safari/537.36");
    mNetRequest.setHeader(QNetworkRequest::ContentLengthHeader, byte_array.length());
    mNetManager->post(mNetRequest,byte_array);
}

void MainWindow::checkLogin()
{
    foreach (QNetworkCookie tCookie, cookies) {
        if(tCookie.name()== "u")
        {
            uid = QString(tCookie.value());
            break;
        }

    }
    if(uid.isEmpty()){
        qDebug() << "LoginError!";
        QMessageBox::question(this,tr("错误"),tr("登录失败！"));
        return;
    }
    qDebug() << uid;
    getZH(uid);
}



void MainWindow::getZH(QString uid)
{

    sendFlag = FLAG_ZH;
    QNetworkRequest getNetRequest;
    qlonglong nTime = QDateTime::currentDateTimeUtc().currentMSecsSinceEpoch();
    QString nTimeStr =QString::number(nTime);

    //http://xueqiu.com/v4/stock/portfolio/stocks.json?size=1000&tuid=7739010226&pid=-1&category=1&type=1&_=1439457087039
    QString url = "http://xueqiu.com/v4/stock/portfolio/stocks.json?size=1000&tuid=";
    url.append(uid);
    url.append("&pid=-1&category=1&type=1&_=");
    url.append(nTimeStr);
    mUrl = QUrl(url);

    QVariant var;
    var.setValue(cookies);

    getNetRequest.setUrl(mUrl);
    getNetRequest.setHeader(QNetworkRequest::CookieHeader,var);
    getNetRequest.setHeader(QNetworkRequest::UserAgentHeader,
                                "Mozilla/5.0 (Windows NT 6.1; WOW64) \
                                AppleWebKit/537.36 (KHTML, like Gecko) \
                                Chrome/41.0.2272.118 UBrowser/5.2.3285.46 Safari/537.36");
    mNetManager->get(getNetRequest);
}

void MainWindow::getZHDetail(QString zhStr)
{

    sendFlag = FLAG_ZHDETAIL;
    qlonglong nTime = QDateTime::currentDateTimeUtc().currentMSecsSinceEpoch();
    QString nTimeStr =QString::number(nTime);

    //http://xueqiu.com/cubes/quote.json?code=ZH647226%2CZH068107%2C...&return_hasexist=false&_=1439562874467
    QString url = "http://xueqiu.com/cubes/quote.json?code=";
    url.append(zhStr); //.replace(",", "%2C")
    url.append("&return_hasexist=false&_=");
    url.append(nTimeStr);
    mUrl = QUrl(url);
    //qDebug() << url;

    QVariant var;
    var.setValue(cookies);

    QNetworkRequest getNetRequest;
    getNetRequest.setUrl(mUrl);
    getNetRequest.setHeader(QNetworkRequest::CookieHeader,var);
    getNetRequest.setHeader(QNetworkRequest::UserAgentHeader,
                            "Mozilla/5.0 (Windows NT 6.1; WOW64) \
                            AppleWebKit/537.36 (KHTML, like Gecko) \
                            Chrome/41.0.2272.118 UBrowser/5.2.3285.46 Safari/537.36");
    mNetManager->get(getNetRequest);

}

QString MainWindow::processZHJson(QByteArray zhArray)
{
    QString zhStr ="";
    QJsonParseError json_error;
    QJsonDocument parse_doucment = QJsonDocument::fromJson(zhArray, &json_error);

    if(json_error.error == QJsonParseError::NoError)
    {
        if(parse_doucment.isObject())
        {
            QJsonObject obj = parse_doucment.object();
            if(obj.contains("stocks"))
            {
                QVariantMap result = parse_doucment.toVariant().toMap();
                if(result["stocks"].toList().size() > 0)
                {
                    foreach (QVariant stock, result["stocks"].toList()) {
                        QVariantMap stockMap = stock.toMap();
                        zhStr += stockMap["code"].toString()+",";
                    }
                    zhStr.chop(1);
                }
            }
        }
    }
    return zhStr;
}

void MainWindow::processZHDetailJson(QByteArray zhDetailArray)
{
    standardItemModel = new QStandardItemModel(this);
    QJsonParseError json_error;
    QJsonDocument parse_doucment = QJsonDocument::fromJson(zhDetailArray, &json_error);

    if(json_error.error == QJsonParseError::NoError)
    {
        if(parse_doucment.isObject())
        {
            QVariantMap result = parse_doucment.toVariant().toMap();

            QMapIterator<QString, QVariant> i(result);
            int j=0;
            while (i.hasNext()) {
                i.next();
                QVariantMap stockMap = i.value().toMap();
                StockZH mStockZH(stockMap["symbol"].toString(),
                        stockMap["name"].toString(),
                        stockMap["net_value"].toFloat(),
                        stockMap["daily_gain"].toFloat(),
                        stockMap["monthly_gain"].toFloat(),
                        stockMap["total_gain"].toFloat(),
                        stockMap["annualized_gain"].toFloat(),
                        stockMap["hasexist"].toBool());
                QStandardItem *item = new QStandardItem(mStockZH.toString()+"\t 总收益："+QString::number(mStockZH.getTotalGain()));
                QVariant var;
                var.setValue(mStockZH);
                item->setData(var, Qt::UserRole);
                if(j % 2 == 1)
                {
                    QLinearGradient linearGrad(QPointF(0, 0), QPointF(200, 200));
                    linearGrad.setColorAt(0, Qt::yellow);
                    linearGrad.setColorAt(1, Qt::red);
                    QBrush brush(linearGrad);
                    item->setBackground(brush);
                }
                standardItemModel->appendRow(item);
                j++;
            }
           ui->listView->setModel(standardItemModel);
           connect(ui->listView,SIGNAL(clicked(QModelIndex)),this,SLOT(itemClicked(QModelIndex)));

        }
    }
}


// login request callback function
void MainWindow::finishedSlot(QNetworkReply *reply)
{
    QVariant statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    qDebug() << "statusCode is " << statusCode.toString();
    QString contentType = reply->header(QNetworkRequest::ContentTypeHeader).toString();
    QString charset = contentType.mid(contentType.indexOf("charset=") + QString("charset=").length());

    do
    {
        if (statusCode.toInt() == 301 || statusCode.toInt() == 302)
        {
            // Or the target URL if it was a redirect:
            QVariant redirectionTargetUrl = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
            qDebug() << "redirection Url is " << redirectionTargetUrl.toString();
            QUrl url(redirectionTargetUrl.toString());
            mNetManager->get(QNetworkRequest(url));
            break;
        }

        // when reply is no error
        if (reply->error() == QNetworkReply::NoError)
        {
            QByteArray bytes = reply->readAll();  // bytes
            QString result;
            if (charset.compare("utf-8") == 0)
            {
                 result = QString::fromUtf8(bytes);
            }
            if (charset.compare("gbk") == 0)
            {
                QTextCodec *gbk = QTextCodec::codecForName("GB18030");
                result = gbk->toUnicode(bytes);
            }
            else
            {
                result = QString(bytes);
            }

            cookies = mNetManager->cookieJar()->cookiesForUrl(mUrl);
            //qDebug() << "COOKIES for" << mUrl.host() << cookies;
            //qDebug() << result;
            //ui->webView->setHtml(result);//直接用QWebView的setHtml直接加载返回的html数据

            if(sendFlag == FLAG_LOGIN)
            {
                checkLogin();
            }
            else if(sendFlag == FLAG_ZH)
            {
                QString zhStr = processZHJson(bytes);
                qDebug() << zhStr;
                getZHDetail(zhStr);
            }
            else if(sendFlag == FLAG_ZHDETAIL)
            {
                processZHDetailJson(bytes);
            }

        }
        else
        {
            qDebug()<< "Error:" << reply->error();
        }
    }while(0);

}


/***槽函数：设置textEdit显示的内容***/
void MainWindow::itemClicked(QModelIndex index)
{
    StockZH stockZH = ui->listView->model()->data(index, Qt::UserRole).value<StockZH>();
    qDebug()<< stockZH.getSymbol();
}
