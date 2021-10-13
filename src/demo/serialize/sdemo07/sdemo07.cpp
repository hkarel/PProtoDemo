/**
  Пример простого версионирования JSON структуры
*/

#include "shared/logger/logger.h"
#include "shared/logger/format.h"
#include "shared/qt/logger_operators.h"

#include "serialize/json.h"

#include <QtCore>

namespace pproto {
namespace data {

struct StructDataOld
{
    qint8   value1;
    qint16  value2;

    J_SERIALIZE_BEGIN
        J_SERIALIZE_ITEM( value1 )
        J_SERIALIZE_ITEM( value2 )
    J_SERIALIZE_END
};

struct StructDataNew
{
    qint8   value1;
    qint16  value2;
    qint32  value3 = {0};
    qint64  value4 = {0};

    J_SERIALIZE_BEGIN
        J_SERIALIZE_ITEM( value1 )
        J_SERIALIZE_ITEM( value2 )
        J_SERIALIZE_OPT ( value3 )
        J_SERIALIZE_OPT ( value4 )
    J_SERIALIZE_END
};

} // namespace data
} // namespace pproto

using namespace pproto::data;

int main(int /*argc*/, char* /*argv*/[])
{
    alog::logger().start();
    alog::logger().addSaverStdOut(alog::Level::Debug2, true);

    log_info << "Start 'Serialize Demo 07'";
    QByteArray buff;

    StructDataOld dataOld;
    dataOld.value1 = 1;
    dataOld.value2 = 2;

    log_info << "---";
    log_info << "Old struct data";
    log_info << "Value1: " << dataOld.value1;
    log_info << "Value2: " << dataOld.value2;

    buff = dataOld.toJson();

    log_info << "---";
    log_info << "JSON representation";
    log_info << buff;

    StructDataNew dataNew;
    dataNew.fromJson(buff);

    log_info << "---";
    log_info << "Deserialization old to new variant";
    log_info << "Value1: " << dataNew.value1;
    log_info << "Value2: " << dataNew.value2;
    log_info << "Value3: " << dataNew.value3;
    log_info << "Value4: " << dataNew.value4;

    dataNew.value1 = 5;
    dataNew.value2 = 6;
    dataNew.value3 = 7;
    dataNew.value4 = 8;

    log_info << "---";
    log_info << "New struct data";
    log_info << "Value1: " << dataNew.value1;
    log_info << "Value2: " << dataNew.value2;
    log_info << "Value3: " << dataNew.value3;
    log_info << "Value4: " << dataNew.value4;

    buff = dataNew.toJson();

    log_info << "---";
    log_info << "JSON representation";
    log_info << buff;

    StructDataOld dataOld2;
    dataOld2.fromJson(buff);

    log_info << "---";
    log_info << "Deserialization new to old variant";
    log_info << "Value1: " << dataOld2.value1;
    log_info << "Value2: " << dataOld2.value2;

    alog::stop();
    return 0;
}
