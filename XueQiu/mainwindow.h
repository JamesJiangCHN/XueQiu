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
#include <QMessageBox>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QTimer>
#include <QPropertyAnimation>
#include <QDesktopWidget>

#include "remindform.h"
#include "stockzh.h"

#define FLAG_LOGIN      1
#define FLAG_ZH         2
#define FLAG_ZHDETAIL   3
#define FLAG_ZHCHANGE   4

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    QString uid;
    int sendFlag;


    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_pb_login_clicked();
    void finishedSlot(QNetworkReply *reply);
    void itemClicked(QModelIndex index);
    void closeEvent(QCloseEvent *e);

    void iconActivated(QSystemTrayIcon::ActivationReason reason);
    void enterEvent(QEvent *);
    void leaveEvent(QEvent *);

    void on_pushButton_clicked();

    void mousePressEvent(QMouseEvent *lpEvent);
    void mouseReleaseEvent(QMouseEvent *lpEvent);
    void mouseMoveEvent(QMouseEvent *lpEvent);

    void checkZHChange();

    void closeAnimation();
    void clearAll();
private:
    Ui::MainWindow *ui;

    QNetworkRequest mNetRequest;
    QNetworkAccessManager *mNetManager;
    QUrl mUrl;
    QList<QNetworkCookie>  cookies;

    QStandardItemModel *standardItemModel;

    QSystemTrayIcon *myTrayIcon;
    QMenu *myMenu;
    QAction *restoreWinAction;
    QAction *quitAction;

    QTimer *timer;

    bool    m_MousePressed;
    QPoint  m_MousePos;
    QPoint  m_WindowPos;

    QPropertyAnimation* animation;
    QTimer *remainTimer;
    QDesktopWidget desktop;
    RemindForm *remind;


    void CreatTrayMenu();
    void CreatTrayIcon();

    void login(QString userName, QString pwd);
    void checkLogin();
    void getZH(QString uid);
    void getZHDetail(QString zhStr);
    QString processZHJson(QByteArray zhArray);
    void processZHDetailJson(QByteArray zhDetailArray);

    void getZHChange(QString zhSymbol);
    void processZHChange(QByteArray zhChangeArray);

     void showAnimation();
};

Q_DECLARE_METATYPE(StockZH)

#endif // MAINWINDOW_H
