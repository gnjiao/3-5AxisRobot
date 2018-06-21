#include <QTranslator>
#include <QDir>
#include <QFile>
#include "icmessagebox.h"
#include <QFileDialog>
#include <QStringListModel>

#include "ichcsystemsettingsframe.h"
#include "ui_ichcsystemsettingsframe.h"
#include "icparameterssave.h"
#include "icprogramheadframe.h"
#include "icadminpassworddialog.h"
#include "icvirtualhost.h"
#include "iccommands.h"
#include "icbackuputility.h"
#include "ictipswidget.h"
#include "icprogramformatchecker.h"
#include "icconfigformatchecker.h"
#include <QDebug>
#include "mainframe.h"
#include "icutility.h"
#include "icconfigstring.h"

#include "iccycletimeandfinishedframe.h"

static void BuildShiftMap(int beg, int* map)
{
    int index = 0;
    for(int i = beg; i < 10; ++i)
    {
        map[index++] = i;
    }
    for(int i = 0; i < beg; ++i)
    {
        map[index++] = i;
    }
}

ICHCSystemSettingsFrame::ICHCSystemSettingsFrame(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::ICHCSystemSettingsFrame)
{
    ui->setupUi(this);
    buttonGroup_ = new QButtonGroup();

    ui->languageButtonGroup->setId(ui->chineseBox,0);
    ui->languageButtonGroup->setId(ui->englishBox,1);
    ui->languageButtonGroup->setId(ui->spanishBox, 2);
    ui->languageButtonGroup->setId(ui->nederlandBox, 3);
    ui->languageButtonGroup->setId(ui->franceBox, 4);
    ui->languageButtonGroup->setId(ui->koreanBox, 5);
    ui->languageButtonGroup->setId(ui->hkBox, 6);
    ui->languageButtonGroup->setId(ui->itBox, 7);
    ui->languageButtonGroup->setId(ui->polskiBox, 8);
    ui->languageButtonGroup->setId(ui->turkeyBox, 9);
    ui->languageButtonGroup->setId(ui->portugalBox, 10);
    InitParameter();
    ui->extentFunctionCheckBox->blockSignals(true);
    ui->extentFunctionCheckBox->setChecked(ICParametersSave::Instance()->IsExtentFunctionUsed());
    ui->extentFunctionCheckBox->blockSignals(false);
    ui->limitFunctionBox->blockSignals(true);
    ui->limitFunctionBox->setChecked(ICParametersSave::Instance()->IsRegisterFunctinOn());
    ui->limitFunctionBox->blockSignals(false);

    ui->programModeGroup->setId(ui->simpleMode, 0);
    ui->programModeGroup->setId(ui->teachMode, 1);

    ui->programModeGroup->button(ICParametersSave::Instance()->ProgramMode())->setChecked(true);

    connect(ui->programModeGroup, SIGNAL(buttonClicked(int)), SLOT(OnProgramModeChanged(int)));


    QAbstractButton* button;
    foreach(button, buttonGroup_->buttons())
    {
        button->setCheckable(true);
    }

    connect(ICProgramHeadFrame::Instance(),
            SIGNAL(LevelChanged(int)),
            this,
            SLOT(OnLevelChanged(int)));
    armXStructValueToName_.insert(0, tr("None"));
    armXStructValueToName_.insert(1, tr("Only Backward Limit"));
    armXStructValueToName_.insert(2, tr("Only Forward Limit"));
    armXStructValueToName_.insert(3, tr("Both"));
    armYStructValueToName_.insert(1, tr("Only Up Limit"));
    armYStructValueToName_.insert(3, tr("Up and Donw Limit"));
    armValueToName_.insert(0, tr("Single Arm"));
    armValueToName_.insert(1, tr("Double Arm"));
    ui->backLightTimeEdit->setValidator(new QIntValidator(0, 120, this));
    ui->backLightTimeEdit->SetThisIntToThisText(ICParametersSave::Instance()->BackLightTime());
    ui->brightnessBar->setValue((ICParametersSave::Instance()->Brightness()));

    QList<QAbstractButton*> buttons_ = ui->languageButtonGroup->buttons();

    for(int i = 0; i != buttons_.size(); ++i)
    {
        buttons_[i]->setCheckable(true);
        connect(buttons_.at(i),
                SIGNAL(clicked()),
                this,
                SLOT(languageBoxChange()));
//        connect(buttons_at(i),
//                SIGNAL(clicked()),
//                this,
//                SLOT(language_Boxchange()));
    }

#ifndef Q_WS_WIN32
    uname(&osInfo_);
#endif

    testvalue = FALSE;
//    ui->systemConfigPages->removeTab(4);
    ui->factoryCode->blockSignals(true);
    ui->factoryCode->setText(ICParametersSave::Instance()->FacotryCode());
    ui->factoryCode->blockSignals(false);
    ui->listView->setSpacing(2);

    editorToConfigIDs_.insert(ui->languageButtonGroup, ICConfigString::kCS_PANEL_Language);
    editorToConfigIDs_.insert(ui->backLightTimeEdit, ICConfigString::kCS_PANEL_Backlight);
    editorToConfigIDs_.insert(ui->extentFunctionCheckBox, ICConfigString::kCS_PANEL_Function_Extent);
    editorToConfigIDs_.insert(ui->limitFunctionBox, ICConfigString::kCS_PANEL_Register_Extent);
    ICLogInit

//            ui->turkeyBox->hide();
//            ui->koreanBox->hide();
              ui->label->hide();
    QList<QAbstractButton*> lbtns = ui->languageButtonGroup->buttons();
    for(int i = 0; i < lbtns.size(); ++i)
    {
        lbtns[i]->hide();
    }
}

ICHCSystemSettingsFrame::~ICHCSystemSettingsFrame()
{
    delete buttonGroup_;
    delete ui;
}

