#include "main_window.h"
#include "ui_main_window.h"

#include "shared/defmac.h"
#include "shared/logger/logger.h"
#include "shared/logger/format.h"
#include "shared/config/appl_conf.h"
#include "shared/qt/logger_operators.h"

#include <QApplication>
#include <QMessageBox>
#include <QHostInfo>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    _labelConnectStatus = new QLabel(u8"Disconnected", this);
    ui->statusBar->addWidget(_labelConnectStatus);

    _socket = tcp::Socket::Ptr{new tcp::Socket};
    chk_connect_q(_socket.get(), &tcp::Socket::message,
                  this, &MainWindow::message)
    chk_connect_q(_socket.get(), &tcp::Socket::connected,
                  this, &MainWindow::socketConnected)
    chk_connect_q(_socket.get(), &tcp::Socket::disconnected,
                  this, &MainWindow::socketDisconnected)

    #define FUNC_REGISTRATION(COMMAND) \
        _funcInvoker.registration(command:: COMMAND, &MainWindow::command_##COMMAND, this);

    FUNC_REGISTRATION(TDemo04_01)
    FUNC_REGISTRATION(TDemo04_02)

    #undef FUNC_REGISTRATION
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::init()
{
    return true;
}

void MainWindow::deinit()
{
    _socket->disconnect();
}

void MainWindow::message(const pproto::Message::Ptr& message)
{
    if (message->processed())
        return;

    if (lst::FindResult fr = _funcInvoker.findCommand(message->command()))
    {
        if (command::pool().commandIsSinglproc(message->command()))
            message->markAsProcessed();
        _funcInvoker.call(message, fr);
    }
}

void MainWindow::socketConnected(pproto::SocketDescriptor)
{
    QString msg = u8"Connected to %1:%2";
    _labelConnectStatus->setText(msg.arg(_socket->peerPoint().address().toString())
                                    .arg(_socket->peerPoint().port()));
}

void MainWindow::socketDisconnected(pproto::SocketDescriptor)
{
    _labelConnectStatus->setText(u8"Disconnected");
}

void MainWindow::on_btnConnect_clicked(bool)
{
    if (_socket->isConnected())
        return;

    if (!_socket->init({QHostAddress::LocalHost, 33023}))
        return;

    _socket->setName("python-server");
    //_socket->setEchoTimeout(5);
    //_socket->setEncryption(true);
    //_socket->setMessageWebFlags(true);
    _socket->setCheckProtocolCompatibility(false);
    _socket->setMessageFormat(SerializeFormat::Json);
    _socket->connect();
}

void MainWindow::on_btnDisconnect_clicked(bool)
{
    _socket->disconnect();
}

void MainWindow::on_btnClose_clicked(bool)
{
    close();
}

void MainWindow::on_btnTDemo04_01_clicked(bool)
{
    data::TDemo04_01 tdemo04_01;
    tdemo04_01.value1 = 5;

    Message::Ptr m = createJsonMessage(tdemo04_01);
    _socket->send(m);
}

void MainWindow::on_btnTDemo04_02_clicked(bool)
{
    _speedTestTimer.reset();
    _speedTestCount = 0;
    _speedTestStop = false;

    Message::Ptr m = createJsonMessage(command::TDemo04_02);
    //m->setPriority(Message::Priority::High);

    data::TDemo04_02 tdemo04_02;
    tdemo04_02.beginTest = true;

    writeToJsonMessage(tdemo04_02, m);
    if (!_socket->send(m))
        return;

    alog::logger().off();

    // Комбинированный тест: на скорость, и на загрузку TCP буфера короткими сообщениями
    for (int i = 0; i <= 1000; ++i)
    {
        m = createJsonMessage(command::TDemo04_02);
        //m->setPriority(Message::Priority::High);

        data::TDemo04_02 tdemo04_02;
        tdemo04_02.uuid = m->id();
        writeToJsonMessage(tdemo04_02, m);
        _socket->send(m);
    }
}

void MainWindow::command_TDemo04_01(const Message::Ptr& message)
{
    data::TDemo04_01 tdemo04_01;
    readFromMessage(message, tdemo04_01);

    QString msg = tdemo04_01.value2;
    msg += "\n_____________________________________________";
    msg += "\nMessage command:\n" + toString(message->command());
    msg += "\nMessage id:\n" + toString(message->id());

    QMessageBox::information(this, "Information", msg);
}

void MainWindow::command_TDemo04_02(const Message::Ptr& message)
{
    if (_speedTestStop)
        return;

    data::TDemo04_02 tdemo04_02;
    readFromMessage(message, tdemo04_02);

    const int seconds = 10;

    if (_speedTestTimer.elapsed() < (seconds * 1000))
    {
        ++_speedTestCount;
        if (_speedTestCount % 1000 == 0)
        {
            for (int i = 0; i <= 1000; ++i)
            {
                Message::Ptr m = createJsonMessage(command::TDemo04_02);
                //m->setPriority(Message::Priority::High);

                tdemo04_02.uuid = m->id();
                writeToJsonMessage(tdemo04_02, m);
                _socket->send(m);
            }
        }
        return;
    }

    _speedTestStop = true;

    Message::Ptr m = createJsonMessage(command::TDemo04_02);
    //m->setPriority(Message::Priority::High);

    tdemo04_02.endTest = true;
    writeToJsonMessage(tdemo04_02, m);
    _socket->send(m);

    alog::logger().on();

    int messagesPerSec = _speedTestCount / seconds;
    QString msg = "Python speed test.\nMessages per second: %1";
    QMessageBox::information(this, qApp->applicationName(), msg.arg(messagesPerSec));
}
