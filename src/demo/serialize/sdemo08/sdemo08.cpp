/**
  Пример JSON сериализации агрегированных структур данных
*/

#include "shared/logger/logger.h"
#include "shared/logger/format.h"
#include "shared/qt/logger_operators.h"

#include "pproto/serialize/json.h"

#include <QtCore>

namespace pproto {
namespace data {

struct Field1
{
    qint8   value1 = {0};
    quint32 value2 = {0};

    J_SERIALIZE_BEGIN
        J_SERIALIZE_ITEM( value1 )
        J_SERIALIZE_ITEM( value2 )
    J_SERIALIZE_END
};

struct Field2
{
    QString value3;
    QVector<qint32> value4;

    J_SERIALIZE_BEGIN
        J_SERIALIZE_ITEM( value3 )
        J_SERIALIZE_ITEM( value4 )
    J_SERIALIZE_END
};

struct Field3
{
    qint16 value5 = {0};
    qint32 value6 = {0};

    J_SERIALIZE_BEGIN
        J_SERIALIZE_ITEM( value5 )
        J_SERIALIZE_ITEM( value6 )
    J_SERIALIZE_END
};

struct StructData
{
    Field1 field1;
    Field2 field2;
    QList<Field3> field3;

    J_SERIALIZE_BEGIN
        J_SERIALIZE_ITEM( field1 )
        J_SERIALIZE_ITEM( field2 )
        J_SERIALIZE_ITEM( field3 )
    J_SERIALIZE_END
};

} // namespace data
} // namespace pproto

namespace alog {

Line& operator<< (Line& line, const pproto::data::Field3& f3)
{
    line << log_format("{%?, %?}", f3.value5, f3.value6);
    return line;
}

} // namespace alog {

using namespace pproto::data;

int main(int /*argc*/, char* /*argv*/[])
{
    alog::logger().start();
    alog::logger().addSaverStdOut(alog::Level::Debug2, true);

    log_info << "Start 'Serialize Demo 08'";
    QByteArray buff;

    StructData data;
    data.field1.value1 = 1;
    data.field1.value2 = 2;
    data.field2.value3 = "string data";
    data.field2.value4 = {1,2,3,4,5};
    data.field3 = {{1,1}, {2,2}, {3,3}};

    log_info << "---";
    log_info << "Source data";
    log_info << "Field1 Value1:   " << data.field1.value1;
    log_info << "Field1 Value2:   " << data.field1.value2;
    log_info << "Field2 Value3:   " << data.field2.value3;
    log_info << "Field2 Value4:   " << data.field2.value4;
    log_info << "Field3 Value5,6: " << data.field3;

    buff = data.toJson();

    log_info << "---";
    log_info << "JSON representation";
    log_info << buff;

    StructData data2;
    data2.fromJson(buff);

    log_info << "---";
    log_info << "Deserialized data";
    log_info << "Field1 Value1:   " << data2.field1.value1;
    log_info << "Field1 Value2:   " << data2.field1.value2;
    log_info << "Field2 Value3:   " << data2.field2.value3;
    log_info << "Field2 Value4:   " << data2.field2.value4;
    log_info << "Field3 Value5,6: " << data2.field3;

    alog::stop();
    return 0;
}
