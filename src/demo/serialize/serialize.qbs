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

    SerializeBase {
        name: "SDemo 02"
        targetName: "sdemo02"
        condition: true

        files: [
            "sdemo02/sdemo02.cpp",
        ]
    }

    SerializeBase {
        name: "SDemo 03"
        targetName: "sdemo03"
        condition: true

        files: [
            "sdemo03/sdemo03.cpp",
        ]
    }

}
