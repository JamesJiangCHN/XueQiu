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
    QUrl pUrl("http://xueqiu.com/user/login");

    // make password to md5
    QByteArray byte_array;
    byte_array.append(pwd);
    QString md5 = QCryptographicHash::hash(byte_array, QCryptographicHash::Md5).toHex();
    qDebug() <<md5 << endl;

    QString jsonString = "areacode=86&password=";
    jsonString.append(md5);
    jsonString.append("&remember_me=on&telephone=");
    jsonString.append(userName);
    QByteArray json = jsonString.toUtf8();

    mNetRequest.setUrl(pUrl);
    mNetRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    mNetRequest.setHeader(QNetworkRequest::ContentLengthHeader, json.length());
    mNetRequest.setRawHeader("User-Agent", "Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/41.0.2272.118 UBrowser/5.2.3285.46 Safari/537.36");
    mNetRequest.setRawHeader("Content-Type", "application/x-www-form-urlencoded; charset=UTF-8");
    mNetRequest.setRawHeader("Content-Length", QString(json.length()).toUtf8());

    QNetworkReply* reply = mNetManager->post(mNetRequest,json);//baseString.toUtf8());
}

// login request callback function
void MainWindow::finishedSlot(QNetworkReply *reply)
{
    QVariant statusCode =
            reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    qDebug() << "statusCode is " << statusCode.toString();
    QString contentType = reply->header(QNetworkRequest::ContentTypeHeader).toString();
    QString charset = contentType.mid(contentType.indexOf("charset=") + QString("charset=").length());
    //int result = 0;
    do
    {
        if (statusCode.toInt() == 301 || statusCode.toInt() == 302)
        {
            //reply->c
            // Or the target URL if it was a redirect:
            QVariant redirectionTargetUrl =
                reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
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
            ui->webView->setHtml(result);//直接用QWebView的setHtml直接加载返回的html数据
            //qDebug() << string;
        }
    }while(0);

}
