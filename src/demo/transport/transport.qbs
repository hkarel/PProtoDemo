import qbs
import "transport_base.qbs" as TransportBase

Project {
    name: "Transport"

    TransportBase {
        name: "TDemo 01"
        targetName: "tdemo01"
        condition: true

        files: [
            "tdemo01/tdemo01.cpp",
        ]
    }

    TransportBase {
        name: "TDemo 02"
        targetName: "tdemo02"
        condition: true

        files: [
            "tdemo02/tdemo02.cpp",
            "tdemo02/tdemo02_appl.cpp",
            "tdemo02/tdemo02_appl.h",
        ]
    }

    TransportBase {
        name: "TDemo 03"
        targetName: "tdemo03_server"
        condition: true

        files: [
            "tdemo03/server/tdemo03_serv.cpp",
            "tdemo03/server/tdemo03_serv_appl.cpp",
            "tdemo03/server/tdemo03_serv_appl.h",
        ]
    }

    TransportBase {
        name: "TDemo 04"
        targetName: "tdemo04_client"
        condition: true

        Depends { name: "Qt"; submodules: ["widgets"] }

        files: [
            "tdemo04/client/main_window.cpp",
            "tdemo04/client/main_window.h",
            "tdemo04/client/main_window.ui",
            "tdemo04/client/tdemo04_client.cpp",
        ]
    }


}
