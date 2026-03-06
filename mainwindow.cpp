#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("欢乐五子棋");
    //信号和槽函数连接
    isLoginMode = true;  // 默认为登录模式

    // 初始化数据库连接
    initDatabase();

    connect(this->ui->pushButton, SIGNAL(clicked()), this, SLOT(pressButton()));
    //qDebug() << "登录信号槽连接完成";
    //connect(this->ui->register_Button,SIGNAL(clicked()),this,SLOT(on_register_Button_clicked()));存在粘连，鼠标按下和松开算两次点击
    // 先断开所有可能的旧连接
    disconnect(ui->register_Button, nullptr, nullptr, nullptr);
    connect(ui->register_Button, &QPushButton::pressed, this, &MainWindow::on_register_Button_clicked);
    //qDebug() << "注册信号槽连接完成";
}

MainWindow::~MainWindow()
{
    delete ui;
}
// bool MainWindow::connectServer(){
//     if(!n.connect()){
//         n.socket.close();
//         QMessageBox::StandardButton result=QMessageBox::critical(this, "错误", "无法连接到服务器！");
//         return false;
//     }
//     return true;
// }
bool MainWindow::initDatabase() {
    // 设置数据库连接参数
    db.setMhost("localhost");     // 数据库服务器地址
    db.setMport(3306);             // 数据库端口
    db.setMdbName("gobang");       // 数据库名
    db.setMuser("gobang_user");     // 数据库用户名
    db.setMpwd("123456");    // 数据库密码

    // 尝试连接数据库
    if (!db.connectSql("gobang")) {
        QMessageBox::warning(this, "错误", "无法连接到数据库！");
        return false;
    }

    // 检查并创建用户表
    createUserTable();
    qDebug() << "创建用户表";
    return true;
}

// 创建用户表（如果不存在）
void MainWindow::createUserTable() {
    QString createTableSql =
        "CREATE TABLE IF NOT EXISTS users ("
        "id INT PRIMARY KEY AUTO_INCREMENT, "
        "username VARCHAR(50) UNIQUE NOT NULL, "
        "password_hash CHAR(64) NOT NULL, "
        "salt CHAR(32) NOT NULL, "
        "created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP, "
        "last_login TIMESTAMP NULL, "
        "total_games INT DEFAULT 0, "
        "wins INT DEFAULT 0, "
        "losses INT DEFAULT 0, "
        "draws INT DEFAULT 0"
        ")";

    db.queryExec(createTableSql);
}

// 生成随机盐值
QString MainWindow::generateSalt() {
    const QString possibleCharacters("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");
    const int saltLength = 16;
    QString salt;

    for(int i = 0; i < saltLength; ++i) {
        int index = QRandomGenerator::global()->bounded(possibleCharacters.length());
        salt.append(possibleCharacters.at(index));
    }
    return salt;
}

// 密码加密（盐值 + 密码 -> SHA256）
QString MainWindow::hashPassword(const QString &password, const QString &salt) {
    QString combined = salt + password;
    QByteArray hash = QCryptographicHash::hash(combined.toUtf8(), QCryptographicHash::Sha256);
    return QString(hash.toHex());
}

// 处理注册
bool MainWindow::handleRegister(const QString &username, const QString &password) {
    // 检查用户名是否已存在
    QString checkSql = QString("SELECT id FROM users WHERE username = '%1'").arg(username);
    QList<QHash<QString, QString>> result;

    if (db.queryExec(checkSql, result) && !result.isEmpty()) {
        return false; // 用户名已存在
    }

    // 生成盐值和密码哈希
    QString salt = generateSalt();
    QString passwordHash = hashPassword(password, salt);

    // 插入新用户
    QHash<QString, QString> userData;
    userData["username"] = username;
    userData["password_hash"] = passwordHash;
    userData["salt"] = salt;

    return db.addData("users", userData);
}

// 处理登录
bool MainWindow::handleLogin(const QString &username, const QString &password) {
    // 查询用户信息
    QString sqlWhere = QString("WHERE username = '%1'").arg(username);
    QList<QHash<QString, QString>> result;

    if (!db.getData("users", result, sqlWhere) || result.isEmpty()) {
        return false; // 用户不存在
    }

    QHash<QString, QString> userInfo = result.first();
    QString storedHash = userInfo["password_hash"];
    QString salt = userInfo["salt"];

    // 验证密码
    QString inputHash = hashPassword(password, salt);

    if (inputHash == storedHash) {
        // 更新最后登录时间
        QString updateSql = QString("UPDATE users SET last_login = NOW() WHERE username = '%1'")
                                .arg(username);
        db.queryExec(updateSql);
        return true;
    }

    return false;
}
/*
 * 进入按钮的槽函数
 */
void MainWindow::pressButton(){
    QString username = this->ui->lineEdit->text().trimmed();
    QString password = this->ui->lineEdit_2->text();

    if(username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "警告", "用户名和密码不能为空！");
        return;
    }

    if(isLoginMode) {
        // 登录模式
        qDebug()<<"登录登录模式";
        if (handleLogin(username, password)) {
            QMessageBox::information(this, "成功", "登录成功！");

            // 创建玩家并进入菜单
            Player p(username);
            Menu *menu = new Menu();
            menu->setPlayer(p);
            menu->show();
            this->close();
        } else {
            QMessageBox::warning(this, "失败", "用户名或密码错误！");
        }
    } else {
        // 注册模式
        qDebug()<<"注册登录模式";
        if (handleRegister(username, password)) {
            QMessageBox::information(this, "成功", "注册成功！请登录");
            ui->lineEdit->clear();
            ui->lineEdit_2->clear();
            isLoginMode = true;
            ui->pushButton->setText("登录");
            ui->register_Button->setText("注册");
        } else {
            QMessageBox::warning(this, "失败", "注册失败！用户名已存在");
        }
    }
}

void MainWindow::on_register_Button_clicked() {
    isLoginMode = !isLoginMode;  // 切换模式
    if(isLoginMode) {
        ui->pushButton->setText("登录");
        ui->register_Button->setText("注册");
        ui->lineEdit->setPlaceholderText("输入你的昵称");
        ui->lineEdit_2->setPlaceholderText("输入你的密码");
        qDebug() << "切换到登录模式";
    } else {
        ui->pushButton->setText("注册");
        ui->register_Button->setText("返回登录");
        ui->lineEdit->setPlaceholderText("设置用户名");
        ui->lineEdit_2->setPlaceholderText("设置密码");
        qDebug() << "切换到注册模式";
    }
    ui->lineEdit->clear();
    ui->lineEdit_2->clear();
}

void MainWindow::on_change_ser_clicked()
{
    networkdialog *nwd=new networkdialog();
    nwd->show();
}