void ICHCSystemSettingsFrame::InitParameter()
{
    ui->dateTimeEdit->setDateTime(QDateTime::currentDateTime());
    ICParametersSave* paraSave = ICParametersSave::Instance();
//    int index = (paraSave->Country() == QLocale::China) ? 0 : 1;
    if(paraSave->Country() == QLocale::China)
        ui->chineseBox->setChecked(true);
    else if(paraSave->Country() == QLocale::UnitedStates)
        ui->englishBox->setChecked(true);
    else if(paraSave->Country() == QLocale::Spain)
        ui->spanishBox->setChecked(true);
    else if(paraSave->Country() == QLocale::Netherlands)
        ui->nederlandBox->setChecked(true);
    else if(paraSave->Country() == QLocale::France)
        ui->franceBox->setChecked(true);
    else if(paraSave->Country() == QLocale::RepublicOfKorea)
        ui->koreanBox->setChecked(true);
    else if(paraSave->Country() == QLocale::HongKong)
        ui->hkBox->setChecked(true);
    else if(paraSave->Country() == QLocale::Italy)
        ui->itBox->setChecked(true);
    else if(paraSave->Country() == QLocale::Poland)
        ui->polskiBox->setChecked(true);
    else if(paraSave->Country() == QLocale::Turkey)
        ui->turkeyBox->setChecked(true);
    else if(paraSave->Country() == QLocale::Portugal)
        ui->portugalBox->setChecked(true);
    // ui->languageComboBox->setCurrentIndex(index);
    if(paraSave->KeyTone())
    {
        ui->keyToneButton->setText(tr("Key Tone(ON)"));
        ui->keyToneButton->blockSignals(true);
        ui->keyToneButton->setChecked(true);
        ui->keyToneButton->blockSignals(false);
    }
    else
    {
        ui->keyToneButton->setText(tr("Key Tone(OFF)"));
        ui->keyToneButton->blockSignals(true);
        ui->keyToneButton->setChecked(false);
        ui->keyToneButton->blockSignals(false);
    }
}

void ICHCSystemSettingsFrame::languageBoxChange()
{
    ICParametersSave* paraSave = ICParametersSave::Instance();
    if(ui->languageButtonGroup->checkedId()== 0)
    {
        paraSave->SetCountry(QLocale::China);

    }
    else if(ui->languageButtonGroup->checkedId() == 1)
    {
        paraSave->SetCountry(QLocale::UnitedStates);
    }
    else if(ui->languageButtonGroup->checkedId() == 2)
    {
        paraSave->SetCountry(QLocale::Spain);
    }
    else if(ui->languageButtonGroup->checkedId() == 3)
    {
        paraSave->SetCountry(QLocale::Netherlands);
    }
    else if(ui->languageButtonGroup->checkedId() == 4)
    {
        paraSave->SetCountry(QLocale::France);
    }
    else if(ui->languageButtonGroup->checkedId() == 5)
    {
        paraSave->SetCountry(QLocale::RepublicOfKorea);
    }
    else if(ui->languageButtonGroup->checkedId() == 6)
    {
        paraSave->SetCountry(QLocale::HongKong);
    }
    else if(ui->languageButtonGroup->checkedId() == 7)
    {
        paraSave->SetCountry(QLocale::Italy);
    }
    else if(ui->languageButtonGroup->checkedId() == 8)
    {
        paraSave->SetCountry(QLocale::Poland);
    }
    else if(ui->languageButtonGroup->checkedId() == 9)
    {
        paraSave->SetCountry(QLocale::Turkey);
    }
    else if(ui->languageButtonGroup->checkedId() == 10)
    {
        paraSave->SetCountry(QLocale::Portugal);
    }
}


void ICHCSystemSettingsFrame::changeEvent(QEvent *e)
{
    QFrame::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
    {
        ui->retranslateUi(this);
        ICParametersSave* paraSave = ICParametersSave::Instance();

//        int index = (paraSave->Country() == QLocale::China) ? 0 : 1;
        if(paraSave->Country() == QLocale::China)
            ui->chineseBox->setChecked(true);
        else if(paraSave->Country() == QLocale::UnitedStates)
            ui->englishBox->setChecked(true);
        else if(paraSave->Country() == QLocale::Spain)
            ui->spanishBox->setChecked(true);
        else if(paraSave->Country() == QLocale::Netherlands)
            ui->nederlandBox->setChecked(true);
        else if(paraSave->Country() == QLocale::France)
            ui->franceBox->setChecked(true);
        else if(paraSave->Country() == QLocale::RepublicOfKorea)
            ui->koreanBox->setChecked(true);
        else if(paraSave->Country() == QLocale::HongKong)
            ui->hkBox->setChecked(true);
        else if(paraSave->Country() == QLocale::Italy)
            ui->itBox->setChecked(true);
        else if(paraSave->Country() == QLocale::Poland)
            ui->polskiBox->setChecked(true);
        else if(paraSave->Country() == QLocale::Turkey)
            ui->turkeyBox->setChecked(true);
        else if(paraSave->Country() == QLocale::Portugal)
            ui->portugalBox->setChecked(true);
        //    ui->languageComboBox->setCurrentIndex(index);
        ui->dateTimeEdit->setDateTime(QDateTime::currentDateTime());

//        on_keyToneButton_toggled();
        if(ui->keyToneButton->isChecked())
        {
            ui->keyToneButton->setText(tr("Key Tone(ON)"));

        }
        else
        {
            ui->keyToneButton->setText(tr("Key Tone(OFF)"));
        }
    }
        break;
    default:
        break;
    }
}

void ICHCSystemSettingsFrame::showEvent(QShowEvent *e)
{
    connect(ICVirtualHost::GlobalVirtualHost(),
            SIGNAL(StatusRefreshed()),
            this,
            SLOT(StatusRefresh()));
    ui->dateTimeEdit->setDateTime(QDateTime::currentDateTime());
    ////    if(ICProgramHeadFrame::Instance()->CurrentLevel() == ICParametersSave::AdvanceAdmin)
    //    {
    ////        ui->adminSettingsPage->setEnabled(true);
    ////        ui->adminSettingsPage->show();
    //    }
    //    else
    //    {
    ////        ui->adminSettingsPage->setEnabled(false);
    ////        ui->adminSettingsPage->hide();
    //    }
    ui->extentLabel->setEnabled(false);
    ui->extentFunctionCheckBox->setEnabled(false);
    ui->limitFunctionBox->setEnabled(false);
    ui->limitFunctionLabel->setEnabled(false);
    ui->teachMode->setEnabled(false);
    ui->simpleMode->setEnabled(false);

    //    ICVirtualHost* host = ICVirtualHost::GlobalVirtualHost();
    //    ui->hmiMachienLenghtLabel->setText(host->SystemParameter(ICVirtualHost::SYS_));
    ui->factoryCode->hide();
    ui->label_41->hide();

    int hour = ICParametersSave::Instance()->RestTime(0);
    if(hour == 0)
    {
        ui->restTime->setText(tr("No Limit"));
    }
    else
    {
        ui->restTime->setText(QString::number(hour) + tr("hour"));
    }

    QFrame::showEvent(e);
}

