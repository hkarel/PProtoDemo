import qbs
import "pproto_demo_base.qbs" as PProtoDemoBase

PProtoDemoBase {
    name: "PProtoDemo (Serialize)"

    references: [
        "src/demo/serialize/serialize.qbs",
        "src/pproto/pproto.qbs",
        "src/rapidjson/rapidjson.qbs",
        "src/shared/shared.qbs",
        "src/yaml/yaml.qbs",
    ]
}
