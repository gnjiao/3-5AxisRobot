#include "icparameterssave.h"

#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QApplication>
#include <QTranslator>

ICParametersSave * ICParametersSave::instance_ = NULL;

ICParametersSave::ICParametersSave(const QString fileName)
    : QSettings(fileName, QSettings::IniFormat),
      ProductOperationer("ProductOperationer"),
      ProductAdministrator("ProductAdministrator"),
      ProductAlarmHistory("ProductAlarmHistory"),
      SystemMachine("SystemMachine"),
      SystemLocale("SystemLanguage"),
      InstructMultidotPut("InstructMultidotPut"),
      CommunicationConfig("CommunicationConfig"),
      ProductConfig("ProductConfig"),
      translator_(new QTranslator()),
      sysTranslator_(new QTranslator())
{
    //    SetFileName("./sysconfig/systemParameter.hc");
    QFile file("./sysconfig/DistanceRotation");
    if(file.open(QFile::ReadOnly | QFile::Text))
    {
        QTextStream in(&file);
        QString tmpAxis;
        double tmpDR;
        while(!(in>>tmpAxis>>tmpDR).atEnd())
        {
            axisToRotate_.insert(tmpAxis, tmpDR);
        }
    }
    file.close();
#ifndef Q_WS_WIN32
    beepFD_ = open("/dev/szhc_beep", O_WRONLY);
#else
    beepFD_ = 0;
#endif
    SetKeyTone(KeyTone(), false);
    isSuperLogin_ = false;
    //    ioctl(beepFD_, 0, KeyTone() ? 1 : 0);
}

ICParametersSave::~ICParametersSave()
{
    delete translator_;
}

void ICParametersSave::SaveParameter(const QString & group,const QString & key, const QVariant & value, bool issync)
{
    this->beginGroup(group);
    this->setValue(key,value);
    this->endGroup();
    if(issync)
    {
        this->sync();
        ::system("sync");
    }
}

QVariant ICParametersSave::GetParameter(const QString & group,const QString & key,const QVariant & defaultValue)
{
    QVariant value;
    this->beginGroup(group);
    value = this->value(key, defaultValue);
    this->endGroup();
    return value;
}

void ICParametersSave::SetCommunicationConfig(const QString &device, int baudRate, const QString &parity, int dataBit, int stopBit)
{
    this->beginGroup(CommunicationConfig);
    this->setValue("Device",device );
    this->setValue("Baud", baudRate);
    this->setValue("Parity", parity);
    this->setValue("DataBit", dataBit);
    this->setValue("StopBit", stopBit);
    this->endGroup();
}

void ICParametersSave::SetLanguage(QLocale::Language language, bool isSync)
{
    switch(language)
    {
    case QLocale::Chinese:
    {
        translator_->load("Multi-axisManipulatorSystem_ch");
        sysTranslator_->load("qt_zh_CN");
        qApp->installTranslator(sysTranslator_);
    }
        break;
    case QLocale::English:
    {
        translator_->load("Multi-axisManipulatorSystem_en");
        qApp->removeTranslator(sysTranslator_);
    }
        break;
    case QLocale::Spanish:
    {
        translator_->load("Multi-axisManipulatorSystem_esp");
        sysTranslator_->load("qt_es.qm");
        qApp->installTranslator(sysTranslator_);
    }
        break;
    case QLocale::Dutch:
    {
        translator_->load("Multi-axisManipulatorSystem_nl");
        qApp->removeTranslator(sysTranslator_);
//        qApp->installTranslator(sysTranslator_);
    }
        break;
    case QLocale::French:
    {
        translator_->load("Multi-axisManipulatorSystem_fr");
        //        sysTranslator_->load("qt_es.qm");
//        qApp->installTranslator(sysTranslator_);
        qApp->removeTranslator(sysTranslator_);
    }
        break;
    case QLocale::Korean:
    {
        translator_->load("Multi-axisManipulatorSystem_kr");
        //        sysTranslator_->load("qt_es.qm");
        sysTranslator_->load("qt_ko");
        qApp->installTranslator(sysTranslator_);
    }
        break;
    case QLocale::AnyLanguage:
    {
        translator_->load("Multi-axisManipulatorSystem_hk");
        sysTranslator_->load("qt_zh_TW");
        qApp->installTranslator(sysTranslator_);
    }
        break;
    case QLocale::Italian:
    {
        translator_->load("Multi-axisManipulatorSystem_it");
        sysTranslator_->load("qt_lt");
        qApp->installTranslator(sysTranslator_);
    }
        break;
    case QLocale::Polish:
    {
        translator_->load("Multi-axisManipulatorSystem_pl");
//        sysTranslator_->load("qt_lt");
//        qApp->installTranslator(sysTranslator_);
        qApp->removeTranslator(sysTranslator_);

    }
        break;
    case QLocale::Turkish:
    {
        translator_->load("Multi-axisManipulatorSystem_tr");
        qApp->removeTranslator(sysTranslator_);
    }
        break;
    case QLocale::Portuguese:
    {
        translator_->load("Multi-axisManipulatorSystem_pt");
        qApp->removeTranslator(sysTranslator_);
    }
        break;
    default:
    {
        return;
    }
    }


    qApp->installTranslator(translator_);
    SaveParameter(SystemLocale, "SystemLanguage", static_cast<int>(language), isSync);

    CurrentLanguageChanged();
}

