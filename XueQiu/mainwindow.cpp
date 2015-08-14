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
    byte_array.append(pwd);
    QString md5 = QCryptographicHash::hash(byte_array, QCryptographicHash::Md5).toHex();
    qDebug() <<md5 << endl;

    QString postString = "areacode=86&password=";
    postString.append(md5);
    postString.append("&remember_me=on&telephone=");
    postString.append(userName);
    QByteArray postData = postString.toUtf8();

    mNetRequest.setUrl(mUrl);
    //mNetRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    mNetRequest.setHeader(QNetworkRequest::UserAgentHeader, "Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/41.0.2272.118 UBrowser/5.2.3285.46 Safari/537.36");
    mNetRequest.setHeader(QNetworkRequest::ContentLengthHeader, postData.length());
    mNetManager->post(mNetRequest,postData);
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
            ui->webView->setHtml(result);//直接用QWebView的setHtml直接加载返回的html数据

            if(sendFlag == FLAG_ZH)
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

void MainWindow::getZH(QString uid)
{
    sendFlag = FLAG_ZH;
    //http://xueqiu.com/v4/stock/portfolio/stocks.json?size=1000&tuid=7739010226&pid=-1&category=1&type=1&_=1439457087039
    QString url = "http://xueqiu.com/v4/stock/portfolio/stocks.json?size=1000&tuid=";
    url.append(uid);
    url.append("&pid=-1&category=1&type=1&_=");

    qlonglong nTime = QDateTime::currentDateTimeUtc().currentMSecsSinceEpoch();
    QString nTimeStr =QString::number(nTime);

    url.append(nTimeStr);

    QVariant var;
    var.setValue(cookies);

    mUrl = QUrl(url);
    mNetRequest.setUrl(mUrl);
    mNetRequest.setHeader(QNetworkRequest::CookieHeader,var);
    mNetRequest.setHeader(QNetworkRequest::UserAgentHeader, "Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/41.0.2272.118 UBrowser/5.2.3285.46 Safari/537.36");
    mNetRequest.setHeader(QNetworkRequest::ContentLengthHeader, 0);
    mNetManager->get(mNetRequest);
}

void MainWindow::getZHDetail(QString zhStr)
{
    sendFlag = FLAG_ZHDETAIL;
    //http://xueqiu.com/v4/stock/portfolio/stocks.json?size=1000&tuid=7739010226&pid=-1&category=1&type=1&_=1439457087039
    QString url = "xueqiu.com/cubes/quote.json?code=";
    url.append(zhStr);
    url.append("&return_hasexist=false&_=");

    qlonglong nTime = QDateTime::currentDateTimeUtc().currentMSecsSinceEpoch();
    QString nTimeStr =QString::number(nTime);

    url.append(nTimeStr);

    QVariant var;
    var.setValue(cookies);

    mUrl = QUrl(url);
    mNetRequest.setUrl(mUrl);
    mNetRequest.setHeader(QNetworkRequest::CookieHeader,var);
    mNetRequest.setHeader(QNetworkRequest::UserAgentHeader, "Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/41.0.2272.118 UBrowser/5.2.3285.46 Safari/537.36");
    mNetRequest.setHeader(QNetworkRequest::ContentLengthHeader, 0);
    mNetManager->get(mNetRequest);

}

void MainWindow::on_pushButton_clicked()
{
     getZH(QString("7739010226"));
}


QString MainWindow::processZHJson(QByteArray zhArray)
{
    QString zhStr ="";
    standardItemModel = new QStandardItemModel(this);

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
            while (i.hasNext()) {
                i.next();
                QVariantMap stockMap = i.value().toMap();
                qDebug() << stockMap["symbol"].toString() << ": " <<  stockMap["net_value"].toString() << endl;
            }

        }
    }
}
