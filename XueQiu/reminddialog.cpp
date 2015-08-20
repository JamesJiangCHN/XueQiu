#include "reminddialog.h"
#include "ui_reminddialog.h"

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