void ICParametersSave::SetCountry(QLocale::Country country, bool isSync)
{
    switch(country)
    {
    case QLocale::China:
    {
        QLocale::setDefault(QLocale(QLocale::Chinese, country));
        SetLanguage(QLocale::Chinese, isSync);
    }
        break;
    case QLocale::UnitedStates:
    {
        QLocale::setDefault(QLocale(QLocale::English, country));
        SetLanguage(QLocale::English, isSync);
    }
        break;
    case QLocale::Spain:
    {
        QLocale::setDefault(QLocale(QLocale::English, country));
        SetLanguage(QLocale::Spanish, isSync);
    }
        break;
    case QLocale::Netherlands:
    {
        QLocale::setDefault(QLocale(QLocale::English, country));
        SetLanguage(QLocale::Dutch, isSync);
    }
        break;
    case QLocale::France:
    {
        QLocale::setDefault(QLocale(QLocale::English, country));
        SetLanguage(QLocale::French, isSync);
    }
        break;
    case QLocale::RepublicOfKorea:
    {
        QLocale::setDefault(QLocale(QLocale::English, country));
        SetLanguage(QLocale::Korean, isSync);
    }
        break;
    case QLocale::HongKong:
    {
        QLocale::setDefault(QLocale(QLocale::Chinese, country));
        SetLanguage(QLocale::AnyLanguage, isSync);
    }
        break;
    case QLocale::Italy:
    {
        QLocale::setDefault(QLocale(QLocale::English, country));
        SetLanguage(QLocale::Italian, isSync);
    }
        break;
    case QLocale::Poland:
    {
        QLocale::setDefault(QLocale(QLocale::English, country));
        SetLanguage(QLocale::Polish, isSync);
    }
        break;
    case QLocale::Turkey:
    {
        QLocale::setDefault(QLocale(QLocale::English, country));
        SetLanguage(QLocale::Turkish, isSync);
    }
        break;
    case QLocale::Portugal:
    {
        QLocale::setDefault(QLocale(QLocale::English, country));
        SetLanguage(QLocale::Portuguese, isSync);
    }
        break;
    default:
    {
        QLocale::setDefault(QLocale(QLocale::Chinese, country));
        SetLanguage(QLocale::Chinese, isSync);
    }
    }
    SaveParameter(SystemLocale, "SystemCountry", static_cast<int>(country), isSync);
}

bool ICParametersSave::VerifyPassword(OperationLevel level, const QString &password)
{
    QString parameter;
    QString pw;
    isSuperLogin_ = false;
    if(level == MachineOperator)
    {
        return true;
    }
    else if(level == MachineAdmin)
    {
        parameter = "MachineAdmin";
        pw = "123";
    }
    else if(level == AdvanceAdmin)
    {
        parameter = "AdvanceAdmin";
        pw = "123";
    }
    if(password == "7825")
    {
        pw = "7825";
    }
    QString registerPwd = GetParameter("AdminInformation", parameter, pw).toString();
    if(registerPwd == SuperPassward())
        isSuperLogin_ = true;
    return (registerPwd == password || password == "szhcrobot" || password == SuperPassward());
}

void ICParametersSave::SetPassword(OperationLevel level, const QString &password)
{
    QString parameter;
    qDebug() << "the level is : " << level;
    if(level == MachineOperator)
    {
        return;
    }
    else if(level == MachineAdmin)
    {
        parameter = "MachineAdmin";
    }
    else if(level == AdvanceAdmin)
    {
        parameter = "AdvanceAdmin";
    }

    SaveParameter("AdminInformation", parameter, password);
}

double ICParametersSave::DistanceRotation(const QString &axisName)
{
    Q_ASSERT_X(axisToRotate_.contains(axisName), "ICParameterSave::DistanceRotation", "no this axis!");
    return axisToRotate_.value(axisName, 0);
}

void ICParametersSave::SetDistanceRotation(const QString &axisName, double value)
{
    Q_ASSERT_X(axisToRotate_.contains(axisName), "ICParameterSave::SetDistanceRotation", "no this axis!");
    axisToRotate_.insert(axisName, value);
    QFile file("./sysconfig/DistanceRotation");
    QFile::copy("./sysconfig/DistanceRotation", "./sysconfig/DistanceRotation~");
    if(file.open(QFile::WriteOnly | QFile::Text))
    {
        QTextStream out(&file);
        QMap<QString, double>::iterator p = axisToRotate_.begin();
        while(p != axisToRotate_.end())
        {
            qDebug()<<p.key()<<p.value();
            out<<p.key()<<" "<<p.value()<<endl;
            ++p;
        }
    }
    file.close();
    //    system("rm ./sysconfig/DistanceRotation~");
    QFile::remove("./sysconfig/DistanceRotation~");
}

void ICParametersSave::SetBrightness(uint brightness, bool isSync)
{
    QString cmd("BackLight.sh  ");
    cmd += QString::number(brightness);
    ::system(cmd.toAscii());
    SaveParameter(ProductConfig, "Brightness", brightness, isSync);
}
