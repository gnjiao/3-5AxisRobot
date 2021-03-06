#include <QPushButton>
#include "ichcinjectionpage.h"
#include "ui_ichcinjectionpage.h"
#include "icvirtualhost.h"
#include "icactioncommand.h"
#include "icvirtualkey.h"

ICHCInjectionPage::ICHCInjectionPage(QWidget *parent) :
    ICInstructionEditorBase(parent),
    ui(new Ui::ICHCInjectionPage),
    onPixmap_(":/resource/ledgreen(16).png"),
    offPixmap_(":/resource/ledgray(16).png")

{
    ui->setupUi(this);

    QPushButton *buttons = new QPushButton[ui->tableWidget->rowCount()];
    QPushButton * button;
    ICLineEditWithVirtualNumericKeypad* delays = new ICLineEditWithVirtualNumericKeypad[ui->tableWidget->rowCount()];
    ICLineEditWithVirtualNumericKeypad *delayEdit;
    ioNames_<<tr("Close Mold Permit  ")<<tr("Ejection Permit  ")
           <<tr("Ejection BW Permit  ")
          <<tr("Core1 Permit  ")
         <<tr("Core1 Out Permit  ")
        <<tr("Core2 Permit  ")
       <<tr("Core2 Out Permit  ");
    onClipToOffClip_.insert(ICMold::ACTCLSMDON, ICMold::ACTCLSMDOFF);
    onClipToOffClip_.insert(ICMold::ACTEJECTON, ICMold::ACTEJECTOFF);
    onClipToOffClip_.insert(ICMold::ACTEJECTON + 100, ICMold::ACTEJECTOFF + 100);
    onClipToOffClip_.insert(ICMold::ACT_CORE1_ON, ICMold::ACT_CORE1_OFF);
    onClipToOffClip_.insert(ICMold::ACT_CORE1_ON + 100, ICMold::ACT_CORE1_OFF + 100);
    onClipToOffClip_.insert(ICMold::ACT_CORE2_ON, ICMold::ACT_CORE2_OFF);
    onClipToOffClip_.insert(ICMold::ACT_CORE2_ON + 100, ICMold::ACT_CORE2_OFF + 100);



    offClipToOnClip_.insert(ICMold::ACTCLSMDOFF, ICMold::ACTCLSMDON);
    offClipToOnClip_.insert(ICMold::ACTEJECTOFF, ICMold::ACTEJECTON);
    offClipToOnClip_.insert(ICMold::ACTEJECTOFF + 100, ICMold::ACTEJECTON + 100);
    offClipToOnClip_.insert(ICMold::ACT_CORE1_OFF, ICMold::ACT_CORE1_ON);
    offClipToOnClip_.insert(ICMold::ACT_CORE1_OFF + 100, ICMold::ACT_CORE1_ON + 100);
    offClipToOnClip_.insert(ICMold::ACT_CORE2_OFF, ICMold::ACT_CORE2_ON);
    offClipToOnClip_.insert(ICMold::ACT_CORE2_OFF + 100, ICMold::ACT_CORE2_ON + 100);


    //    QList<uint> initStatus = onClipToOffClip_.values();
    QList<uint> initStatus;
    initStatus<<ICMold::ACTCLSMDOFF<<ICMold::ACTEJECTOFF<<(ICMold::ACTEJECTOFF + 100)
             <<ICMold::ACT_CORE1_OFF<<(ICMold::ACT_CORE1_OFF + 100)
            <<ICMold::ACT_CORE2_OFF<<(ICMold::ACT_CORE2_OFF + 100);
    QIntValidator *validator = new QIntValidator(0, 30000, this);
    for(int i = 0; i != ui->tableWidget->rowCount(); ++i)
    {
        button = buttons + i;
        button->setIcon(offPixmap_);
        button->setText(ioNames_.at(i));

        delayEdit = delays + i;
        delayEdit->SetDecimalPlaces(2);
        delayEdit->setValidator(validator);
        delayEdit->setText("0.50");
        //        delayEdit->setFixedWidth(50);

        settingButtons_.append(button);
        delayEdits_.append(delayEdit);
        ui->tableWidget->setCellWidget(i, 1, button);
        ui->tableWidget->setCellWidget(i, 2, delayEdit);

        buttonToClip_.insert(button, initStatus.at(i));
        buttonSignalMapper_.setMapping(button, button);
        connect(button,
                SIGNAL(clicked()),
                &buttonSignalMapper_,
                SLOT(map()));
    }
    //    ui->tableWidget->resizeColumnsToContents();
#ifdef HC_SK_5
    ui->tableWidget->setColumnWidth(0, 46);
    ui->tableWidget->setColumnWidth(1, 110);
#else
    ui->tableWidget->setColumnWidth(0, 50);
    ui->tableWidget->setColumnWidth(1, 140);
#endif


    commandKeyMap_.insert(settingButtons_.at(0), qMakePair(static_cast<int>(IC::VKEY_CLSMDON), static_cast<int>(IC::VKEY_CLSMDOFF)));
    commandKeyMap_.insert(settingButtons_.at(1), qMakePair(static_cast<int>(IC::VKEY_EJECTON), static_cast<int>(IC::VKEY_EJECTOFF)));
    //    commandKeyMap_.insert(settingButtons_.at(2), qMakePair(static_cast<int>(IC::VKEY_LAYOUTON), static_cast<int>(IC::VKEY_LAYOUTOFF)));
    commandKeyMap_.insert(settingButtons_.at(2), qMakePair(0, 0));
    commandKeyMap_.insert(settingButtons_.at(3), qMakePair(0, 0));
    commandKeyMap_.insert(settingButtons_.at(4), qMakePair(0, 0));
    commandKeyMap_.insert(settingButtons_.at(5), qMakePair(0, 0));
    commandKeyMap_.insert(settingButtons_.at(6), qMakePair(0, 0));

//    ui->tableWidget->resizeColumnsToContents();
#ifdef HC_SK_5
    ui->tableWidget->setColumnWidth(0, 36);
    ui->tableWidget->setColumnWidth(1, 110);
#else
    ui->tableWidget->setColumnWidth(0, 50);
    ui->tableWidget->setColumnWidth(1, 160);
#endif


    commandKeyMap_.insert(settingButtons_.at(0), qMakePair(static_cast<int>(IC::VKEY_CLSMDON), static_cast<int>(IC::VKEY_CLSMDOFF)));
    commandKeyMap_.insert(settingButtons_.at(1), qMakePair(static_cast<int>(IC::VKEY_EJECTON), static_cast<int>(IC::VKEY_EJECTOFF)));
//    commandKeyMap_.insert(settingButtons_.at(2), qMakePair(static_cast<int>(IC::VKEY_LAYOUTON), static_cast<int>(IC::VKEY_LAYOUTOFF)));
    commandKeyMap_.insert(settingButtons_.at(2), qMakePair(0, 0));
    commandKeyMap_.insert(settingButtons_.at(3), qMakePair(0, 0));
    commandKeyMap_.insert(settingButtons_.at(4), qMakePair(0, 0));


    connect(&buttonSignalMapper_,
            SIGNAL(mapped(QWidget*)),
            this,
            SLOT(CommandButtonClicked(QWidget*)));
}

