import qbs
import "pproto_demo_base.qbs" as PProtoDemoBase

PProtoDemoBase {
    name: "PProtoDemo (Transport)"

    references: [
        "src/demo/transport/transport.qbs",
        "src/pproto/pproto.qbs",
        "src/rapidjson/rapidjson.qbs",
        "src/shared/shared.qbs",
        "src/yaml/yaml.qbs",
    ]
}
