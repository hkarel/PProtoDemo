/**
  Пример сериализации унаследованных JSON структур
*/

#include "shared/logger/logger.h"
#include "shared/logger/format.h"
#include "shared/qt/logger_operators.h"

#include "serialize/json.h"

#include <QtCore>

namespace pproto {
namespace data {

struct BaseStruct
{
    qint8   value1;
    quint32 value2;
    quint64 value3;

    J_SERIALIZE_BASE_BEGIN
        J_SERIALIZE_ITEM( value1 )
        J_SERIALIZE_ITEM( value2 )
        J_SERIALIZE_ITEM( value3 )
    J_SERIALIZE_BASE_END
    J_SERIALIZE_BASE_ONE
};

struct DerivedStruct : BaseStruct
{
    QString value4;
    QUuidEx value5;

    J_SERIALIZE_BEGIN
        J_SERIALIZE_BASE( BaseStruct )
        J_SERIALIZE_ITEM( value4 )
        J_SERIALIZE_ITEM( value5 )
    J_SERIALIZE_END
};

} // namespace data
} // namespace pproto

using namespace pproto::data;

int main(int /*argc*/, char* /*argv*/[])
{
    alog::logger().start();
    alog::logger().addSaverStdOut(alog::Level::Debug2, true);

    log_info << "Start 'Serialize Demo 09'";
    QByteArray buff;

    DerivedStruct data;
    data.value1 = 3;
    data.value2 = 4;
    data.value3 = 5;
    data.value4 = "string data";
    data.value5 = QUuid::createUuid();

    log_info << "---";
    log_info << "Source data";
    log_info << "Value1: " << data.value1;
    log_info << "Value2: " << data.value2;
    log_info << "Value3: " << data.value3;
    log_info << "Value4: " << data.value4;
    log_info << "Value5: " << data.value5;

    buff = data.toJson();

    log_info << "---";
    log_info << "JSON representation";
    log_info << buff;

    BaseStruct data2;
    data2.fromJson(buff);

    log_info << "---";
    log_info << "Deserialized base data";
    log_info << "Value1: " << data2.value1;
    log_info << "Value2: " << data2.value2;
    log_info << "Value3: " << data2.value3;

    DerivedStruct data3;
    data3.fromJson(buff);

    log_info << "---";
    log_info << "Deserialized derived data";
    log_info << "Value1: " << data3.value1;
    log_info << "Value2: " << data3.value2;
    log_info << "Value3: " << data3.value3;
    log_info << "Value4: " << data3.value4;
    log_info << "Value5: " << data3.value5;

    alog::stop();
    return 0;
}
