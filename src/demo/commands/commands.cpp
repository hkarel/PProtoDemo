#include "commands.h"

#include "pproto/commands_pool.h"

namespace pproto {
namespace command {

#define REGISTRY_COMMAND_SINGLPROC(COMMAND, UUID) \
    const QUuidEx COMMAND = command::Pool::Registry{UUID, #COMMAND, false};

#define REGISTRY_COMMAND_MULTIPROC(COMMAND, UUID) \
    const QUuidEx COMMAND = command::Pool::Registry{UUID, #COMMAND, true};

REGISTRY_COMMAND_SINGLPROC(TDemo01,         "76179ba9-c22d-4959-9c52-e853fbf52db0")
REGISTRY_COMMAND_SINGLPROC(WebPProtoHello,  "b8338344-bec9-4f7d-b8e2-b81a6d4591c7")
REGISTRY_COMMAND_SINGLPROC(WebSpeedTest,    "59cb5357-80bb-4fa4-a15e-4797a535b50d")

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

} // namespace data
} // namespace pproto
