#include "networkdialog.h"
#include "ui_networkdialog.h"

networkdialog::networkdialog(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::networkdialog)
{
    ui->setupUi(this);
    setWindowTitle("连接服务器");
}

networkdialog::~networkdialog()
{
    delete ui;
}


void networkdialog::on_cancel_pushButton_clicked()
{
    this->close();
}


void networkdialog::on_pushButton_clicked()
{
    QString ip(this->ui->ip_lineEdit->text());
    QString port(this->ui->port_lineEdit->text());
    n = new Net(ip,port.toInt());
    if(!n->connect()){
        QMessageBox mess(QMessageBox::Information, tr("消息"), tr("连接服务器失败！"));
        QPushButton *quit = (mess.addButton(tr("退出"), QMessageBox::AcceptRole));
        mess.exec();
        if(mess.clickedButton() == quit){
            mess.close();
        }
    }
}

