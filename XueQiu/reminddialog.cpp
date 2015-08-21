#include "reminddialog.h"
#include "ui_reminddialog.h"

#include <QLabel>

RemindDialog::RemindDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RemindDialog)
{
    ui->setupUi(this);
    setWindowFlags ( windowFlags()|Qt::FramelessWindowHint|Qt::WindowTitleHint|Qt::WindowStaysOnTopHint);
}

RemindDialog::~RemindDialog()
{
    delete ui;
}

void RemindDialog::addLink(QString detail, QString url)
{

     QLabel* label = new QLabel(this);
     label->setOpenExternalLinks(true);
     label->setText("<style> a {text-decoration: none} </style><a href ="+url+">"+detail+ "</a>");
     label->show();
     widgetvec << label;
}

void RemindDialog::clearLinks()
{
    for(int i = 0; i < widgetvec.count(); i++)
    {
        if( widgetvec[i] != NULL)
        {
            delete widgetvec[i];
        }

    }

    widgetvec.clear();

}
