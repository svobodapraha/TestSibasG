#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>



//***********************************
TbitInput::TbitInput(int iIndex, QWidget *parent) : QCheckBox(parent)
{
    this->setText(tr("B%1").arg(iIndex));
}

bool TbitInput::GetStatus()
{
    return this->isChecked();
}

void TbitInput::Reset()
{
   this->setChecked(false);
}

TbitInput::~TbitInput()
{

}

TbitOutput::TbitOutput(int iIndex, QWidget *parent) : QLineEdit(parent)
{

    this->setText(tr("B%1").arg(iIndex));
    this->setFixedSize(30,15);
    this->setStyleSheet("background-color:white;");

}

void TbitOutput::SetStatus(bool boStatus)
{
   if(boStatus) this->setStyleSheet("background-color:green;color: white;");
   else         this->setStyleSheet("background-color:white;color: black;");
}

TbitOutput::~TbitOutput()
{

}









//*********************************

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    //init
    boDriverInitOK  = false;
    boDriverOpenOK    = false;
    boChannelOpenOK = false;
    memset(abSendData, 0, sizeof(abSendData));
    memset(abRecvData, 0, sizeof(abSendData));
    InputData.u64 = 0;
    OutputData.u64 = 0;

    CleanUpAll();

    ui->setupUi(this);
    ComStatus = this->statusBar();

    iCommOK = -1;
    iTimeSlotID_T5 = this->startTimer(TIME_INTERVAL_T5);


    QGridLayout *bitInputsLayout = new QGridLayout(ui->groupBox_inputs);
    QGridLayout *bitOutputsLayout = new QGridLayout(ui->groupBox_outputs);



    //test adding widgets
    //TbitInput *bitInput1 = new TbitInput(-1, ui->centralWidget);
    int x=0;
    int y=0;
    for (int i = 0; i < NUMBER_OF_BIT_INPUTS; ++i)
    {
      bitInputs[i] =  new TbitInput(i);
      bitOutputs[i] = new TbitOutput(i);
      bitInputsLayout->addWidget(bitInputs[i], x, y);
      bitOutputsLayout->addWidget(bitOutputs[i], x, y);
      //ui->inputsLayout->addWidget(bitInputs[i]);

      //new position
      if ((i+1)%8 == 0)
      {
        x=0;
        y++;
      }
      else
      {
        x++;
      }
    }


}











MainWindow::~MainWindow()
{
    delete ui;
}









int MainWindow::CleanUpAll()
{

  return 0;
}



void MainWindow::timerEvent(QTimerEvent *event)
{

    if (event->timerId() == iTimeSlotID_T5)
    {

        if(ui->cb_RunT5->isChecked()) slotT5();


        //read checkboxes status
        for (int i = 0; i < NUMBER_OF_BIT_INPUTS; ++i)
        {

          if(bitInputs[i]->GetStatus())  InputData.u64 |= (U64(1)<<i);
          else                           InputData.u64 &= ~(U64(1)<<i);


        }








        //qDebug() << InputData.u64;
        OutputData = InputData;




        //write to indicators
        for (int i = 0; i < NUMBER_OF_BIT_INPUTS; ++i)
        {

          if((OutputData.u64 >> i)& 0x1 ) bitOutputs[i]->SetStatus(true);
          else                            bitOutputs[i]->SetStatus(false);

        }



    }
}



void MainWindow::on_btn_StepT5_clicked()
{
    slotT5();
}

