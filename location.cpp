#include "location.h"
#include "ui_location.h"
#include <QTextCodec>
#include <QLineEdit>
#include <iostream>
#include <time.h>
#include "Smtp.h"

location::location(QString str, QPixmap pic, int index, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::location)
{
    ui->setupUi(this);
    QFont fonthere("华文彩云", 16);
    fonthere.setBold (true);
    ui->labelname->setFont(fonthere);
    QFont fonthere2("微软雅黑", 9);
    fonthere2.setBold (true);
    ui->textBrowser2->setFont(fonthere2);
    QFont fonthere3("仿宋", 9);
    fonthere3.setBold (false);
    ui->textBrowser->setFont(fonthere3);
    ui->labelname->setText(str);
    ui->label->setPixmap (pic);
    this->picindex=index;
    this->strname=str;
}

location::~location()
{
    delete ui;
}

void location::on_pushButton_clicked()
{
    strforprint.sprintf("用餐地点:\n风味餐厅 5/5.0\n奥运餐厅 5/5.0\n自习地点:\n旧图 5/5.0\n三教 5/5.0\n住宿地点:\n建国饭店\n");
    ui->textBrowser2->append(strforprint);
}

void location::on_pushButton2_clicked()
{
    QDialog *emailDlg = new QDialog;
    QLabel *emailLabel = new QLabel(tr("请输入您的邮箱地址:"));
    QLineEdit *inputemail = new QLineEdit(this);
    QPushButton *okBtn = new QPushButton(tr("确认"));
    QPushButton *cancelBtn = new QPushButton(tr("关闭"));
    connect (inputemail, SIGNAL(textChanged(const QString &)), this, SLOT(changeaddress(const QString &)));
    connect (okBtn, SIGNAL(clicked()), this, SLOT(sendemail()));
    connect (cancelBtn, SIGNAL(clicked()), emailDlg, SLOT(close()));

    QGridLayout *emaillayout = new QGridLayout(emailDlg);
    emaillayout->addWidget (emailLabel, 0, 0);
    emaillayout->addWidget (inputemail, 0, 1);
    emaillayout->addWidget (okBtn, 1, 1, 1, 1);
    emaillayout->addWidget (cancelBtn, 1, 2, 1, 1);

    emailDlg->setWindowTitle (tr("定制邮件"));
    emailDlg->exec();
}

void location::changeaddress(const QString &text)
{
    this->address=text;
}

void location::sendemail()
{
    QString filePath,content;
    content.sprintf("Hello! Thank you for choosing us.\nHere are the location information you have ordered.\nPlease check your attachment!\n\n\nSincerely, BJUTmap Team");
    string strhere=content.toStdString();
    string stremail=this->address.toStdString();
    CSmtp smtp(
            25,								//smtp端口
            "smtp.163.com",					//smtp服务器地址
            "testfordsa@163.com",	        //客户端邮箱地址
            "shouquantest1121",					//邮箱密码
            stremail/*"testfordsa@163.com"*/,	//目的邮箱地址
            "Welcome to BJUTmap!",							//主题
            strhere		//邮件正文
        );
    filePath.sprintf("D:\\image\\%d.jpg", this->picindex);
    strhere=filePath.toStdString();
    smtp.AddAttachment(strhere);
    int err;
    ui->textBrowser2->append(QString::fromStdString(stremail));
    if ((err = smtp.SendEmail_Ex()) != 0)
    {
        if (err == 1){
            ui->textBrowser2->setTextColor(QColor(255,0,0));
            strforprint.sprintf("错误1: 出现故障!请检查邮箱名称或网络连接.");
            ui->textBrowser2->append(strforprint);
            ui->textBrowser2->setTextColor(QColor(0,0,0));
        }
        if (err == 2){
            ui->textBrowser2->setTextColor(QColor(255,0,0));
            strforprint.sprintf("错误2: 客户端邮箱用户名错误!.");
            ui->textBrowser2->append(strforprint);
            ui->textBrowser2->setTextColor(QColor(0,0,0));
        }
        if (err == 3){
            ui->textBrowser2->setTextColor(QColor(255,0,0));
            strforprint.sprintf("错误3: 客户端邮箱密码错误!.");
            ui->textBrowser2->append(strforprint);
            ui->textBrowser2->setTextColor(QColor(0,0,0));
        }
        if (err == 4){
            ui->textBrowser2->setTextColor(QColor(255,0,0));
            strforprint.sprintf("错误4: 读取附件失败!.");
            ui->textBrowser2->append(strforprint);
            ui->textBrowser2->setTextColor(QColor(0,0,0));
        }
        return;
    }
    ui->textBrowser2->setTextColor(QColor(255,0,0));
    strforprint.sprintf("邮件成功发出");
    ui->textBrowser2->append(strforprint);
    ui->textBrowser2->setTextColor(QColor(0,0,0));
}

void location::on_pushButton3_clicked()
{
    QDialog *remarkDlg = new QDialog;
    QLabel *remarkLabel = new QLabel(tr("昵称:"));
    QLabel *remarkLabel2 = new QLabel(tr("评论内容:"));
    QLineEdit *input1 = new QLineEdit(this);
    QLineEdit *input2 = new QLineEdit(this);
    QPushButton *okBtn = new QPushButton(tr("提交"));
    QPushButton *cancelBtn = new QPushButton(tr("关闭"));
    connect (input1, SIGNAL(textChanged(const QString &)), this, SLOT(changename(QString)));
    connect (input2, SIGNAL(textChanged(const QString &)), this, SLOT(changeremark(QString)));
    connect (okBtn, SIGNAL(clicked()), this, SLOT(addremark()));
    connect (cancelBtn, SIGNAL(clicked()), remarkDlg, SLOT(close()));

    QGridLayout *remarklayout = new QGridLayout(remarkDlg);
    remarklayout->addWidget (remarkLabel, 0, 0);
    remarklayout->addWidget (input1, 0, 1);
    remarklayout->addWidget (remarkLabel2, 1, 0);
    remarklayout->addWidget (input2, 1, 1);
    remarklayout->addWidget (okBtn, 2, 1, 1, 1);
    remarklayout->addWidget (cancelBtn, 2, 2, 1, 1);

    remarkDlg->setWindowTitle (tr("添加评论"));
    remarkDlg->exec();
}

void location::changename(const QString &text)
{
    this->username=text;
}

void location::changeremark(const QString &text)
{
    this->remark=text;
}

void location::addremark()
{
    time_t tt = time(NULL);
    tm* t= localtime(&tt);
    strforprint.sprintf("<p style=\" margin-top:2px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; line-height:20px; background-color:#ffffff;\"><span style=\" font-family:'Arial,Helvetica,SimSun,sans-serif'; color:#666666; background-color:#ffffff;\">");
    strforprint.append(this->remark);
    strforprint.append("</span></p><p style=\" margin-top:2px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; line-height:20px; background-color:#ffffff;\"><span style=\" font-family:'Arial,Helvetica,SimSun,sans-serif'; color:#999999; background-color:#ffffff;\">");
    strforprint.append(this->username);
    strname.sprintf(" %d-%02d-%02d %02d:%02d:%02d",t->tm_year + 1900,t->tm_mon + 1,t->tm_mday, t->tm_hour,t->tm_min,t->tm_sec);
    strforprint.append(strname);
    strforprint.append("</span></p></body></html>");
    ui->textBrowser->append(strforprint);
}
