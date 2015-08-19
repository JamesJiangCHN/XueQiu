#ifndef REMINDFORM_H
#define REMINDFORM_H

#include <QWidget>

namespace Ui {
class RemindForm;
}

class RemindForm : public QWidget
{
    Q_OBJECT

public:
    explicit RemindForm(QWidget *parent = 0);
    ~RemindForm();

private:
    Ui::RemindForm *ui;
};

#endif // REMINDFORM_H
