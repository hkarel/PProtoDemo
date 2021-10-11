
#include "shared/logger/logger.h"
#include "shared/logger/format.h"
#include "shared/qt/logger_operators.h"

#include "pproto/serialize/qbinary.h"
#include "pproto/bserialize_space.h"

#include <QtCore>

namespace pproto {
namespace data {

struct StructData
{
    qint8   value1;
    qint16  value2;
    qint32  value3;
    qint64  value4;
    QString value5;

    DECLARE_B_SERIALIZE_FUNC
};

bserial::RawVector StructData::toRaw() const
{
    B_SERIALIZE_V1(stream)
    stream << value1;
    stream << value2;
    stream << value3;
    stream << value4;
    stream << value5;
    B_SERIALIZE_RETURN
}

void StructData::fromRaw(const bserial::RawVector& vect)
{
    B_DESERIALIZE_V1(vect, stream)
    stream >> value1;
    stream >> value2;
    stream >> value3;
    stream >> value4;
    stream >> value5;
    B_DESERIALIZE_END
}

} // namespace data
} // namespace pproto

using namespace pproto::data;

int main(int /*argc*/, char* /*argv*/[])
{
    alog::logger().start();
    alog::logger().addSaverStdOut(alog::Level::Debug2);

    log_info << "Start 'Serialize Demo 01'";

    QByteArray buff;

    { //Block for QDataStream
        QDataStream stream1 {&buff, QIODevice::WriteOnly};
        STREAM_INIT(stream1);

        StructData data1;
        data1.value1 = 1;
        data1.value2 = 2;
        data1.value3 = 3;
        data1.value4 = 4;
        data1.value5 = "Hello PProto";

        stream1 << data1;
    }

    { //Block for QDataStream
        QDataStream stream2 {buff};
        STREAM_INIT(stream2);

        StructData data2;
        stream2 >> data2;

        log_info << "value1: " << data2.value1;
        log_info << "value2: " << data2.value2;
        log_info << "value3: " << data2.value3;
        log_info << "value4: " << data2.value4;
        log_info << "value5: " << data2.value5;
    }

    log_info << "Stop 'Serialize Demo 01'";
    alog::stop();
    return 0;
}
