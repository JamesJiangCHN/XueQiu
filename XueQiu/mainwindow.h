#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextCodec>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QNetworkCookieJar>
#include <QNetworkCookie>

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


    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_pb_login_clicked();
    void finishedSlot(QNetworkReply *reply);

    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;

    void login(QString userName, QString pwd);

    void getZH(QString uid);



};

#endif // MAINWINDOW_H
