#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QCloseEvent>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowFlags ( windowFlags()|Qt::FramelessWindowHint|Qt::WindowTitleHint|Qt::WindowStaysOnTopHint);

    mNetManager = new QNetworkAccessManager(this);
    QObject::connect(mNetManager, SIGNAL(finished(QNetworkReply*)),
                 this, SLOT(finishedSlot(QNetworkReply*)));
    CreatTrayIcon();

    timer = new QTimer( this );
    connect(timer,SIGNAL(timeout()),SLOT(checkZHChange()));
    remind = new RemindDialog;
    qDebug() << "Desktop"+QString::number(desktop.availableGeometry().width()) + ", "+QString::number(desktop.availableGeometry().height());
    remind->move((desktop.availableGeometry().width()-remind->width()),
                 desktop.availableGeometry().height());//初始化位置到右下角
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
                item->setCheckable(true);
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
    timer->start(10000);
}

void MainWindow::checkZHChange()
{
    qDebug() << "Check";
    //停止 处理信息
    if (timer->isActive())
    {
        timer->stop();
    }

    int rows = standardItemModel->rowCount();
    for (int i=0;i<rows;++i)
    {
        QStandardItem* childItem = standardItemModel->item(i);
        if (childItem->checkState())
        {
            getZHChange(childItem->data(Qt::UserRole).value<StockZH>().getSymbol());
        }
    }

    //开启定时器
    timer->start(10000);
}

