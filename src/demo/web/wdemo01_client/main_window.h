#pragma once

#include "commands/commands.h"
#include "commands/error.h"

#include "pproto/commands_base.h"
#include "pproto/commands_pool.h"
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

    void on_btnWebPProtoHello_clicked(bool);



    //void checkConnect();
    //void internalConnect();

private:
    Q_OBJECT
    void closeEvent(QCloseEvent*) override;

    void loadGeometry();
    void saveGeometry();

    //--- Обработчики команд ---
    void command_WebPProtoHello(const Message::Ptr&);

private:
    Ui::MainWindow *ui;
    //static QUuidEx _applId;

    QLabel* _labelConnectStatus;

    tcp::Socket::Ptr _socket;
    FunctionInvoker  _funcInvoker;

    //QUuidEx _taskId;
    //atomic_bool _checkingConnect = {false};
};