void ICHCSystemSettingsFrame::on_saveButton_clicked()
{
    QString dateTimeCmd = "date -s " + ui->dateTimeEdit->dateTime().toString("yyyy.MM.dd-hh:mm:ss");
    dateTimeCmd += " && hwclock -w";
    ::system(dateTimeCmd.toAscii());
    ICParametersSave::Instance()->SetBackLightTime(ui->backLightTimeEdit->TransThisTextToThisInt());
    ICMainFrame::SetScreenSaverInterval(ui->backLightTimeEdit->TransThisTextToThisInt() * 60000);
    ICAlarmFrame::Instance()->OnActionTriggered(ICConfigString::kCS_PANEL_Datetime,
                                                QString(tr("Set to %1")).arg(ui->dateTimeEdit->dateTime().toString("yyyy/MM/dd hh:mm:ss")),
                                                "");
    ICAlarmFrame::Instance()->OnActionTriggered(ICConfigString::kCS_PANEL_Config_Save,
                                                tr("Save"),
                                                "");
}

void ICHCSystemSettingsFrame::on_keyToneButton_toggled(bool checked)
{
    if(checked)
    {
        ui->keyToneButton->setText(tr("Key Tone(ON)"));
        ICAlarmFrame::Instance()->OnActionTriggered(ICConfigString::kCS_PANEL_Key_Tone,
                                                    tr("ON"),
                                                    "");
    }
    else
    {
        ui->keyToneButton->setText(tr("Key Tone(OFF)"));
        ICAlarmFrame::Instance()->OnActionTriggered(ICConfigString::kCS_PANEL_Key_Tone,
                                                    tr("OFF"),
                                                    "");
    }
    ICParametersSave::Instance()->SetKeyTone(checked);
}

void ICHCSystemSettingsFrame::on_changeButton_clicked()
{
    if(/*ui->oldPwdEdit->text().isEmpty() ||*/
            ui->newPwdEdit->text().isEmpty())
    {
        ICMessageBox::ICWarning(this,
                                 tr("Information"),
                                 tr("Please enter the old password and new password!"));
        return;
    }
    ICParametersSave* config = ICParametersSave::Instance();
    ICParametersSave::OperationLevel level;
    if(ui->machineAdminBox->isChecked())
    {
        level = ICParametersSave::MachineAdmin;
    }
    else if(ui->advanceAdminBox->isChecked())
    {
        level = ICParametersSave::AdvanceAdmin;
    }
    int currentlevel = ICProgramHeadFrame::Instance()->CurrentLevel();

    if(currentlevel <= level && !config->VerifyPassword(level, ui->oldPwdEdit->text()))
    {
        ICMessageBox::ICWarning(this,
                             tr("Warning"),
                             tr("Old password is wrong!"));
        return;
    }
    ICParametersSave::Instance()->SetPassword(level, ui->newPwdEdit->text());
    ui->oldPwdEdit->clear();
    ui->newPwdEdit->clear();
    ICMessageBox::ICWarning(this,
                             tr("Information"),
                             tr("Change password successfully!"));
    ICAlarmFrame::Instance()->OnActionTriggered(ui->advanceAdminBox->isChecked() ? ICConfigString::kCS_PANEL_Root_Password : ICConfigString::kCS_PANEL_Admin_Password,
                                                QString("Changed"),
                                                "");
}

void ICHCSystemSettingsFrame::on_extentFunctionCheckBox_toggled(bool checked)
{
    //    if(passwordDialog_->exec() != QDialog::Accepted)
    //    {
    //        ui->extentFunctionCheckBox->setChecked(!checked);
    //        return;
    //    }
    ICParametersSave::Instance()->SetExtentFunctionUsed(checked);
}

void ICHCSystemSettingsFrame::on_verifyButton_clicked()
{
    if(ui->pwdEdit->text() == "szhc2a")
    {
        ui->extentLabel->setEnabled(true);
        ui->extentFunctionCheckBox->setEnabled(true);
        ui->limitFunctionLabel->setEnabled(true);
        ui->limitFunctionBox->setEnabled(true);
        ui->teachMode->setEnabled(true);
        ui->simpleMode->setEnabled(true);
    }
    ui->pwdEdit->clear();
}

bool ICHCSystemSettingsFrame::CheckIsUsbAttached() const
{
    QDir dir("/proc/scsi/usb-storage");
    if(!dir.exists())
    {
        return false;
    }
    if(dir.entryList(QStringList(), QDir::Files).isEmpty())
    {
        return false;
    }
    QFile file("/proc/mounts");
    if(!file.open(QFile::ReadOnly))
    {
        return false;
    }
    QString content = file.readAll();
    file.close();
    return content.contains(QRegExp("/dev/sd[a-z]*[1-9]*"));

}

