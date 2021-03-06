#ifndef HCMANUALRESERVEPAGE_H
#define HCMANUALRESERVEPAGE_H

#include <QWidget>
#include <QBitArray>

class ICCommandKeyWrapper;

namespace Ui {
    class HCManualReservePage;
}

class HCManualReservePage : public QWidget
{
    Q_OBJECT

public:
    explicit HCManualReservePage(QWidget *parent = 0);
    ~HCManualReservePage();

protected:
    void changeEvent(QEvent *e);
    void showEvent(QShowEvent *e);
    void hideEvent(QHideEvent *e);

private Q_SLOTS:
    void StatusRefreshed();

private:
    Ui::HCManualReservePage *ui;
    QList<ICCommandKeyWrapper*> wrappers_;
    QBitArray clips_;
};

#endif // HCMANUALRESERVEPAGE_H
