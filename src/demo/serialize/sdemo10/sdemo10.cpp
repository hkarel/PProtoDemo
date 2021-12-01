/**
  JSON сериализация, маппинг наименований полей
*/

#include "shared/logger/logger.h"
#include "shared/logger/format.h"
#include "shared/qt/logger_operators.h"

#include "serialize/json.h"

#include <QtCore>

namespace pproto {
namespace data {

struct StructData
{
    qint8   value1 = {0};
    qint32  value2 = {0};
    QString value3;
    QUuidEx value4;
    QVector<qint32> value5;

    J_SERIALIZE_BEGIN
        J_SERIALIZE_MAP_ITEM( "value_name1", value1 )
        J_SERIALIZE_MAP_ITEM( "value_name2", value2 )
        J_SERIALIZE_MAP_ITEM( "value_name3", value3 )
        J_SERIALIZE_MAP_ITEM( "value_name4", value4 )
        J_SERIALIZE_MAP_ITEM( "value_name5", value5 )
    J_SERIALIZE_END
};

} // namespace data
} // namespace pproto

using namespace pproto::data;

int main(int /*argc*/, char* /*argv*/[])
{
    alog::logger().start();
    alog::logger().addSaverStdOut(alog::Level::Debug2, true);

    log_info << "Start 'Serialize Demo 10'";
    QByteArray buff;

    StructData data1;
    data1.value1 = 1;
    data1.value2 = 2;
    data1.value3 = "Hello PProto";
    data1.value4 = QUuid::createUuid();
    data1.value5 = {1,2,3,4,5};

    buff = data1.toJson();

    log_info << "---";
    log_info << "JSON representation";
    log_info << buff;

    StructData data2;
    data2.fromJson(buff);

    log_info << "---";
    log_info << "Deserialization";
    log_info << "Value1: " << data2.value1;
    log_info << "Value2: " << data2.value2;
    log_info << "Value3: " << data2.value3;
    log_info << "Value4: " << data2.value4;
    log_info << "Value5: " << data2.value5;

    alog::stop();
    return 0;
}