void ICHCSystemSettingsFrame::on_backupMachineButton_clicked()
{
    if(!CheckIsUsbAttached())
    {
        ICMessageBox::ICWarning(this, tr("Warning"), tr("USB is not connected!"));
        return;
    }
    ICTipsWidget tipsWidget(tr("Backuping, please wait..."));
    tipsWidget.show();qApp->processEvents();
    ICBackupUtility backupUtility;
    Information(backupUtility.BackupDir("./sysconfig",
                                        "/mnt/udisk/HC5ABackup/sysconfig",
                                        QStringList()<<"param*.txt"<<"DistanceRotation"));
    //    system("rm /mnt/udisk/HC5ABackup/sysconfig/param*.txt");
    //    system("rm /mnt/udisk/HC5ABackup/sysconfig/DistanceRotation");
    //    system("mkdir -p /mnt/udisk/HC5ABackup/sysconfig");
    //    //    bool isSuccess = QFile::copy("./sysconfig/paramx.txt", "/mnt/udisk/HC5ABackup/sysconfig/paramx.txt");
    //    //    isSuccess = isSuccess && QFile::copy("./sysconfig/paramy.txt", "/mnt/udisk/HC5ABackup/sysconfig/paramy.txt");
    //    //    isSuccess = isSuccess && QFile::copy("./sysconfig/paramz.txt", "/mnt/udisk/HC5ABackup/sysconfig/paramz.txt");
    //    Information(system("cp /opt/Qt/bin/sysconfig/param*.txt /opt/Qt/bin/sysconfig/DistanceRotation /mnt/udisk/HC5ABackup/sysconfig/ -f") >= 0 );
}

void ICHCSystemSettingsFrame::on_backupSystemButton_clicked()
{
    if(!CheckIsUsbAttached())
    {
        ICMessageBox::ICWarning(this, tr("Warning"), tr("USB is not connected!"));
        return;
    }
    ICTipsWidget tipsWidget(tr("Backuping, please wait..."));
    tipsWidget.show();qApp->processEvents();
    ICBackupUtility backupUtility;
    Information(backupUtility.BackupDir("./sysconfig",
                                        "/mnt/udisk/HC5ABackup/sysconfig",
                                        QStringList()<<"system.txt"));
    //    system("rm /mnt/udisk/HC5ABackup/sysconfig/system.txt");
    //    system("mkdir -p /mnt/udisk/HC5ABackup/sysconfig");
    //    //    bool isSuccess = QFile::copy("./sysconfig/system.txt", "/mnt/udisk/HC5ABackup/sysconfig/system.txt");
    //    Information(system("cp /opt/Qt/bin/sysconfig/system.txt /mnt/udisk/HC5ABackup/sysconfig/system.txt -f") >= 0);
}

void ICHCSystemSettingsFrame::Information(bool isSuccess, const QString &msg)
{
    if(isSuccess)
    {
        ICMessageBox::ICWarning(this, tr("Information"), tr("Operation finished!"));
    }
    else
    {
        ICMessageBox::ICWarning(this, tr("Information"), tr("Operation fail!") + msg);
    }
}

//void ICHCSystemSettingsFrame::on_backupMoldsButton_clicked()
//{
//    if(!CheckIsUsbAttached())
//    {
//        ICMessageBox::ICWarning(this, tr("Warning"), tr("USB is not connected!"));
//        return;
//    }
//    ICTipsWidget tipsWidget(tr("Backuping, please wait..."));
//    tipsWidget.show();
//    qApp->processEvents();
//    ICBackupUtility backupUtility;
//    bool ret = backupUtility.BackupDir("/opt/Qt/bin/records",
//                                       "/mnt/udisk/HC5ABackup/records",
//                                       QStringList()<<"*.act"<<"*.fnc");
//    ret = ret && backupUtility.BackupDir("/opt/Qt/bin/subs",
//                                         "/mnt/udisk/HC5ABackup/subs",
//                                         QStringList()<<"sub[0-7].prg");
//    Information(ret);
//    //    system("rm -R /mnt/udisk/HC5ABackup/records");
//    //    system("rm -R /mnt/udisk/HC5ABackup/subs");
//    //    system("mkdir -p /mnt/udisk/HC5ABackup/");
//    //    //    bool isSuccess = QFile::copy("./records", "/mnt/udisk/HC5ABackup/");
//    //    //    isSuccess = isSuccess && QFile::copy("./subs", "/mnt/udisk/HC5ABackup/");
//    //    Information(system("cp -Rf /opt/Qt/bin/records/ /opt/Qt/bin/subs/ /mnt/udisk/HC5ABackup") >= 0);
//}

void ICHCSystemSettingsFrame::on_backupAllButton_clicked()
{
#if defined(Q_WS_WIN32) || defined(Q_WS_X11)
    QString getFileDir = QFileDialog::getExistingDirectory();
    //    QDir dir(getFileDir + "/HC5ABackup/records");
    //    QString path = getFileDir;
#else
    if(!CheckIsUsbAttached())
    {
        ICMessageBox::ICWarning(this, tr("Warning"), tr("USB is not connected!"));
        return;
    }
    //    QDir dir("/mnt/udisk/HC5ABackup/records");
    QString getFileDir = "/mnt/udisk";
#endif
    ICTipsWidget tipsWidget(tr("Backuping, please wait..."));
    tipsWidget.show();qApp->processEvents();
    ICBackupUtility backupUtility;
    bool ret = backupUtility.BackupDir("./sysconfig",
                                       getFileDir + "/HC5ABackup/sysconfig",
                                       QStringList()<<"param*.txt"<<"DistanceRotation");
    ret = ret && backupUtility.BackupDir("./sysconfig",
                                         getFileDir + "/HC5ABackup/sysconfig",
                                         QStringList()<<"system.txt");

    ret = ret && backupUtility.BackupDir("./records/",
                                         getFileDir + "/HC5ABackup/records/",
                                         QStringList()<<"*.act"<<"*.fnc<<*.st");

    ret = ret && backupUtility.BackupDir("./subs",
                                         getFileDir + "/HC5ABackup/subs",
                                         QStringList()<<"sub[0-7].prg");
    Information(ret);

    //    system("rm -R /mnt/udisk/HC5ABackup");
    //    system("mkdir -p /mnt/udisk/HC5ABackup/sysconfig");
    //    //    bool isSuccess = QFile::copy("./sysconfig/paramx.txt", "/mnt/udisk/HC5ABackup/sysconfig/paramx.txt");
    //    //    isSuccess = isSuccess && QFile::copy("./sysconfig/paramy.txt", "/mnt/udisk/HC5ABackup/sysconfig/paramy.txt");
    //    //    isSuccess = isSuccess && QFile::copy("./sysconfig/paramz.txt", "/mnt/udisk/HC5ABackup/sysconfig/paramz.txt");

    //    //    isSuccess = isSuccess && QFile::copy("./sysconfig/system.txt", "/mnt/udisk/HC5ABackup/sysconfig/system.txt");

    //    //    isSuccess = isSuccess && QFile::copy("./records", "/mnt/udisk/HC5ABackup/");
    //    //    isSuccess = isSuccess && QFile::copy("./subs", "/mnt/udisk/HC5ABackup/");

    //    Information(system("cp /opt/Qt/bin/sysconfig/param*.txt /opt/Qt/bin/sysconfig/system.txt /opt/Qt/bin/sysconfig/DistanceRotation /mnt/udisk/HC5ABackup/sysconfig/ -f && cp -Rf /opt/Qt/bin/records/ /opt/Qt/bin/subs/ /mnt/udisk/HC5ABackup") >= 0);
}

