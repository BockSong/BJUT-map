#ifndef LOCATION_H
#define LOCATION_H

#include <QDialog>

namespace Ui {
class location;
}

class location : public QDialog
{
    Q_OBJECT

public:
    explicit location(QString str, QPixmap pic, int index, QWidget *parent = 0);
    ~location();

private slots:
    void on_pushButton_clicked();
    void on_pushButton2_clicked();
    void on_pushButton3_clicked();
    void sendemail();
    void addremark();
    void changeaddress(const QString &text);
    void changename(const QString &text);
    void changeremark(const QString &text);

private:
    Ui::location *ui;
    QString strforprint;
    QString strname;
    QString address;
    int picindex;
    QString username;
    QString remark;
};

#endif // LOCATION_H
