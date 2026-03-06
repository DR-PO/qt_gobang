#ifndef NETWORKDIALOG_H
#define NETWORKDIALOG_H

#include <QWidget>
#include <net.h>
#include <QMessageBox>
//#include <gameinterface.h>
namespace Ui {
class networkdialog;
}

class networkdialog : public QWidget
{
    Q_OBJECT

public:
    explicit networkdialog(QWidget *parent = nullptr);
    ~networkdialog();

private slots:

    void on_cancel_pushButton_clicked();

    void on_pushButton_clicked();

private:
    Ui::networkdialog *ui;
    Net *n;
};

#endif // NETWORKDIALOG_H
