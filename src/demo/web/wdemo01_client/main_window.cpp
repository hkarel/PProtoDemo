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
#include <unistd.h>

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

    FUNC_REGISTRATION(WebPProtoHello)
    FUNC_REGISTRATION(WebSpeedTest)
    FUNC_REGISTRATION(WebReturnError)
    FUNC_REGISTRATION(WebEmitEvent)

    #undef FUNC_REGISTRATION
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::init()
{
    loadGeometry();
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

    QHostAddress hostAddress {"127.0.0.1"};
    config::readHostAddress("connection.address", hostAddress);

    int port = 28052;
    config::base().getValue("connection.port", port);

    if (!_socket->init({hostAddress, port}))
        return;

    _socket->setName("web-server");
    //_socket->setEchoTimeout(5);
    //_socket->setEncryption(true);
    _socket->setMessageWebFlags(true);
    _socket->setCheckProtocolCompatibility(false);
    _socket->setMessageFormat(SerializeFormat::Json);

    setCursor(Qt::WaitCursor);

    _socket->connect();

    int attempts = 0;
    while (attempts++ < 12 /*ждем 3 сек*/)
    {
        qApp->processEvents();
        usleep(250*1000 /*0.25 сек*/);
        if (_socket->isConnected())
            break;
    }
    setCursor(Qt::ArrowCursor);

    if (!_socket->isConnected())
    {
        _socket->stop();
        _labelConnectStatus->setText(u8"Disconnected");

        QString msg = u8"Failed connect to host %1:%2";
        msg = msg.arg(hostAddress.toString()).arg(port);
        QMessageBox::critical(this, "Error", msg);
    }
}

void MainWindow::on_btnDisconnect_clicked(bool)
{
    _socket->disconnect();
}

void MainWindow::on_btnClose_clicked(bool)
{
    close();
}

void MainWindow::on_btnWebPProtoHello_clicked(bool)
{
    Message::Ptr m = createJsonMessage(command::WebPProtoHello);
    _socket->send(m);
}

void MainWindow::on_btnWebSpeedTest_clicked(bool)
{
    _webSpeedTestTimer.reset();
    _webSpeedTestCount = 0;
    _webSpeedTestStop = false;

    Message::Ptr m = createJsonMessage(command::WebSpeedTest);
    //m->setPriority(Message::Priority::High);

    data::WebSpeedTest webSpeedTest;
    webSpeedTest.beginTest = true;

    writeToJsonMessage(webSpeedTest, m);
    if (!_socket->send(m))
        return;

    alog::logger().off();

    // Комбинированный тест: на скорость, и на загрузку TCP буфера короткими сообщениями
    for (int i = 0; i <= 1000; ++i)
    {
        m = createJsonMessage(command::WebSpeedTest);
        //m->setPriority(Message::Priority::High);

        data::WebSpeedTest webSpeedTest2;
        webSpeedTest2.uuid = m->id();
        writeToJsonMessage(webSpeedTest2, m);
        _socket->send(m);
    }
}

void MainWindow::on_btnWebReturnError_clicked(bool)
{
    Message::Ptr m = createJsonMessage(command::WebReturnError);
    _socket->send(m);
}

void MainWindow::on_btnWebEmitEvent_clicked(bool)
{
    Message::Ptr m = createJsonMessage(command::WebEmitEvent);
    _socket->send(m);
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    // В основном окне приложения метод saveGeometry() нужно вызывать в этой
    // точке, иначе геометрия окна будет сохранятся по разному в разных ОС
    saveGeometry();
    event->accept();
}

void MainWindow::loadGeometry()
{
    QVector<int> v {10, 10, 800, 600};
    config::state().getValue("windows.main_window.geometry", v);
    setGeometry(v[0], v[1], v[2], v[3]);
}

void MainWindow::saveGeometry()
{
    QRect g = geometry();
    QVector<int> v {g.x(), g.y(), g.width(), g.height()};
    config::state().setValue("windows.main_window.geometry", v);
}

void MainWindow::command_WebPProtoHello(const Message::Ptr& message)
{
    data::WebPProtoHello webPProtoHello;
    readFromMessage(message, webPProtoHello);

    QString msg = "Command WebPProtoHello";
    msg += "\nValue: " + webPProtoHello.value;
    msg += "\n_____________________________________________";
    msg += "\nMessage command:\n" + toString(message->command());
    msg += "\nMessage id:\n" + toString(message->id());

    QMessageBox::information(this, "Information", msg);
}

void MainWindow::command_WebSpeedTest(const Message::Ptr& message)
{
    if (_webSpeedTestStop)
        return;

    data::WebSpeedTest webSpeedTest;
    readFromMessage(message, webSpeedTest);

    const int seconds = 10;

    if (_webSpeedTestTimer.elapsed() < (seconds * 1000))
    {
        ++_webSpeedTestCount;
        if (_webSpeedTestCount % 1000 == 0)
        {
            for (int i = 0; i <= 1000; ++i)
            {
                Message::Ptr m = createJsonMessage(command::WebSpeedTest);
                //m->setPriority(Message::Priority::High);

                webSpeedTest.uuid = m->id();
                writeToJsonMessage(webSpeedTest, m);
                _socket->send(m);
            }
        }
        return;
    }

    _webSpeedTestStop = true;

    Message::Ptr m = createJsonMessage(command::WebSpeedTest);
    //m->setPriority(Message::Priority::High);

    webSpeedTest.endTest = true;
    writeToJsonMessage(webSpeedTest, m);
    _socket->send(m);

    alog::logger().on();

    int messagesPerSec = _webSpeedTestCount / seconds;
    QString msg = "Command WebSpeedTest.\nMessages per second: %1";
    QMessageBox::information(this, qApp->applicationName(), msg.arg(messagesPerSec));
}

void MainWindow::command_WebReturnError(const Message::Ptr& message)
{
    if (message->execStatus() != Message::ExecStatus::Success)
    {
        QString descr = errorDescription(message);

        QString msg = "Command WebReturnError";
        msg += "\nError description: " + descr;
        msg += "\n_____________________________________________";
        msg += "\nMessage command:\n" + toString(message->command());
        msg += "\nMessage id:\n" + toString(message->id());

        QMessageBox::information(this, "Information", msg);
    }
}

void MainWindow::command_WebEmitEvent(const Message::Ptr& message)
{
    if (message->type() == Message::Type::Event)
    {
        data::WebEmitEvent webEmitEvent;
        readFromMessage(message, webEmitEvent);

        QString msg = "Event WebEmitEvent";
        msg += "\nEvent data: " + QString::number(webEmitEvent.eventData);
        msg += "\n_____________________________________________";
        msg += "\nMessage command:\n" + toString(message->command());
        msg += "\nMessage id:\n" + toString(message->id());

        QMessageBox::information(this, "Information", msg);
    }
}