void ICHCSystemSettingsFrame::on_restoreMachineButton_clicked()
{
    if(!CheckIsUsbAttached())
    {
        ICMessageBox::ICWarning(this, tr("Warning"), tr("USB is not connected!"));
        return;
    }
    ICTipsWidget tipsWidget(tr("Restoring, please wait..."));
    tipsWidget.show();qApp->processEvents();

    if(CheckRestoreMachineFiles_())
    {

        ICBackupUtility backupUtility;
        bool ret = (backupUtility.RestoreDir("/mnt/udisk/HC5ABackup/sysconfig",
                                             "./sysconfig",
                                             QStringList()<<"param*.txt"<<"DistanceRotation"));
        Information(ret, tr("Backup files is broken!"));
        //    QDir dir("/mnt/udisk/HC5ABackup/sysconfig");
        //    if(!dir.exists())
        //    {
        //        ICMessageBox::ICWarning(this, tr("Warnning"), tr("Backup files is not exists!"));
        //        return;
        //    }
        //    system("cp /mnt/udisk/HC5ABackup/sysconfig/param* /opt/Qt/bin/sysconfig -f");
        //    system("cp /mnt/udisk/HC5ABackup/sysconfig/DistanceRotation /opt/Qt/bin/sysconfig -f");
        //    Information(true);
        if(ret)
        {
            system("reboot");
        }
    }
}

void ICHCSystemSettingsFrame::on_restoreSystemButton_clicked()
{
    if(!CheckIsUsbAttached())
    {
        ICMessageBox::ICWarning(this, tr("Warning"), tr("USB is not connected!"));
        return;
    }
    ICTipsWidget tipsWidget(tr("Restoring, please wait..."));
    tipsWidget.show();qApp->processEvents();
    if(CheckRestoreSystemFiles_())
    {
        ICBackupUtility backupUtility;
        bool ret = (backupUtility.RestoreDir("/mnt/udisk/HC5ABackup/sysconfig",
                                             "./sysconfig",
                                             QStringList()<<"system.txt"));
        Information(ret, tr("Backup files is broken!"));
        //    QDir dir("/mnt/udisk/HC5ABackup/sysconfig");
        //    if(!dir.exists())
        //    {
        //        ICMessageBox::ICWarning(this, tr("Warnning"), tr("Backup files is not exists!"));
        //        return;
        //    }
        //    system("cp /mnt/udisk/HC5ABackup/sysconfig/system.txt /opt/Qt/bin/sysconfig -f");
        //    Information(true);
        if(ret)
        {
            system("reboot");
        }
    }
}

//void ICHCSystemSettingsFrame::on_restoreMoldsButton_clicked()
//{
//    if(!CheckIsUsbAttached())
//    {
//        ICMessageBox::ICWarning(this, tr("Warning"), tr("USB is not connected!"));
//        return;
//    }
//    ICTipsWidget tipsWidget(tr("Restoring, please wait..."));
//    tipsWidget.show();qApp->processEvents();
//    QDir dir("/mnt/udisk/HC5ABackup/records");
//    if(!dir.exists())
//    {
//        ICMessageBox::ICWarning(this, tr("Warnning"), tr("Backup files is not exists!"));
//        return;
//    }
//    QStringList acts = dir.entryList(QStringList()<<"*.act");
//    QStringList fncs = dir.entryList(QStringList()<<"*.fnc");
//    if(fncs.contains("Base.fnc"))
//    {
//        fncs.removeOne("Base.fnc");
//    }
//    if(acts.size() != fncs.size())
//    {
//        ICMessageBox::ICWarning(this, tr("Warnning"), tr("Backup files is incomplete!"));
//        return;
//    }
//    for(int i = 0; i != fncs.size(); ++i)
//    {
//        fncs[i] = fncs.at(i).left(fncs.at(i).size() - 4);
//    }
//    for(int i = 0; i != acts.size(); ++i)
//    {
//        if(!fncs.contains(acts.at(i).left(acts.at(i).size() - 4)))
//        {
//            ICMessageBox::ICWarning(this, tr("Warnning"), tr("Backup files is incomplete!"));
//            return;
//        }
//    }
//    QFile file;
//    QString actContent;
//    ICProgramFormatChecker programChecker;
//    for(int i = 0; i != acts.size(); ++i)
//    {
//        file.setFileName(dir.absoluteFilePath(acts.at(i)));
//        actContent.clear();
//        file.open(QFile::ReadOnly | QFile::Text);
//        actContent = file.readAll();
//        file.close();
//        if(!programChecker.Check(actContent))
//        {
//            ICMessageBox::ICWarning(this, tr("Warnning"), tr("Wrong program format!"));
//            return;
//        }
//    }
//    ICConfigFormatChecker configFormatChecker;
//    for(int i = 0; i != fncs.size(); ++i)
//    {
//        file.setFileName(dir.absoluteFilePath(fncs.at(i) + ".fnc"));
//        actContent.clear();
//        file.open(QFile::ReadOnly | QFile::Text);
//        actContent = file.readAll();
//        file.close();
//        if(!configFormatChecker.CheckRowCount(actContent, 58,ICDataFormatChecker::kCompareEqual))
//        {
//            ICMessageBox::ICWarning(this, tr("Warnning"), tr("Wrong config format!"));
//            return;
//        }
//        if(!configFormatChecker.Check(actContent))
//        {
//            ICMessageBox::ICWarning(this, tr("Warnning"), tr("Wrong config format!"));
//            return;
//        }
//    }
//    ICBackupUtility backupUtility;
//    bool ret = backupUtility.RestoreDir("/mnt/udisk/HC5ABackup/records",
//                                        "/opt/Qt/bin/records",
//                                        QStringList()<<"*.act"<<"*.fnc");
//    dir.cdUp();
//    dir.cd("subs");
//    QStringList subs = dir.entryList(QStringList()<<"sub[0-7]");
//    for(int i = 0; i != subs.size(); ++i)
//    {
//        file.setFileName(dir.absoluteFilePath(subs.at(i)));
//        actContent.clear();
//        file.open(QFile::ReadOnly | QFile::Text);
//        actContent = file.readAll();
//        file.close();
//        if(!programChecker.Check(actContent))
//        {
//            ICMessageBox::ICWarning(this, tr("Warnning"), tr("Wrong program format!"));
//            return;
//        }
//    }
//    if(ret)
//    {
//        ret = ret && backupUtility.RestoreDir("/mnt/udisk/HC5ABackup/subs",
//                                              "/opt/Qt/bin/subs",
//                                              QStringList()<<"sub[0-7].prg");
//    }
//    Information(ret, tr("Backup files is broken!"));
//    //    system("cp /mnt/udisk/HC5ABackup/records/*.act /mnt/udisk/HC5ABackup/records/*.fnc /opt/Qt/bin/records -f");
//    //    dir.setPath("/mnt/udisk/HC5ABackup/subs");
//    //    if(!dir.exists())
//    //    {
//    //        ICMessageBox::ICWarning(this, tr("Warnning"), tr("Backup files is not exists!"));
//    //        return;
//    //    }
//    //    system("cp /mnt/udisk/HC5ABackup/subs/* /opt/Qt/bin/subs -f");
//    //    Information(true);
//    if(ret)
//    {
//        system("reboot");
//    }
//}

