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

}
