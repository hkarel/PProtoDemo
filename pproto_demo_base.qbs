import qbs
import "qbs/imports/QbsUtl/qbsutl.js" as QbsUtl

Project {
    minimumQbsVersion: "1.18.0"
    qbsSearchPaths: ["qbs"]

    property bool useSodium: false       // Использовать собранную libsodium (директория расположения /opt/sodium/1.0.18)
    property bool useSystemSodium: false // Использовать системную libsodium
    property string sodiumVersion: "1.0.18"

    readonly property var projectVersion: projectProbe.projectVersion
    readonly property string projectGitRevision: projectProbe.projectGitRevision

    Probe {
        id: projectProbe
        property var projectVersion;
        property var databaseVersion;
        property string projectGitRevision;

        readonly property string projectBuildDirectory:  project.buildDirectory
        readonly property string projectSourceDirectory: project.sourceDirectory

        configure: {
            projectVersion = QbsUtl.getVersions(projectSourceDirectory + "/VERSION");
            projectGitRevision = QbsUtl.gitRevision(projectSourceDirectory);
        }
    }

    property var cppDefines: {
        var def = [
            "VERSION_PROJECT=\"" + projectVersion[0] + "\"",
            "VERSION_PROJECT_MAJOR=" + projectVersion[1],
            "VERSION_PROJECT_MINOR=" + projectVersion[2],
            "VERSION_PROJECT_PATCH=" + projectVersion[3],
            "GIT_REVISION=\"" + projectGitRevision + "\"",
            "QDATASTREAM_VERSION=QDataStream::Qt_5_12",
          //"QDATASTREAM_BYTEORDER=QDataStream::LittleEndian",
            "PPROTO_VERSION_LOW=0",
            "PPROTO_VERSION_HIGH=0",
            "PPROTO_JSON_SERIALIZE",
            "PPROTO_QBINARY_SERIALIZE",
            "PPROTO_UDP_SIGNATURE=\"PPDM\"",
            "LOGGER_LESS_SNPRINTF"
        ];

        if (qbs.targetOS.contains("windows")
            && qbs.toolchain && qbs.toolchain.contains("mingw"))
        {
            def.push("CONFIG_DIR=\"AppData/pproto-demo\"");
        }
        else
            def.push("CONFIG_DIR=\"/etc/pproto-demo\"");

        if (useSodium === true || useSystemSodium === true)
            def.push("SODIUM_ENCRYPTION");

        return def;
    }

    property var cxxFlags: [
        "-ggdb3",
        "-Wall",
        "-Wextra",
        "-Wno-unused-parameter",
        "-Wno-variadic-macros",
        "-Wno-register",
    ]
    property string cxxLanguageVersion: "c++17"
}
