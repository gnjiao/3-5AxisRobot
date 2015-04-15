#ifndef ICHCPRODUCTSETTINGFRAME_H
#define ICHCPRODUCTSETTINGFRAME_H

#include <QFrame>
#include <QList>
#include <QButtonGroup>
#include <QSignalMapper>
#include <QMap>
#include "icconfigstring.h"
class ICLineEditWrapper;

namespace Ui {
    class ICHCProductSettingFrame;
}

class ICHCProductSettingFrame : public QFrame
{
    Q_OBJECT

public:
    explicit ICHCProductSettingFrame(QWidget *parent = 0);
    ~ICHCProductSettingFrame();

protected:
    void hideEvent(QHideEvent *e);
    void changeEvent(QEvent *e);

private slots:
    void OnMoldNumberParamChanged();

    void on_productClearButton_clicked();

    void FixtureBoxChange();
    void retranslateUi_();

    void on_countUnitBox_currentIndexChanged(int index);

    void on_getFailWay_activated(int index);

//    void on_fixtureComboBox_currentIndexChanged(int index);

    void on_productSave_toggled(bool checked);

private:
    Ui::ICHCProductSettingFrame *ui;
    QList<ICLineEditWrapper*> wrappers_;
    QButtonGroup* buttongroup_ ;
    void InitCheckBox();

private slots:
    void OnConfigChanged(QObject* w, const QString& newV, const QString& oldV);
    void OnConfigChanged(const QString& text);
    void OnConfigChanged(int v);
    void OnConfigChanged(int v, int ov);
    void OnConfigChanged(bool b);
private:
    QMap<QObject*, int> editorToConfigIDs_;
};

#endif // ICHCPRODUCTSETTINGFRAME_H
