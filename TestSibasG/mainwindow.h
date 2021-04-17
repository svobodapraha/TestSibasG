

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCheckBox>
#include <QWidget>
#include <QGroupBox>
#include <QLineEdit>
#include "stdint.h"
#include "stdtypes.h"



#define TIME_INTERVAL_T5  250


//macro to show binary value in LineEdit
#define SHOW_ON_FORM(ind, value)\
{\
    if(value) {ui->lineEdit_##ind->setText("TRUE");  ui->lineEdit_##ind->setStyleSheet("background-color:green");}\
    else      {ui->lineEdit_##ind->setText("FALSE"); ui->lineEdit_##ind->setStyleSheet("background-color:red");}\
}



#define BIT_INPUT_PITCH 20
#define NUMBER_OF_BIT_INPUTS 64
class TbitInput : public QCheckBox
{
    Q_OBJECT

public:
    TbitInput(int iIndex = -1, QWidget *parent = nullptr);
    ~TbitInput();
    bool GetStatus();
    void Reset();
};

class TbitOutput : public QLineEdit
{
    //Q_OBJECT
public:
    TbitOutput(int iIndex = -1, QWidget *parent = nullptr);
    ~TbitOutput();
    void SetStatus(bool boStatus);
};



//Delay on class
class DELAYON
{
public:
    DELAYON(int iTimerIntervalInit);
    bool operator()(bool boIN, int iT);
    operator bool() const;

private:
    int iCounter;
    int iTimerInterval;
    bool boIN_Last;
    bool boOUT;
};


class RSFFSET
{
public:
    RSFFSET(int iTimerIntervalInit = 0);
    bool operator()(bool boSET, bool boRES);
    operator bool() const;

private:
    int iTimerInterval;
    bool boOUT;
};

class AND2
{
public:
    AND2(int iTimerIntervalInit = 0);
    bool operator()(bool boIN1, bool boIN2);
    operator bool() const;

private:
    int iTimerInterval;
    bool boOUT;
};

class OR2
{
public:
    OR2(int iTimerIntervalInit = 0);
    bool operator()(bool boIN1, bool boIN2);
    operator bool() const;

private:
    int iTimerInterval;
    bool boOUT;
};



namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    int slotT5(void);

protected:
    void timerEvent(QTimerEvent *event);
private slots:

     void on_btnResetInputs_clicked();


     void on_btn_StepT5_clicked();

private:
    Ui::MainWindow *ui;



    int iTimeSlotID_T5;
    int32_t iCommOK;

public:
    TbitInput *bitInputs[NUMBER_OF_BIT_INPUTS];
    TbitOutput *bitOutputs[NUMBER_OF_BIT_INPUTS];
    UU64 InputData;
    UU64 OutputData;
    uint8_t  abSendData[32];
    uint8_t  abRecvData[32];

    bool boDriverInitOK;
    bool boDriverOpenOK;
    bool boChannelOpenOK;
    bool boBusCommOK;

    int CleanUpAll();

    QStatusBar * ComStatus;
};




#endif // MAINWINDOW_H
