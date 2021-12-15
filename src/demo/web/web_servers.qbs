import qbs
import "web_base.qbs" as WebBase

Project {
    name: "Web (Servers)"

    WebBase {
        name: "WDemo 01 Server"
        targetName: "wdemo01_server"
        condition: true

        Depends { name: "Qt"; submodules: ["widgets"] }

        files: [
            "wdemo01_server/wdemo01_serv.cpp",
            "wdemo01_server/wdemo01_serv.qrc",
            "wdemo01_server/wdemo01_serv_appl.cpp",
            "wdemo01_server/wdemo01_serv_appl.h",
        ]
    }
}
