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

    SerializeBase {
        name: "SDemo 06"
        targetName: "sdemo06"
        condition: true

        Depends { name: "RapidJson" }

        files: [
            "sdemo06/sdemo06.cpp",
        ]
    }

    SerializeBase {
        name: "SDemo 07"
        targetName: "sdemo07"
        condition: true

        Depends { name: "RapidJson" }

        files: [
            "sdemo07/sdemo07.cpp",
        ]
    }

    SerializeBase {
        name: "SDemo 08"
        targetName: "sdemo08"
        condition: true

        Depends { name: "RapidJson" }

        files: [
            "sdemo08/sdemo08.cpp",
        ]
    }

    SerializeBase {
        name: "SDemo 09"
        targetName: "sdemo09"
        condition: true

        Depends { name: "RapidJson" }

        files: [
            "sdemo09/sdemo09.cpp",
        ]
    }

    SerializeBase {
        name: "SDemo 10"
        targetName: "sdemo10"
        condition: true

        Depends { name: "RapidJson" }

        files: [
            "sdemo10/sdemo10.cpp",
        ]
    }

    SerializeBase {
        name: "SDemo 11"
        targetName: "sdemo11"
        condition: true

        Depends { name: "RapidJson" }

        files: [
            "sdemo11/sdemo11.cpp",
        ]
    }

    SerializeBase {
        name: "SDemo 12"
        targetName: "sdemo12"
        condition: true

        Depends { name: "protobuf" }
        Depends { name: "RapidJson" }

        cpp.includePaths: [
            protobuf.generatedFilesDir,
        ]

        cpp.dynamicLibraries: [
            "protobuf",
        ]

        files: [
            "sdemo12/addressbook.proto",
            "sdemo12/sdemo12.cpp",
        ]
    }

    SerializeBase {
        name: "SDemo 13"
        targetName: "sdemo13"
        condition: true

        Depends { name: "RapidJson" }

        files: [
            "sdemo13/sdemo13.cpp",
        ]
    }
}
