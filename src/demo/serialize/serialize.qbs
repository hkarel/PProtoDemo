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

    SerializeBase {
        name: "SDemo 04"
        targetName: "sdemo04"
        condition: true

        files: [
            "sdemo04/sdemo04.cpp",
        ]
    }

    SerializeBase {
        name: "SDemo 05"
        targetName: "sdemo05"
        condition: true

        files: [
            "sdemo05/sdemo05.cpp",
        ]
    }

}
