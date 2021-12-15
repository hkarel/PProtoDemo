import qbs
import "web_base.qbs" as WebBase

Project {
    name: "Web (Clients)"

    WebBase {
        name: "WDemo 01 Client"
        targetName: "wdemo01_client"
        condition: true

        Depends { name: "Qt"; submodules: ["widgets"] }

        files: [
            "wdemo01_client/main_window.cpp",
            "wdemo01_client/main_window.h",
            "wdemo01_client/main_window.ui",
            "wdemo01_client/wdemo01_client.cpp",
            "wdemo01_client/wdemo01_client.qrc",
        ]
    }

}