void ICHCSystemSettingsFrame::on_restoreAllButton_clicked()
{
#if defined(Q_WS_WIN32) || defined(Q_WS_X11)
    QString getFileDir = QFileDialog::getExistingDirectory();
#else
    if(!CheckIsUsbAttached())
    {
        ICMessageBox::ICWarning(this, tr("Warning"), tr("USB is not connected!"));
        return;
    }
    QString getFileDir = "/mnt/udisk";
#endif
    ICTipsWidget tipsWidget(tr("Restoring, please wait..."));
    tipsWidget.show();qApp->processEvents();
    bool ret = CheckRestoreSystemFiles_();
    ret = ret && CheckRestoreMachineFiles_();
    QDir dir(getFileDir + "/HC5ABackup/records");
    if(!dir.exists())
    {
        ret = false;
        return;
    }
    QStringList acts = dir.entryList(QStringList()<<"*.act");
    QStringList fncs = dir.entryList(QStringList()<<"*.fnc");
    QStringList workReocrds;
    QString temp;
    int count = qMin(acts.size(), fncs.size());
    for(int i = 0; i != count; ++i)
    {
        temp = acts.at(i);
        temp.chop(4);
        if(fncs.contains(temp + ".fnc"))
            workReocrds.append(temp);
    }

    QStringList skipRecords;
    QFile file;
    QString actContent;
    ICProgramFormatChecker programChecker;
    ICConfigFormatChecker configFormatChecker;

    for(int i = 0; i != workReocrds.size(); ++i)
    {
        file.setFileName(dir.absoluteFilePath(workReocrds.at(i) + ".act"));
        actContent.clear();
        file.open(QFile::ReadOnly | QFile::Text);
        actContent = file.readAll();
        file.close();
        if(!programChecker.Check(actContent))
        {
            ICMessageBox::ICWarning(this, tr("Warnning"), QString(tr("%1 wrong program format! Will skip this record!").arg(workReocrds.at(i))));
            //            ret = false;
            skipRecords.append(workReocrds.at(i));
            continue;
        }

        file.setFileName(dir.absoluteFilePath(workReocrds.at(i) + ".fnc"));
        actContent.clear();
        file.open(QFile::ReadOnly | QFile::Text);
        actContent = file.readAll();
        file.close();
        if(!configFormatChecker.CheckRowCount(actContent, 58,ICDataFormatChecker::kCompareEqual))
        {
            ICMessageBox::ICWarning(this, tr("Warnning"), QString(tr("%1 wrong config format! Will skip this record!").arg(workReocrds.at(i))));
            //            ret = false;
            skipRecords.append(workReocrds.at(i));
            continue;
        }
        if(!configFormatChecker.Check(actContent))
        {
            ICMessageBox::ICWarning(this, tr("Warnning"), QString(tr("%1 wrong config format! Will skip this record!").arg(workReocrds.at(i))));
            //            ret = false;
            skipRecords.append(workReocrds.at(i));
        }
    }
    for(int i = 0; i != skipRecords.size(); ++i)
    {
        workReocrds.removeOne(skipRecords.at(i));
    }
    dir.cdUp();
    dir.cd("subs");
    skipRecords.clear();
    QStringList subs = dir.entryList(QStringList()<<"sub[0-7]");
    for(int i = 0; i != subs.size(); ++i)
    {
        file.setFileName(dir.absoluteFilePath(subs.at(i)));
        actContent.clear();
        file.open(QFile::ReadOnly | QFile::Text);
        actContent = file.readAll();
        file.close();
        if(!programChecker.Check(actContent))
        {
            ICMessageBox::ICWarning(this, tr("Warnning"), QString(tr("%1 wrong program format! Will skip this sub!").arg(subs.at(i))));
            skipRecords.append(subs.at(i));
            //            ret = false;
            //            return;
        }
    }
    for(int i = 0; i != skipRecords.size(); ++i)
    {
        subs.removeOne(skipRecords.at(i));
    }

    for(int i = 0; i != workReocrds.size(); ++i)
    {
        workReocrds[i] += ".*";
    }

    ICBackupUtility backupUtility;
    ret = backupUtility.RestoreDir(getFileDir + "/HC5ABackup/sysconfig",
                                   "./sysconfig",
                                   QStringList()<<"param*.txt"<<"DistanceRotation");
    ret = ret && backupUtility.RestoreDir(getFileDir + "/HC5ABackup/sysconfig",
                                          "./sysconfig",
                                          QStringList()<<"system.txt");

    ret = ret && backupUtility.RestoreDir(getFileDir +  "/HC5ABackup/records",
                                          "./records",
                                          workReocrds);

    ret = ret && backupUtility.RestoreDir(getFileDir + "/HC5ABackup/subs",
                                          "./subs",
                                          subs);

    Information(ret);
    if(ret)
    {
        system("reboot");
    }


}

