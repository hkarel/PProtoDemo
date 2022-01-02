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

}
