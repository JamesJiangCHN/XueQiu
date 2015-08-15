#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextCodec>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QNetworkCookieJar>
#include <QNetworkCookie>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>

#include "stockzh.h"

#define FLAG_LOGIN      1
#define FLAG_ZH         2
#define FLAG_ZHDETAIL   3

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    QNetworkRequest mNetRequest;
    QNetworkAccessManager *mNetManager;
    QUrl mUrl;
    QList<QNetworkCookie>  cookies;

    QStandardItemModel *standardItemModel;

    QString uid;
    int sendFlag;


    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_pb_login_clicked();
    void finishedSlot(QNetworkReply *reply);

    void on_pushButton_clicked();
    void itemClicked(QModelIndex index);

private:
    Ui::MainWindow *ui;

    void login(QString userName, QString pwd);

    void getZH(QString uid);
    void getZHDetail(QString zhStr);
    QString processZHJson(QByteArray zhArray);
    void processZHDetailJson(QByteArray zhDetailArray);

};

Q_DECLARE_METATYPE(StockZH)

#endif // MAINWINDOW_H
