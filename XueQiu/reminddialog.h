#ifndef REMINDDIALOG_H
#define REMINDDIALOG_H

#include <QDialog>

namespace Ui {
class RemindDialog;
}

class RemindDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RemindDialog(QWidget *parent = 0);
    ~RemindDialog();

    void addLink(QString detail, QString url);
    void clearLinks();
private:
    Ui::RemindDialog *ui;

    QVector<QWidget*> widgetvec;
};

#endif // REMINDDIALOG_H
