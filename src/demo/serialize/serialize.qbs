import qbs
import "serialize_base.qbs" as SerializeBase

Project {
    name: "Serialize"

    SerializeBase {
        name: "SDemo 01"
        targetName: "sdemo01"
        condition: true

        files: [
            "sdemo01/sdemo01.cpp",
        ]
    }


}
