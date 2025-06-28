/**
  Пример пассивного версионирования сериализуемой структуры. В функциях сериали-
  зации новые поля данных должны добавляться после существующих полей
*/

#include "shared/logger/logger.h"
#include "shared/logger/format.h"
#include "shared/qt/logger_operators.h"

#include "pproto/serialize/qbinary.h"
#include "pproto/bserialize_space.h"

#include <QtCore>

namespace pproto {
namespace data {

struct StructDataOld
{
    qint8   value1 = {0};
    qint16  value2 = {0};

    DECLARE_B_SERIALIZE_FUNC

    void toRaw2(QDataStream&) const;
};

struct StructDataNew
{
    qint8   value1 = {0};
    qint16  value2 = {0};
    qint32  value3 = {0};
    qint64  value4 = {0};

    DECLARE_B_SERIALIZE_FUNC
};

void StructDataOld::toRaw(bserial::DataStream& stream) const
{
    B_SERIALIZE_V1
    stream << value1;
    stream << value2;
    B_SERIALIZE_RETURN
}

void StructDataOld::fromRaw(const bserial::RawVector& vect)
{
    B_DESERIALIZE_V1(vect)
    stream >> value1;
    stream >> value2;
    B_DESERIALIZE_END
}

void StructDataOld::toRaw2(QDataStream& s) const
{
    bserial::DataStream stream {s.device()};

    QIODevice* device = stream.device();
    qint64 posBegin = device->pos();
    quint8 versionCount = 0;
    stream << versionCount;

    {
        ++versionCount;
        //qint64 pos = qbuff->pos();
        qint64 posDataSize = device->pos();
        quint32 dataSize = 0;
        stream << dataSize;
        qint64 posBeginDataN = device->pos();

        stream << value1;
        stream << value2;

        qint64 posEndDataN = device->pos();
        dataSize = posEndDataN - posBeginDataN;

        device->seek(posDataSize);
        stream << dataSize;
        device->seek(posEndDataN);
    }

    qint64 posEnd = device->pos();
    device->seek(posBegin);
    stream << versionCount;
    device->seek(posEnd);
}

void StructDataNew::toRaw(bserial::DataStream& stream) const
{
    B_SERIALIZE_V1
    stream << value1;
    stream << value2;
    stream << value3;
    stream << value4;
    B_SERIALIZE_RETURN
}

void StructDataNew::fromRaw(const bserial::RawVector& vect)
{
    B_DESERIALIZE_V1(vect)
    stream >> value1;
    stream >> value2;
    stream >> value3;
    stream >> value4;
    B_DESERIALIZE_END
}

} // namespace data
} // namespace pproto

using namespace pproto::data;

int main(int /*argc*/, char* /*argv*/[])
{
    alog::logger().start();
    alog::logger().addSaverStdOut(alog::Level::Debug2, true);

    log_info << "Start 'Serialize Demo 02'";
    QByteArray buff;

    StructDataOld dataOld;
    dataOld.value1 = 1;
    dataOld.value2 = 2;

    log_info << "---";
    log_info << "Old struct data";
    log_info << "Value1: " << dataOld.value1;
    log_info << "Value2: " << dataOld.value2;

//    { //Block for QDataStream
//        QDataStream stream {&buff, QIODevice::WriteOnly};
//        STREAM_INIT(stream);
//        stream << dataOld;
//    }

    //QByteArray buff2;
    { //Block for QDataStream
        QDataStream stream {&buff, QIODevice::WriteOnly};
        STREAM_INIT(stream);
        stream << dataOld;
        //dataOld.toRaw2(stream);
    }

//    StructDataOld dataOld2_;
//    { //Block for QDataStream
//        QDataStream stream {buff2};
//        STREAM_INIT(stream);
//        stream >> dataOld2_;
//    }

    StructDataNew dataNew;
    { //Block for QDataStream
        QDataStream stream {buff};
        STREAM_INIT(stream);
        stream >> dataNew;
    }
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

    buff.clear();
    { //Block for QDataStream
        QDataStream stream {&buff, QIODevice::WriteOnly};
        STREAM_INIT(stream);
        stream << dataNew;
    }

    StructDataOld dataOld2;
    { //Block for QDataStream
        QDataStream stream {buff};
        STREAM_INIT(stream);
        stream >> dataOld2;
    }
    log_info << "---";
    log_info << "Deserialization new to old variant";
    log_info << "Value1: " << dataOld2.value1;
    log_info << "Value2: " << dataOld2.value2;

    alog::stop();
    return 0;
}