void MainWindow::getZHChange(QString zhSymbol)
{
    sendFlag = FLAG_ZHCHANGE;

    if(zhSymbol.isEmpty())
    {
        return;
    }
    qDebug() << "getChange" << zhSymbol;
    //http://xueqiu.com/cubes/rebalancing/history.json?cube_symbol=ZH191982&count=1&page=1
    QString url = "http://xueqiu.com/cubes/rebalancing/history.json?cube_symbol=";
    url.append(zhSymbol); //.replace(",", "%2C")
    url.append("&count=1&page=1");
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

void MainWindow::processZHChange(QByteArray zhChangeArray)
{
    QJsonParseError json_error;
    QJsonDocument parse_doucment = QJsonDocument::fromJson(zhChangeArray, &json_error);

    if(json_error.error == QJsonParseError::NoError)
    {
        if(parse_doucment.isObject())
        {
            QVariantMap result = parse_doucment.toVariant().toMap();

            int totalCount = result["totalCount"].toInt();
            qDebug() << QString::number(totalCount)+":";
            QVariantList list = result["list"].toList();

            QVariantList rebalancing_histories = list[0].toMap().value("rebalancing_histories").toList();

            QListIterator<QVariant> i(rebalancing_histories);
            while (i.hasNext()) {

                QVariantMap stockMap = i.next().toMap();
                QString dtStr =  QDateTime::currentDateTimeUtc()
                        .fromMSecsSinceEpoch(stockMap["created_at"].toULongLong())
                        .toString("yyyy-MM-dd hh:mm:ss");
                QString chStr = dtStr+"  "
                        +stockMap["stock_name"].toString()+" "
                        +stockMap["stock_symbol"].toString()+" ￥"
                        +stockMap["price"].toString()+"\n"
                        +stockMap["prev_weight"].toString()+"%  ->  "
                        +stockMap["target_weight"].toString()+"%\n";

                ui->statusBar->showMessage(chStr);
                qDebug() << chStr.toLocal8Bit();
                showAnimation();
            }
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
            else if(sendFlag == FLAG_ZHCHANGE)
            {
                processZHChange(bytes);
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

//关闭到托盘---------
void MainWindow::closeEvent(QCloseEvent *e)
{
    e->ignore();
    this->hide();
}

void MainWindow::CreatTrayIcon()
{
    CreatTrayMenu();

    if (!QSystemTrayIcon::isSystemTrayAvailable())      //判断系统是否支持系统托盘图标
    {
        return;
    }

    myTrayIcon = new QSystemTrayIcon(this);

    myTrayIcon->setIcon(QIcon(":/images/stock.ico"));   //设置图标图片
    setWindowIcon(QIcon(":/images/stock.ico"));  //把图片设置到窗口上

    myTrayIcon->setToolTip("Stock");    //托盘时，鼠标放上去的提示信息

    myTrayIcon->showMessage("Stock","Hi,This is my trayIcon",QSystemTrayIcon::Information,10000);
    myTrayIcon->setContextMenu(myMenu);     //设置托盘上下文菜单
    myTrayIcon->show();
    this->connect(myTrayIcon,SIGNAL(activated(QSystemTrayIcon::ActivationReason)),this,SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));

}

void MainWindow::CreatTrayMenu()
{
    restoreWinAction = new QAction("还 原(&R)",this);
    quitAction = new QAction("退出(&Q)",this);

    this->connect(restoreWinAction,SIGNAL(triggered()),this,SLOT(showNormal()));
    this->connect(quitAction,SIGNAL(triggered()),qApp,SLOT(quit()));

    myMenu = new QMenu((QWidget*)QApplication::desktop());

    myMenu->addAction(restoreWinAction);
    myMenu->addSeparator();     //加入一个分离符
    myMenu->addAction(quitAction);
}


void MainWindow::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch(reason)
    {
    case QSystemTrayIcon::Trigger:
    case QSystemTrayIcon::DoubleClick:
        showNormal();
        break;
    case QSystemTrayIcon::MiddleClick:
        myTrayIcon->showMessage("雪球","雪球第三方客户端",QSystemTrayIcon::Information,10000);
        break;

    default:
        break;
    }
}
/*侧边栏停靠*/
void MainWindow::enterEvent(QEvent *)
{
    QRect rc;
    QRect rect;
    rect = this->geometry();
    rc.setRect(rect.x(),rect.y(),rect.width(),rect.height());
    if(rect.top()<0)
    {
        rect.setX(rc.x());
        rect.setY(0);
        move(rc.x(),-2);
    }
}

void MainWindow::leaveEvent(QEvent *)
{
    QRect rc;
    QRect rect;
    rect = this->geometry();
    rc.setRect(rect.x(),rect.y(),rect.width(),rect.height());
    if(rect.top()<0)
    {
        move(rc.x(),-rc.height()+2);
    }
}

void MainWindow::mousePressEvent(QMouseEvent *lpEvent)
{
    //__super::mousePressEvent(lpEvent);
    if (lpEvent->button() == Qt::LeftButton)
    {
        m_WindowPos = this->pos();
        m_MousePos = lpEvent->globalPos();
        this->m_MousePressed = true;
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent *lpEvent)
{
    //__super::mouseReleaseEvent(lpEvent);
    if (lpEvent->button() == Qt::LeftButton)
    {
        this->m_MousePressed = false;
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *lpEvent)
{
    if (m_MousePressed)
    {
        this->move(m_WindowPos + (lpEvent->globalPos() - m_MousePos));
    }
}

void MainWindow::on_pushButton_clicked()
{
    this->hide();
}

//弹出动画
void MainWindow::showAnimation(){
    qDebug() << "Show" + QString::number(remind->x()) + "," + QString::number(remind->y());
    remind->show();
    //显示弹出框动画
    animation=new QPropertyAnimation(remind,"pos");
    animation->setDuration(2000);
    animation->setStartValue(QPoint(remind->x(),remind->y()));
    animation->setEndValue(QPoint((desktop.availableGeometry().width()-remind->width()),
                                  (desktop.availableGeometry().height()-remind->height())));
    animation->start();

    //设置弹出框显示2秒、在弹回去
    remainTimer=new QTimer();
    connect(remainTimer,SIGNAL(timeout()),this,SLOT(closeAnimation()));
    remainTimer->start(4000);//弹出动画2S,停留2S回去
}
//关闭动画
void MainWindow::closeAnimation(){
    qDebug() << "End" + QString::number(remind->x()) + "," + QString::number(remind->y());
    if(remainTimer!=NULL){
        //清除Timer指针和信号槽
        remainTimer->stop();
        disconnect(remainTimer,SIGNAL(timeout()),this,SLOT(closeAnimation()));
        delete remainTimer;
        remainTimer=NULL;
    }
    //弹出框回去动画
    animation->setStartValue(QPoint(remind->x(),remind->y()));
    animation->setEndValue(QPoint((desktop.availableGeometry().width()-remind->width()),
                                  desktop.availableGeometry().height()));
    animation->start();
    //弹回动画完成后清理动画指针
    connect(animation,SIGNAL(finished()),this,SLOT(clearAll()));
}
//清理动画指针
void MainWindow::clearAll(){
    remind->hide();
    if(animation!=NULL)
    {
        disconnect(animation,SIGNAL(finished()),this,SLOT(clearAll()));
        delete animation;
        animation=NULL;
    }
}


