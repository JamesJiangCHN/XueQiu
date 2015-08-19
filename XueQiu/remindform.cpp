#include "remindform.h"
#include "ui_remindform.h"

RemindForm::RemindForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RemindForm)
{
    ui->setupUi(this);
}

RemindForm::~RemindForm()
{
    delete ui;
}
