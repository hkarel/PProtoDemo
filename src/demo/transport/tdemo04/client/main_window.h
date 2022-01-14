#pragma once

#include "commands/commands.h"
#include "commands/error.h"

#include "shared/steady_timer.h"
#include "pproto/commands/base.h"
#include "pproto/commands/pool.h"
#include "pproto/func_invoker.h"
#include "pproto/transport/tcp.h"

#include <QMainWindow>
#include <QLabel>
#include <atomic>

using namespace std;
using namespace pproto;
using namespace pproto::transport;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    bool init();
    void deinit();

private slots:
    void message(const pproto::Message::Ptr&);
    void socketConnected(pproto::SocketDescriptor);
    void socketDisconnected(pproto::SocketDescriptor);

    void on_btnConnect_clicked(bool);
    void on_btnDisconnect_clicked(bool);
    void on_btnClose_clicked(bool);

    void on_btnTDemo04_01_clicked(bool);
    void on_btnTDemo04_02_clicked(bool);

private:
    Q_OBJECT

    //--- Обработчики команд ---
    void command_TDemo04_01(const Message::Ptr&);
    void command_TDemo04_02(const Message::Ptr&);

private:
    Ui::MainWindow *ui;
    QLabel* _labelConnectStatus;

    tcp::Socket::Ptr _socket;
    FunctionInvoker  _funcInvoker;

    steady_timer _speedTestTimer;
    quint32 _speedTestCount;
    bool _speedTestStop;
};
