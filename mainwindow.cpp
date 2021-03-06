#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    int x=(QApplication::desktop()->width() - this->width())/2;
    int y=(QApplication::desktop()->height() - this->height())/2;
    int readSet=readSetting();
    ui->setupUi(this);
    this->move (x,y);
    ui->txtbInfo->document()->setMaximumBlockCount(MaxLine);
    ui->btStart->setShortcut(Qt::Key_A);
    ui->btOpen->setShortcut(Qt::Key_O);
    ui->btQuit->setShortcut(Qt::Key_S);
    ui->lbInfo->setText(welcome);
    autoStart();
    autoHideWin();
}

MainWindow::~MainWindow()
{
    if(bStart)
    {
        p->kill();
    }
    delete ui;
}

int MainWindow::readSetting()
{
//    if(QFile::exists(conf))
//    {
//        configIni = new QSettings(conf, QSettings::IniFormat);
//        configIni->setIniCodec("UTF8");
//        configIni->beginGroup("Settings");
//        //        QString atrun=configIni->value("autoRun").toString();
//        //        bool ok;
//        //        autoRun = atrun.toInt(&ok, 10);
//        autoRun = configIni->value("autoRun").toInt();
//        autoHide = configIni->value("autoHide").toInt();
//        filename = configIni->value("filename").toString();
//        configIni->endGroup();
//        return 1;
//    }
//    else
//    {
//        return -1;
//    }
    settings = new QSettings(organization, application);
    autoRun = settings->value("autoRun").toInt();
    autoHide = settings->value("autoHide").toInt();
    filename = settings->value("filename").toString();
    return 1;
}

int MainWindow::saveSetting()
{
//    //if(QFile::exists(conf))
//    configIni = new QSettings(conf, QSettings::IniFormat);
//    if(!configIni)
//        return -1;
//    else
//    {
//        configIni->setIniCodec("UTF8");
//        configIni->beginGroup("Settings");
//        configIni->setValue("autoRun",autoRun);
//        configIni->setValue("autoHide",autoHide);
//        configIni->setValue("filename",filename);
//        configIni->endGroup();
//        //        QString confPath = configIni->fileName();
//        //        QMessageBox::information(this, "Warning", confPath, QMessageBox::Ok);
//        return 1;
//    }
    settings = new QSettings(organization, application);
    settings->setValue("autoRun",autoRun);
    settings->setValue("autoHide",autoHide);
    settings->setValue("filename",filename);
    return 1;
}

int MainWindow::startRun()
{
    ui->lbInfo->setText(gainstart);
    p = new QProcess;
    connect(p, SIGNAL(readyReadStandardOutput()), this, SLOT(redFromStdOut()));
    connect(p, SIGNAL(readyReadStandardError()), this, SLOT(redFromStdErr()));
    connect(p, SIGNAL(error(QProcess::ProcessError)), this, SLOT(processError(QProcess::ProcessError)));
    connect(p, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(processFinished(int,QProcess::ExitStatus)));
    p->start(filename);
    bStart = p->waitForStarted();
    //bFinish = p->waitForFinished(-1);
    if(bStart)
        return 1;
    else
        return -1;
//    return 1;
}

void MainWindow::autoHideWin()
{
    if(autoHide>0)
    {
        ui->cbHide->setCheckState(Qt::Checked);
        if(bStart)
        {
            this->showMinimized();
        }
    }
    else
    {
        ui->cbHide->setCheckState(Qt::Unchecked);
    }
}

int MainWindow::getAutoRun()
{
    return autoRun;
}

void MainWindow::autoStart()
{
    if(autoRun>0)
    {
        ui->ckbAuto->setCheckState(Qt::Checked);
        int startret = startRun();
    }
    else
        ui->ckbAuto->setCheckState(Qt::Unchecked);
}

