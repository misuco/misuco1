#ifndef DIALOGNET_H
#define DIALOGNET_H

#include <QWidget>
#include <QTextEdit>
#include <QGridLayout>
#include <QPushButton>

class DialogNet : public QWidget
{
    Q_OBJECT
public:
    explicit DialogNet(QWidget *parent = 0);

signals:

public slots:

private:
    //    QLineEdit *lineEdit;
    QTextEdit *lineEdit;
    QGridLayout *layout;
    QPushButton *button;
};

#endif // DIALOGNET_H
