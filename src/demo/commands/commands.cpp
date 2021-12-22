#include "commands.h"

#include "pproto/commands_pool.h"

namespace pproto {
namespace command {

#define REGISTRY_COMMAND_SINGLPROC(COMMAND, UUID) \
    const QUuidEx COMMAND = command::Pool::Registry{UUID, #COMMAND, false};

#define REGISTRY_COMMAND_MULTIPROC(COMMAND, UUID) \
    const QUuidEx COMMAND = command::Pool::Registry{UUID, #COMMAND, true};

REGISTRY_COMMAND_SINGLPROC(WebPProtoHello,  "b8338344-bec9-4f7d-b8e2-b81a6d4591c7")
REGISTRY_COMMAND_SINGLPROC(WebSpeedTest,    "59cb5357-80bb-4fa4-a15e-4797a535b50d")

#undef REGISTRY_COMMAND_SINGLPROC
#undef REGISTRY_COMMAND_MULTIPROC

} // namespace command

namespace data {

} // namespace data
} // namespace pproto