int MainWindow::slotT5()
{
    int iThisTimerInterval = TIME_INTERVAL_T5;
    //timer instances
    static DELAYON  DELAYON_P3074DEL(iThisTimerInterval);   //400T3
    static AND2     AND2____P3074AND(iThisTimerInterval);   //480T5
    static DELAYON  DELAYON_P3074DE0(iThisTimerInterval);   //490T5
    static OR2      OR2_____OR_BSG13(iThisTimerInterval);   //500T5
    static RSFFSET  RSFFSET_P3074FF1(iThisTimerInterval);   //510T5
    static DELAYON  DELAYON_P3074DE1(iThisTimerInterval);   //520T5

    //test timers
    #define KN_TEST_TOGGLE_INTERVAL 5000;
    static int iTestTimerCounter = KN_TEST_TOGGLE_INTERVAL;
    static bool boTestToggle = false;
    if(iTestTimerCounter > 0)
    {
        iTestTimerCounter -= iThisTimerInterval;
    }
    else
    {
        iTestTimerCounter = KN_TEST_TOGGLE_INTERVAL;
        boTestToggle = !boTestToggle;
    }


//    bool B1 = P3074DE0(boTestToggle, 1000);
//    bool B2 = P3074DE1(boTestToggle, 2000);

//    qDebug() << boTestToggle << P3074DE0 << P3074DE1 << B1 << B2;

//    //test and
//    P3074AND(ui->cb_F_ZSGM_TE_ist_fuehrend->isChecked(), ui->cb_Z_BSGZ_ZugSteuerBremse1_STS->isChecked());
//    SHOW_ON_FORM(AND, P3074AND);


    //SIM START
    //INPUTS
    bool F_ZSGM_TE_ist_fuehrend         = ui->cb_F_ZSGM_TE_ist_fuehrend->isChecked();
    bool Z_BSGZ_ZugSteuerBremse1_STS    = ui->cb_Z_BSGZ_ZugSteuerBremse1_STS->isChecked();

    //CORE
    DELAYON_P3074DEL(F_ZSGM_TE_ist_fuehrend, 10000);
    AND2____P3074AND(Z_BSGZ_ZugSteuerBremse1_STS, DELAYON_P3074DEL);
    DELAYON_P3074DE0(AND2____P3074AND, 1000);
    OR2_____OR_BSG13(!DELAYON_P3074DEL, DELAYON_P3074DE1);
    RSFFSET_P3074FF1(DELAYON_P3074DE0, OR2_____OR_BSG13);
    DELAYON_P3074DE1(RSFFSET_P3074FF1, 6000);

    //INDICATION
    SHOW_ON_FORM(ERR, DELAYON_P3074DE1);

    static short int iPassCounter = 0;
    iPassCounter++;
    QString asPassCounter = QStringLiteral("%1").arg(iPassCounter, 6, 10, QLatin1Char(' '));
    qDebug()
        <<  asPassCounter  << "#"
        << "DELAYON_P3074DEL:" << DELAYON_P3074DEL
        << "AND2____P3074AND:" << AND2____P3074AND
        << "DELAYON_P3074DE0:" << DELAYON_P3074DE0
        << "OR2_____OR_BSG13:" << OR2_____OR_BSG13
        << "RSFFSET_P3074FF1:" << RSFFSET_P3074FF1
        << "DELAYON_P3074DE1:" << DELAYON_P3074DE1;
    return 0;
}



void MainWindow::on_btnResetInputs_clicked()
{
    for (int i = 0; i < NUMBER_OF_BIT_INPUTS; ++i)
    {

      bitInputs[i]->Reset();
    }
}


//***************
//* AUX CLASSES *
//* DELAYON     *
//***************
DELAYON::DELAYON(int iTimerIntervalInit)
{
   iCounter = 0;
   boIN_Last = false;
   boOUT = false;
   iTimerInterval = iTimerIntervalInit;
}

bool DELAYON::operator()(bool boIN, int iT)
{
    if(boIN && !boIN_Last)
    {
      iCounter = iT;
    }

    if(iCounter > 0)
    {
        iCounter -= iTimerInterval;
        boOUT = false;
    }
    else
    {
        boOUT = boIN;
    }



    boIN_Last = boIN;
    //qDebug() << boIN << iT << boOUT << iCounter << iTimerInterval;
    return boOUT;
}

DELAYON::operator bool() const
{
    return boOUT;
}

//***************
//* AUX CLASSES *
//* RSFFSET     *
//***************
RSFFSET::RSFFSET(int iTimerIntervalInit)
{
   boOUT = false;
   iTimerInterval = iTimerIntervalInit;
}

bool RSFFSET::operator()(bool boSET, bool boRES)
{
    if(boSET)
    {
      boOUT = true;
    }
    else
    {
      if(boRES)
      {
        boOUT = false;
      }
    }

    return boOUT;
}

RSFFSET::operator bool() const
{
    return boOUT;
}

//***************
//* AUX CLASSES *
//* AND2        *
//***************
AND2::AND2(int iTimerIntervalInit)
{
   boOUT = false;
   iTimerInterval = iTimerIntervalInit;
}

bool AND2::operator()(bool boIN1, bool boIN2)
{

    boOUT = boIN1 && boIN2;
    return boOUT;
}

AND2::operator bool() const
{
    return boOUT;
}

//***************
//* AUX CLASSES *
//* OR2        *
//***************
OR2::OR2(int iTimerIntervalInit)
{
   boOUT = false;
   iTimerInterval = iTimerIntervalInit;
}

bool OR2::operator()(bool boIN1, bool boIN2)
{

    boOUT = boIN1 || boIN2;
    return boOUT;
}

OR2::operator bool() const
{
    return boOUT;
}