ICHCInjectionPage::~ICHCInjectionPage()
{
    delete ui;
}

void ICHCInjectionPage::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        ioNames_.clear();
        ioNames_<<tr("Close Mold Permit  ")<<tr("Ejection Permit  ")
               <<tr("Ejection BW Permit  ")
              <<tr("Core1 Permit  ")
             <<tr("Core1 Out Permit  ")
            <<tr("Core2 Permit  ")
           <<tr("Core2 Out Permit  ");
        for(int i = 0; i != settingButtons_.size(); ++i)
        {
            settingButtons_[i]->setText(ioNames_.at(i));
        }
        break;
    default:
        break;
    }
}

void ICHCInjectionPage::showEvent(QShowEvent *e)
{
    QWidget::showEvent(e);
    connect(ICVirtualHost::GlobalVirtualHost(),
            SIGNAL(StatusRefreshed()),
            this,
            SLOT(StatusRefreshed()));
}

void ICHCInjectionPage::hideEvent(QHideEvent *e)
{
    QWidget::hideEvent(e);
    disconnect(ICVirtualHost::GlobalVirtualHost(),
               SIGNAL(StatusRefreshed()),
               this,
               SLOT(StatusRefreshed()));
}

void ICHCInjectionPage::SyncStatusImpl(const QList<ICMoldItem> &items)
{
    Q_UNUSED(items)
}