void ICHCSystemSettingsFrame::on_umountButton_clicked()
{
    system("umount /mnt/udisk");
}

void ICHCSystemSettingsFrame::OnLevelChanged(int level)
{
    if(level == ICParametersSave::AdvanceAdmin)
    {
        for(int i = 1; i != ui->systemConfigPages->count(); ++i)
        {
            ui->systemConfigPages->setTabEnabled(i, true);
        }
    }
    else
    {
        for(int i = 1; i != ui->systemConfigPages->count(); ++i)
        {
            ui->systemConfigPages->setTabEnabled(i, false);
        }
    }
}


void ICHCSystemSettingsFrame::StatusRefresh()
{
#ifdef Q_WS_WIN32
    QString os("1.0");
#else
    QString os(osInfo_.release);
#endif
    os += "; ";
    ui->versionLabel->setText("Version: OS:" + os + QString(tr("\nApp %1; Host:")).arg(QString::fromUtf8(SHOW_VERSION)) + ICVirtualHost::GlobalVirtualHost()->HostStatus(ICVirtualHost::Time).toString());
}

void ICHCSystemSettingsFrame::on_calibrationBtn_clicked()
{
    if(ICMessageBox::ICWarning(this,
                            tr("Warning"),
                            tr("The system will be reboot to calibrate! Do you want to continue?"),
                            QMessageBox::Ok | QMessageBox::Cancel) == QMessageBox::Ok)
    {
        ICAlarmFrame::Instance()->OnActionTriggered(ICConfigString::kCS_PANEL_Touch_Recal,
                                                    tr("Recal"),
                                                    "");
        ::system("cd /home/szhc && echo recal >>recal && sync && reboot");
    }

}

void ICHCSystemSettingsFrame::on_brightMinus_clicked()
{
    uint brightness = ui->brightnessBar->value();
    if(brightness == 1)
    {
        return;
    }
    ui->brightnessBar->setValue((--brightness));
    ICParametersSave::Instance()->SetBrightness(brightness);
    ICAlarmFrame::Instance()->OnActionTriggered(ICConfigString::kCS_PANEL_Bright,
                                                QString(tr("Tune down to %1")).arg(brightness),
                                                "");
}

void ICHCSystemSettingsFrame::on_brightPlus_clicked()
{
    uint brightness = ui->brightnessBar->value();
    if(brightness == (uint)ui->brightnessBar->maximum())
    {
        return;
    }
    ui->brightnessBar->setValue((++brightness));
    ICParametersSave::Instance()->SetBrightness(brightness);
    ICAlarmFrame::Instance()->OnActionTriggered(ICConfigString::kCS_PANEL_Bright,
                                                QString(tr("Tune up to %1")).arg(brightness),
                                                "");
}

bool ICHCSystemSettingsFrame::CheckRestoreSystemFiles_()
{

    ICConfigFormatChecker configChecker;
    QFile file("/mnt/udisk/HC5ABackup/sysconfig/system.txt");
    if(file.open(QFile::ReadOnly | QFile::Text))
    {
        QString sysContent = file.readAll();
        file.close();
        if(!configChecker.CheckRowCount(sysContent, 162, ICDataFormatChecker::kCompareEqual))
        {
            ICMessageBox::ICWarning(this, tr("Warnning"), tr("Wrong system config format!"));
            return false;
        }
        if(!configChecker.Check(sysContent))
        {
            ICMessageBox::ICWarning(this, tr("Warnning"), tr("Wrong system config format!"));
            return false;
        }
    }
    return true;
}

bool ICHCSystemSettingsFrame::CheckRestoreMachineFiles_()
{
    QFile file;
    QString configContent;
    QDir dir("/mnt/udisk/HC5ABackup/sysconfig");
    QStringList configs = dir.entryList( QStringList()<<"param*.txt");
    ICConfigFormatChecker configChecker;
    for(int i = 0; i != configs.size(); ++i)
    {
        file.setFileName(dir.absoluteFilePath(configs.at(i)));
        file.open(QFile::ReadOnly | QFile::Text);
        configContent = file.readAll();
        file.close();
        if(!configChecker.CheckRowCount(configContent, 7, ICDataFormatChecker::kCompareEqual))
        {
            ICMessageBox::ICWarning(this, tr("Warnning"), tr("Wrong Axis config format!"));
            return false;
        }
        if(!configChecker.Check(configContent))
        {
            ICMessageBox::ICWarning(this, tr("Warnning"), tr("Wrong Axis config format!"));
            return false;
        }
    }
    file.setFileName(dir.absoluteFilePath("DistanceRotation"));
    if(file.open(QFile::ReadOnly | QFile::Text))
    {
        configContent = file.readAll();
        file.close();
        QStringList rows = configContent.split("\n", QString::SkipEmptyParts);
        if(rows.size() != 8)
        {
            ICMessageBox::ICWarning(this, tr("Warnning"), tr("Wrong Rotation config format!"));
            return false;
        }
        QStringList cols;
        QStringList axisName;
        axisName<<"X"<<"Y"<<"Z"<<"P"<<"Q"<<"A"<<"B"<<"C";
        for(int i = 0; i != rows.size(); ++i)
        {
            cols = rows.at(i).split(" ", QString::SkipEmptyParts);
            if(cols.size() != 2)
            {
                ICMessageBox::ICWarning(this, tr("Warnning"), tr("Wrong Rotation config format!"));
                return false;
            }
            //            if(cols.at(0) != axisName.at(i))
            //            {
            //                ICMessageBox::ICWarning(this, tr("Warnning"), tr("Wrong Rotation config format!"));
            //                return false;
            //            }
//            for(int j = 0; j != cols.at(1).size(); ++j)
//            {
//                if(!cols.at(1).at(j).isDigit())
//                {
//                    ICMessageBox::ICWarning(this, tr("Warnning"), tr("Wrong Rotation config format!"));
//                    return false;
//                }
//            }

        }
    }
    return true;

}

