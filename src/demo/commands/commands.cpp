#include "commands.h"

#include "pproto/commands/pool.h"

namespace pproto {
namespace command {

#define REGISTRY_COMMAND_SINGLPROC(COMMAND, UUID) \
    const QUuidEx COMMAND = command::Pool::Registry{UUID, #COMMAND, false};

#define REGISTRY_COMMAND_MULTIPROC(COMMAND, UUID) \
    const QUuidEx COMMAND = command::Pool::Registry{UUID, #COMMAND, true};

REGISTRY_COMMAND_SINGLPROC(TDemo01,            "76179ba9-c22d-4959-9c52-e853fbf52db0")

REGISTRY_COMMAND_SINGLPROC(TDemo02_01,         "6eee6784-3472-4724-a2c0-6053a3010324")
REGISTRY_COMMAND_SINGLPROC(TDemo02_02,         "6422f74e-73d1-43dc-b4aa-90312a850bc2")
REGISTRY_COMMAND_SINGLPROC(TDemo02_03,         "5ac9ebf2-4887-44f5-9d23-a7aaa7ccc86d")
REGISTRY_COMMAND_SINGLPROC(TDemo02_04,         "faed04a6-4654-497b-adbc-8e22eada7655")
REGISTRY_COMMAND_SINGLPROC(TDemo02_05,         "be816a45-1449-4a95-bc19-18d61bc50946")
REGISTRY_COMMAND_SINGLPROC(TDemo02_06,         "48d32586-2cf0-4721-a7c4-d7c6d5191db9")

REGISTRY_COMMAND_SINGLPROC(TDemo03_01,         "ae2347f4-788b-4135-961c-166a3b49d65f")
REGISTRY_COMMAND_SINGLPROC(TDemo03_02,         "114949cb-2b6a-48f4-a5a4-15a682b2f45a")

REGISTRY_COMMAND_SINGLPROC(TDemo04_01,         "6f241482-bd55-4787-89e9-97a469ec67a8")
REGISTRY_COMMAND_SINGLPROC(TDemo04_02,         "7ebe2813-6054-4e1d-a94f-c7db26d4def9")

REGISTRY_COMMAND_SINGLPROC(TDemo05_GetData,    "fddb9a45-5775-489c-a0c5-d9ea73e20e23")
REGISTRY_COMMAND_SINGLPROC(TDemo05_SendChunk,  "253dd71f-c4e4-4718-880e-01547656ee5b")
REGISTRY_COMMAND_SINGLPROC(TDemo05_SendFinish, "845785c0-9d2a-4351-943e-4d22a84480a5")
REGISTRY_COMMAND_SINGLPROC(TDemo05_SendError,  "2c9e8369-abb9-4fb7-91d7-42c284622682")

REGISTRY_COMMAND_SINGLPROC(WebPProtoHello,     "b8338344-bec9-4f7d-b8e2-b81a6d4591c7")
REGISTRY_COMMAND_SINGLPROC(WebSpeedTest,       "59cb5357-80bb-4fa4-a15e-4797a535b50d")

#undef REGISTRY_COMMAND_SINGLPROC
#undef REGISTRY_COMMAND_MULTIPROC

} // namespace command

namespace data {

bserial::RawVector TDemo01::toRaw() const
{
    B_SERIALIZE_V1(stream)
    stream << value1;
    B_SERIALIZE_RETURN
}

void TDemo01::fromRaw(const bserial::RawVector& vect)
{
    B_DESERIALIZE_V1(vect, stream)
    stream >> value1;
    B_DESERIALIZE_END
}

bserial::RawVector TDemo02_01::toRaw() const
{
    B_SERIALIZE_V1(stream)
    stream << value1;
    B_SERIALIZE_RETURN
}

void TDemo02_01::fromRaw(const bserial::RawVector& vect)
{
    B_DESERIALIZE_V1(vect, stream)
    stream >> value1;
    B_DESERIALIZE_END
}

bserial::RawVector TDemo02_02::toRaw() const
{
    B_SERIALIZE_V1(stream)
    stream << value1;
    stream << value2;
    B_SERIALIZE_RETURN
}

void TDemo02_02::fromRaw(const bserial::RawVector& vect)
{
    B_DESERIALIZE_V1(vect, stream)
    stream >> value1;
    stream >> value2;
    B_DESERIALIZE_END
}

bserial::RawVector TDemo02_03::toRaw() const
{
    B_SERIALIZE_V1(stream)
    stream << value1;
    B_SERIALIZE_RETURN
}

void TDemo02_03::fromRaw(const bserial::RawVector& vect)
{
    B_DESERIALIZE_V1(vect, stream)
    stream >> value1;
    B_DESERIALIZE_END
}

bserial::RawVector TDemo02_03A::toRaw() const
{
    B_SERIALIZE_V1(stream)
    stream << value1;
    stream << value2;
    stream << value3;
    B_SERIALIZE_RETURN
}

void TDemo02_03A::fromRaw(const bserial::RawVector& vect)
{
    B_DESERIALIZE_V1(vect, stream)
    stream >> value1;
    stream >> value2;
    stream >> value3;
    B_DESERIALIZE_END
}

bserial::RawVector TDemo02_05_Error::toRaw() const
{
    B_SERIALIZE_V1(stream)

    // Структуры унаследованные от MessageError сериализуются в особом порядке,
    // поэтому для них сериализация/десериализация базового класса не обязательна
    // stream << B_BASE_CLASS(MessageError);

    stream << extCode;
    B_SERIALIZE_RETURN
}

void TDemo02_05_Error::fromRaw(const bserial::RawVector& vect)
{
    B_DESERIALIZE_V1(vect, stream)

    // Структуры унаследованные от MessageError сериализуются в особом порядке,
    // поэтому для них сериализация/десериализация базового класса не обязательна
    // stream >> B_BASE_CLASS(MessageError);

    stream >> extCode;
    B_DESERIALIZE_END
}

bserial::RawVector TDemo02_06_Failed::toRaw() const
{
    B_SERIALIZE_V1(stream)

    // Структуры унаследованные от MessageFailed сериализуются в особом порядке,
    // поэтому для них сериализация/десериализация базового класса не обязательна
    // stream << B_BASE_CLASS(MessageFailed);

    stream << extCode;
    B_SERIALIZE_RETURN
}

void TDemo02_06_Failed::fromRaw(const bserial::RawVector& vect)
{
    B_DESERIALIZE_V1(vect, stream)

    // Структуры унаследованные от MessageFailed сериализуются в особом порядке,
    // поэтому для них сериализация/десериализация базового класса не обязательна
    // stream >> B_BASE_CLASS(MessageFailed);

    stream >> extCode;
    B_DESERIALIZE_END
}

bserial::RawVector TDemo05_GetData::toRaw() const
{
    B_SERIALIZE_V1(stream)
    stream << chunkCount;
    B_SERIALIZE_RETURN
}

void TDemo05_GetData::fromRaw(const bserial::RawVector& vect)
{
    B_DESERIALIZE_V1(vect, stream)
    stream >> chunkCount;
    B_DESERIALIZE_END
}

bserial::RawVector TDemo05_SendChunk::toRaw() const
{
    B_SERIALIZE_V1(stream)
    stream << index;
    stream << data;
    B_SERIALIZE_RETURN
}

void TDemo05_SendChunk::fromRaw(const bserial::RawVector& vect)
{
    B_DESERIALIZE_V1(vect, stream)
    stream >> index;
    stream >> data;
    B_DESERIALIZE_END
}

bserial::RawVector TDemo05_SendFinish::toRaw() const
{
    B_SERIALIZE_V1(stream)
    stream << crc;
    B_SERIALIZE_RETURN
}

void TDemo05_SendFinish::fromRaw(const bserial::RawVector& vect)
{
    B_DESERIALIZE_V1(vect, stream)
    stream >> crc;
    B_DESERIALIZE_END
}

bserial::RawVector TDemo05_SendError::toRaw() const
{
    B_SERIALIZE_V1(stream)
    stream << errorInfo;
    B_SERIALIZE_RETURN
}

void TDemo05_SendError::fromRaw(const bserial::RawVector& vect)
{
    B_DESERIALIZE_V1(vect, stream)
    stream >> errorInfo;
    B_DESERIALIZE_END
}

} // namespace data
} // namespace pproto
