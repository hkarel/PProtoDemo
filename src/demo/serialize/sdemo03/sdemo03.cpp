/**
  Пример версионирования сериализуемой структуры с заменой типа поля
*/

#include "shared/logger/logger.h"
#include "shared/logger/format.h"
#include "shared/qt/logger_operators.h"

#include "pproto/serialize/qbinary.h"
#include "pproto/bserialize_space.h"

#include <QtCore>

namespace pproto {
namespace data {

struct StructDataV1
{
    qint8           value1;
    quint32         value2;
    QVector<qint32> value3;

    DECLARE_B_SERIALIZE_FUNC
};

struct StructDataV2
{
    qint8           value1;
    QString         value2; // Изменен тип поля
    QVector<qint32> value3;

    DECLARE_B_SERIALIZE_FUNC
};

bserial::RawVector StructDataV1::toRaw() const
{
    B_SERIALIZE_V1(stream)
    stream << value1;
    stream << value2;
    stream << value3;
    B_SERIALIZE_RETURN
}

void StructDataV1::fromRaw(const bserial::RawVector& vect)
{
    B_DESERIALIZE_V1(vect, stream)
    stream >> value1;
    stream >> value2;
    stream >> value3;
    B_DESERIALIZE_END
}

bserial::RawVector StructDataV2::toRaw() const
{
    B_SERIALIZE_V1(stream)
    stream << value1;
    stream << qHash(value2); // Сохраняем совместимость со старой версией,
                             // записываем какое-то осмысленное значение
    stream << value3;
    B_SERIALIZE_V2(stream)
    stream << value2;
    B_SERIALIZE_RETURN
}

void StructDataV2::fromRaw(const bserial::RawVector& vect)
{
    B_DESERIALIZE_V1(vect, stream)
    stream >> value1;

    quint32 oldValue2;
    stream >> oldValue2;
    value2 = QString::number(oldValue2); // При необходимости можно заполнить
                                         // поле значением из версии V1
    stream >> value3;
    B_DESERIALIZE_V2(vect, stream)
    stream >> value2;
    B_DESERIALIZE_END
}

} // namespace data
} // namespace pproto

using namespace pproto::data;

int main(int /*argc*/, char* /*argv*/[])
{
    alog::logger().start();
    alog::logger().addSaverStdOut(alog::Level::Debug2, true);

    log_info << "Start 'Serialize Demo 03'";
    QByteArray buff;

    StructDataV1 dataV1;
    dataV1.value1 = 1;
    dataV1.value2 = 2;
    dataV1.value3 = {1,2,3,4,5};

    log_info << "---";
    log_info << "Source data V1";
    log_info << "V1 value1: " << dataV1.value1;
    log_info << "V1 value2: " << dataV1.value2;
    log_info << "V1 value3: " << dataV1.value3;

    { //Block for QDataStream
        QDataStream stream {&buff, QIODevice::WriteOnly};
        STREAM_INIT(stream);
        stream << dataV1;
    }

    StructDataV2 dataV2;
    { //Block for QDataStream
        QDataStream stream {buff};
        STREAM_INIT(stream);
        stream >> dataV2;
    }
    log_info << "---";
    log_info << "Deserialization V1 -> V2";
    log_info << "V2 value1: " << dataV2.value1;
    log_info << "V2 value2: " << dataV2.value2;
    log_info << "V2 value3: " << dataV2.value3;

    dataV2.value1 = 5;
    dataV2.value2 = "string data";
    dataV2.value3 = {11,12,13};

    log_info << "---";
    log_info << "New data for V2";
    log_info << "V2 value1: " << dataV2.value1;
    log_info << "V2 value2: " << dataV2.value2;
    log_info << "V2 value3: " << dataV2.value3;

    buff.clear();
    { //Block for QDataStream
        QDataStream stream {&buff, QIODevice::WriteOnly};
        STREAM_INIT(stream);
        stream << dataV2;
    }

    { //Block for QDataStream
        QDataStream stream {buff};
        STREAM_INIT(stream);
        stream >> dataV1;
    }
    log_info << "---";
    log_info << "Back deserialization V2 -> V1";
    log_info << "V1 value1: " << dataV1.value1;
    log_info << "V1 value2: " << dataV1.value2;
    log_info << "V1 value3: " << dataV1.value3;


    StructDataV2 dataV2_;
    { //Block for QDataStream
        QDataStream stream {buff};
        STREAM_INIT(stream);
        stream >> dataV2_;
    }
    log_info << "---";
    log_info << "Deserialization V2 to new V2'";
    log_info << "V2' value1: " << dataV2_.value1;
    log_info << "V2' value2: " << dataV2_.value2;
    log_info << "V2' value3: " << dataV2_.value3;

    alog::stop();
    return 0;
}