void MainWindow::on_btOpen_clicked()
{
    if(!bStart)
    {
        //QMessageBox::information(this, "Warning", "GoAgentUI is running!", QMessageBox::Ok | QMessageBox::Cancel);
        QString tmpFileName = QFileDialog::getOpenFileName(
                    this,
                    "Open Proxy.py",
                    QDir::currentPath(),
                    "Python files (*.py);;All files(*.*)");
        if (!tmpFileName.isNull()) { //用户选择了文件
            filename = tmpFileName;
            int startret = startRun();
            int saveRet = saveSetting();
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
        //p->kill();
        p->close();
        bStart = false;
        ui->lbInfo->setText(gakill);
    }
    else
    {
        QProcess term;
        QStringList args =QStringList() << "-c" << psGetPid;
        term.start("sh", args);
        term.waitForStarted(-1);
        term.waitForFinished(-1);
        QString psOut = term.readAllStandardOutput();
        int proxyPid = psOut.toInt();
        if(proxyPid > 0)
        {
            QString killcmd = "kill -9 " + QString::number(proxyPid);
            term.start(killcmd);
            term.waitForStarted(-1);
            term.waitForFinished(-1);
            QString coutStr = "Process (" + QString::number(proxyPid) + ") has been killed.\nRestart goagent by pressing Start button.";
            ui->lbInfo->setText(coutStr);
        }
        else
        {
            ui->lbInfo->setText(ganotrun);
        }
    }
}

void MainWindow::on_ckbAuto_stateChanged(int arg1)
{
    autoRun = arg1;
    int saveRet = saveSetting();
}

void MainWindow::on_btStart_clicked()
{
    if(bStart)
    {
        ui->lbInfo->setText(garun);
    }
    else if(QFile::exists(filename))
    {
        int startret = startRun();
    }
    else
    {
        ui->lbInfo->setText(ganotfind);
    }
}

void MainWindow::on_lbName_linkActivated(const QString &link)
{
    //QMessageBox::information(this, "Warning", "buaazp!", QMessageBox::Ok);
    QDesktopServices::openUrl(QUrl(QLatin1String(link.toLatin1().data())));
}

void MainWindow::on_cbHide_stateChanged(int arg1)
{
    autoHide = arg1;
    int saveRet = saveSetting();
}


void MainWindow::redFromStdOut()
{
    //ui->txtbInfo->append("<font color=green>Std OUT:</font>");
    ui->txtbInfo->append(p->readAllStandardOutput());
    //ui->txtbInfo->append("<font color=green>Std OUT End:</font>");
}
void MainWindow::redFromStdErr()
{
    QString strerr = p->readAllStandardError();
    //ui->txtbInfo->append("<font color=red>Std ERROR:</font>");
    ui->txtbInfo->append(strerr);
    //ui->txtbInfo->append("<font color=red>Std ERROR End.</font>");
    QString suberr, errline, strInfo;
    QStringList sublines;
    int index = strerr.indexOf(errflag);
    if(index >= 0)
    {
        bStart = false;
        if(this->isMinimized())
        {
            this->showMaximized();
        }
        suberr = strerr.right(strerr.length()-index);
        sublines = suberr.split('\n');
        errline = sublines.at(0);
        index = errline.indexOf(strPortInUse);
        if(index >= 0)
        {
            strInfo = gafial + " Port " + strPortInUse + "\n" + errline;
        }
        else
        {
            strInfo = gafial + "\n" + errline;
        }
        ui->lbInfo->setText(strInfo);
    }
}
void MainWindow::processError(QProcess::ProcessError error)
{
    if(error==QProcess::FailedToStart)
    {
        bStart=false;
        if(this->isMinimized())
        {
            this->showMaximized();
        }
        ui->lbInfo->setText(gafial);
        ui->txtbInfo->append("<font color=red>QProcess start FAILED.</font>");
    }
}
void MainWindow::processFinished(int exitCode,QProcess::ExitStatus exitStatus)
{
    if(exitStatus == QProcess::CrashExit)
    {
        bStart=false;
        if(this->isMinimized())
        {
            this->showMaximized();
        }
        ui->lbInfo->setText(gakill);
        ui->txtbInfo->append("<font color=red>Proxy.py is TERMINATED.</font>");
    }
    else
    {
        bStart=false;
        if(this->isMinimized())
        {
            this->showMaximized();
        }
        //ui->lbInfo->setText(gafial);
        ui->txtbInfo->append("<font color=green>Proxy.py END.</font>");
    }
}

void MainWindow::on_txtbInfo_textChanged()
{
    ui->txtbInfo->moveCursor(QTextCursor::End);
}
