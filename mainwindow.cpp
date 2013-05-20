#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    int readSet=readSetting();
    ui->setupUi(this);
    if(autoRun>0)
        ui->ckbAuto->setCheckState(Qt::Checked);
    else
        ui->ckbAuto->setCheckState(Qt::Unchecked);
}

MainWindow::~MainWindow()
{
    if(bStart)
    {
        p->kill();
    }
    int saveRet = saveSetting();
    delete ui;
}

int MainWindow::readSetting()
{
    if(QFile::exists(conf))
    {
        configIni = new QSettings(conf, QSettings::IniFormat);
        configIni->setIniCodec("UTF8");
        configIni->beginGroup("Settings");
//        QString atrun=configIni->value("autoRun").toString();
//        bool ok;
//        autoRun = atrun.toInt(&ok, 10);
        autoRun = configIni->value("autoRun").toInt();
        filename = configIni->value("filename").toString();
        configIni->endGroup();
        return 1;
    }
    else
    {
        return -1;
    }
}

int MainWindow::saveSetting()
{
    //if(QFile::exists(conf))
    configIni = new QSettings(conf, QSettings::IniFormat);
    if(!configIni)
        return -1;
    else
    {
        configIni->setIniCodec("UTF8");
        configIni->beginGroup("Settings");
//        QString atrun=QString::number(autoRun, 10);
//        configIni->setValue("autoRun",atrun);
        configIni->setValue("autoRun",autoRun);
        configIni->setValue("filename",filename);
        configIni->endGroup();
        return 1;
    }
}

void MainWindow::on_btOpen_clicked()
{
    if(!bStart)
    {
        //QMessageBox::information(this, "Warning", "GoAgentUI is running!", QMessageBox::Ok | QMessageBox::Cancel);
        filename = QFileDialog::getOpenFileName(
                    this,
                    "Open Proxy.py",
                    QDir::currentPath(),
                    "Document files (*.py);;All files(*.*)");
        if (!filename.isNull()) { //用户选择了文件
            //process way
            p = new QProcess;
            p->start(filename);
            bStart = p->waitForStarted();
            if(!bStart)
            {
                ui->lbInfo->setText(gatimeout);
                //ui->btQuit->setText("Start(A)");
            }
            else
            {
                qDebug()<<QString::fromLocal8Bit(p->readAllStandardOutput());
                ui->lbInfo->setText(gastart);
                //ui->btQuit->setText("Stop(S)");
            }
        }
        else // 用户取消选择
            ui->lbInfo->setText(ganotfind);
    }
    else
    {
        ui->lbInfo->setText(garun);
    }
}

void MainWindow::on_btQuit_clicked()
{
    if(bStart)
    {
        p->kill();
        ui->lbInfo->setText(gakill);
        bStart = false;
        //ui->btQuit->setText("Start(A)");
    }
    else
    {
        ui->lbInfo->setText(ganotrun);
    }
}

void MainWindow::on_ckbAuto_stateChanged(int arg1)
{
    autoRun = arg1;
    if(autoRun > 0)
        ui->lbInfo->setText("AutoRun selected.");
    else
        ui->lbInfo->setText("AutoRun unselect.");
}

void MainWindow::on_btStart_clicked()
{
    if(bStart)
    {
        ui->lbInfo->setText(garun);
    }
    else if(QFile::exists(filename))
    {
        p = new QProcess;
        p->start(filename);
        bStart = p->waitForStarted();
        if(!bStart)
        {
            ui->lbInfo->setText(gatimeout);
            //ui->btQuit->setText("Start(A)");
        }
        else
        {
            qDebug()<<QString::fromLocal8Bit(p->readAllStandardOutput());
            ui->lbInfo->setText(gastart);
            //ui->btQuit->setText("Stop(S)");
        }
    }
    else
    {
        ui->lbInfo->setText(ganotfind);
    }
}