void ICHCSystemSettingsFrame::on_clearButton_clicked()
{
    ui->newPwdEdit->clear();
    ui->oldPwdEdit->clear();
}

void ICHCSystemSettingsFrame::on_limitFunctionBox_toggled(bool checked)
{
    ICParametersSave::Instance()->SetRegisterFunction(checked);
}

void ICHCSystemSettingsFrame::on_generateBtn_clicked()
{
    QString ret;
    qsrand(QDateTime::currentDateTime().toMSecsSinceEpoch());
    for(int i = 0; i != 6; ++i)
    {
        ret.append(QString::number(qrand() % 10));
    }
    ui->machineCode->setText(ret);
}

void ICHCSystemSettingsFrame::on_registerBtn_clicked()
{
    if(ui->machineCode->text().isNull())
    {
        ui->tipLabel->setText(tr("Wrong Register Code!"));
        return;
    }
    int hour = ICUtility::Register(ICParametersSave::Instance()->FacotryCode(), ui->machineCode->text(), ui->registerCode->text());
    if(hour == -1)
    {
        ui->tipLabel->setText(tr("Wrong Register Code!"));
    }
    else
    {
        //        ICAppSettings().SetUsedTime(hour);
        ICParametersSave::Instance()->SetRestTime(hour);
        ui->tipLabel->setText(tr("Register Success!"));
        if(hour == 0)
        {
            ui->restTime->setText(tr("No Limit"));
        }
        else
        {
            ui->restTime->setText(QString::number(hour) + tr("hour"));
        }
        //        emit RegisterSucceed();
        ui->machineCode->clear();
        ui->registerCode->clear();

        //        ICDALHelper::UpdateConfigValue(ICAddr_System_OtherUsedTime, hour);
    }
    ICProgramHeadFrame::Instance()->ReashRestTime();
}

void ICHCSystemSettingsFrame::on_verifySupperButton_clicked()
{
    if(ui->supperPwdEdit->text() == ICParametersSave::Instance()->SuperPassward()
            || ui->supperPwdEdit->text() == "szhcrobot")
    {
        ui->label_41->show();
        ui->factoryCode->show();
    }
    ui->supperPwdEdit->clear();
}

void ICHCSystemSettingsFrame::on_factoryCode_textChanged(const QString &arg1)
{
    ICParametersSave::Instance()->SetFacotryCode(arg1);
}

ICLogFunctions(ICHCSystemSettingsFrame)

void ICHCSystemSettingsFrame::on_memory_fix_clicked()
{
    if(ICMessageBox::ICWarning(this,
                            tr("Warning"),
                            tr("The system will be for storage to repair ! Do you want to continue?"),
                            QMessageBox::Ok | QMessageBox::Cancel) == QMessageBox::Ok)
    {
        ::system("e2fsck -p /dev/mmcblkop2");
        ::system("root");
    }
}

void ICHCSystemSettingsFrame::on_machineAdminBox_toggled(bool checked)
{
    if(ui->machineAdminBox->isChecked())
    {
        ui->oldPwdEdit->setEnabled(false);
    }
    else
    {
        ui->oldPwdEdit->setEnabled(true);
    }
}

void ICHCSystemSettingsFrame::OnProgramModeChanged(int id)
{
    ICParametersSave::Instance()->SetProgramMode(id);
}

void ICHCSystemSettingsFrame::on_scanGhost_clicked()
{
#if defined(Q_WS_WIN32) || defined(Q_WS_X11)
#else
    if(!CheckIsUsbAttached())
    {
        ICMessageBox::ICWarning(this, tr("Warning"), tr("USB is not connected!"));
        return;
    }
#endif
    ICBackupUtility backupUtility;
    ghostFileListModel_.setStringList(backupUtility.ScanGhost());
    ui->listView->setModel(&ghostFileListModel_);
}

void ICHCSystemSettingsFrame::on_newGhost_clicked()
{
#if defined(Q_WS_WIN32) || defined(Q_WS_X11)
//    QString getFileDir = QFileDialog::getExistingDirectory();
#else
    if(!CheckIsUsbAttached())
    {
        ICMessageBox::ICWarning(this, tr("Warning"), tr("USB is not connected!"));
        return;
    }
//    QString getFileDir = "/mnt/udisk";
#endif
    ICBackupUtility backupUtility;
    if(ui->lineEdit->text().isEmpty()) return;
    ICTipsWidget tipsWidget(tr("Ghosting, please wait..."));
    tipsWidget.show();qApp->processEvents();
    backupUtility.MakeGhost(ui->lineEdit->text());
    Information(true);
}

void ICHCSystemSettingsFrame::on_loadGhost_clicked()
{
#if defined(Q_WS_WIN32) || defined(Q_WS_X11)
//    QString getFileDir = QFileDialog::getExistingDirectory();
#else
    if(!CheckIsUsbAttached())
    {
        ICMessageBox::ICWarning(this, tr("Warning"), tr("USB is not connected!"));
        return;
    }
//    QString getFileDir = "/mnt/udisk";
#endif
    QString ghostFileName;
    QModelIndex index = ui->listView->selectionModel()->currentIndex();
    ghostFileName = ghostFileListModel_.data(index,Qt::DisplayRole).toString();
    ICTipsWidget tipsWidget(tr("Loading, please wait..."));
    tipsWidget.show();qApp->processEvents();
    ICBackupUtility backupUtility;
    backupUtility.LoadGhost(ghostFileName);
    Information(true);
    ::system("reboot");
}