void ICHCInjectionPage::StatusRefreshed()
{
    //    static ICVirtualHost* host = ICVirtualHost::GlobalVirtualHost();
    //    QMap<QLabel*, int>::iterator p = currentPosMap_.begin();
    //    while(p != currentPosMap_.end())
    //    {
    //        if(host->IsClipOn(p.value()))
    //        {
    //            if(!ledIsOnMap_.value(p.key()))
    //            {
    //                p.key()->setPixmap(onPixmap_);
    //                ledIsOnMap_.insert(p.key(), true);
    //            }
    //        }
    //        else
    //        {
    //            if(ledIsOnMap_.value(p.key()))
    //            {
    //                p.key()->setPixmap(offPixmap_);
    //                ledIsOnMap_.insert(p.key(), false);
    //            }
    //        }
    //        ++p;
    //    }
}

void ICHCInjectionPage::CommandButtonClicked(QWidget *widget)
{
    QAbstractButton* button = qobject_cast<QAbstractButton*>(widget);
    Q_ASSERT_X(button != NULL, "ICHCFixturePage::CommandButtonClicked", "widget is null");
    int key;
    int currentClip = buttonToClip_.value(button);
    if(onClipToOffClip_.contains(currentClip))
    {
        key = commandKeyMap_.value(button).second;
        buttonToClip_.insert(button, onClipToOffClip_.value(currentClip));
        button->setIcon(offPixmap_);
    }
    else
    {
        key = commandKeyMap_.value(button).first;
        buttonToClip_.insert(button, offClipToOnClip_.value(currentClip));
        button->setIcon(onPixmap_);
    }
    ICCommandProcessor::Instance()->ExecuteVirtualKeyCommand(key);

}


//void ICHCInjectionPage::on_closeMoldlDelayEdit_textChanged(QString )
//{
//    ICCommandProcessor::Instance()->TeachParam(ICVirtualHost::ST_DLY_CLSMDON, ui->closeMoldlDelayEdit->TransThisTextToThisInt());
//}

//void ICHCInjectionPage::on_closeMoldOtherEdit_textChanged(QString )
//{
//    ICCommandProcessor::Instance()->TeachParam(ICVirtualHost::ST_SPD_CLSMDON, ui->closeMoldOtherEdit->TransThisTextToThisInt());
//}

//void ICHCInjectionPage::on_ejecteDelayEdit_textChanged(QString )
//{
//    ICCommandProcessor::Instance()->TeachParam(ICVirtualHost::ST_DLY_EJECTON, ui->ejecteDelayEdit->TransThisTextToThisInt());
//}

//void ICHCInjectionPage::on_ejecteOtherEdit_textChanged(QString )
//{
//    ICCommandProcessor::Instance()->TeachParam(ICVirtualHost::ST_SPD_EJECTON, ui->ejecteOtherEdit->TransThisTextToThisInt());
//}

//void ICHCInjectionPage::on_placeDelayEdit_textChanged(QString )
//{
//    ICCommandProcessor::Instance()->TeachParam(ICVirtualHost::ST_DLY_LAYOUTON, ui->placeDelayEdit->TransThisTextToThisInt());
//}

//void ICHCInjectionPage::on_placeOtherEdit_textEdited(QString )
//{
//    ICCommandProcessor::Instance()->TeachParam(ICVirtualHost::ST_SPD_LAYOUTON, ui->placeOtherEdit->TransThisTextToThisInt());
//}

QList<ICMoldItem> ICHCInjectionPage::CreateCommandImpl() const
{
    QList<ICMoldItem> ret;

       ICMoldItem item;
       int clip;
       for(int i = 0; i != ui->tableWidget->rowCount(); ++i)
       {
           if(ui->tableWidget->item(i,0)->checkState() == Qt::Checked)
           {
               item.SetIFVal(0);
               clip = buttonToClip_.value(qobject_cast<QAbstractButton*>(ui->tableWidget->cellWidget(i, 1)));
               if(clip >= 100)
               {
                   item.SetClip(clip - 100);
                   item.SetIFVal(1);
               }
               else
                   item.SetClip(clip);
               item.SetDVal(delayEdits_.at(i)->TransThisTextToThisInt());
               ret.append(item);
           }
       }
       return ret;
}
