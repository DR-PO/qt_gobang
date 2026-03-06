#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <player.h>
#include <menu.h>
#include <QDebug>
#include <mydatabase.h>
#include <QMessageBox>
#include "networkdialog.h"
#include <QCryptographicHash>  // 添加密码加密所需头文件
#include <QRandomGenerator>     // 添加随机数生成所需头文件
#include <QDateTime>            // 添加时间相关头文件

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    //bool connectServer();

private slots:
    void pressButton();
    void on_register_Button_clicked();
    void on_change_ser_clicked();

private:
    Ui::MainWindow *ui;
    mydatabase db;           // 数据库对象
    bool isLoginMode;        // true:登录模式 false:注册模式

    bool initDatabase();                     // 初始化数据库
    void createUserTable();                  // 创建用户表
    QString generateSalt();                  // 生成盐值
    QString hashPassword(const QString &password, const QString &salt);  // 密码加密
    bool handleRegister(const QString &username, const QString &password);  // 处理注册
    bool handleLogin(const QString &username, const QString &password);     // 处理登录
    // enum TYPE{
    //     LOGIN = 0,
    //     REGISTER = 1
    // };
    // TYPE t;
    //Net n;
    //Menu menu;
};
#endif // MAINWINDOW_H
