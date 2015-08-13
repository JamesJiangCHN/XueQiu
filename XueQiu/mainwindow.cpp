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

    mNetRequest.setUrl(QUrl("http://xueqiu.com/user/login"));
    mNetRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    mNetRequest.setHeader(QNetworkRequest::UserAgentHeader, "Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/41.0.2272.118 UBrowser/5.2.3285.46 Safari/537.36");
    mNetRequest.setHeader(QNetworkRequest::ContentLengthHeader, postData.length());
    mNetRequest.setRawHeader("Origin","http://xueqiu.com");
    mNetRequest.setRawHeader("Referer", "http://xueqiu.com/7739010226");
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
            //break;
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
            qDebug() << result;
            ui->webView->setHtml(result);//直接用QWebView的setHtml直接加载返回的html数据

        }
        else
        {
            qDebug()<< "Error:" << reply->error();
        }
    }while(0);

}

void MainWindow::getZH(QString uid)
{
    //http://xueqiu.com/v4/stock/portfolio/stocks.json?size=1000&tuid=7739010226&pid=-1&category=1&type=1&_=1439457087039 1439460188
    QString url = "http://xueqiu.com/v4/stock/portfolio/stocks.json?size=1000&tuid=";
    url.append(uid);
    url.append("&pid=-1&category=1&type=1&_=");

    QString postString = "size=1000&tuid=";
    postString.append(uid);
    postString.append("&pid=-1&category=1&type=1&_=");

    qlonglong nTime = QDateTime::currentDateTimeUtc().currentMSecsSinceEpoch();
    QString nTimeStr =QString::number(nTime);

    url.append(nTimeStr);
    postString.append(nTimeStr);
    QByteArray postData = postString.toUtf8();
    qDebug() << postString;

    mNetRequest.setUrl(QUrl(url));
    mNetRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    mNetRequest.setHeader(QNetworkRequest::UserAgentHeader, "Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/41.0.2272.118 UBrowser/5.2.3285.46 Safari/537.36");
    mNetRequest.setHeader(QNetworkRequest::ContentLengthHeader, postData.length());
    mNetManager->post(mNetRequest,postData);
}

void MainWindow::on_pushButton_clicked()
{
     getZH(QString("7739010226"));
}
