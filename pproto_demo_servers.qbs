import qbs
import "pproto_demo_base.qbs" as PProtoDemoBase

PProtoDemoBase {
    name: "PProtoDemo (Servers)"

    references: [
        "src/demo/commands/commands.qbs",
        "src/demo/web/web_servers.qbs",
        "src/pproto/pproto.qbs",
        "src/rapidjson/rapidjson.qbs",
        "src/shared/shared.qbs",
        "src/yaml/yaml.qbs",
    ]
}
