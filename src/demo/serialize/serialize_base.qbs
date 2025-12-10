import qbs
import QbsUtl

Product {
    type: "application"
    consoleApplication: true
    destinationDirectory: "bin"

    Depends { name: "cpp" }
  //Depends { name: "cppstdlib" }
    Depends { name: "PProto" }
    Depends { name: "SharedLib" }
    Depends { name: "Qt"; submodules: ["core"] }

    cpp.defines: project.cppDefines
    cpp.cxxLanguageVersion: project.cxxLanguageVersion

    cpp.rpaths: QbsUtl.concatPaths(
        //cppstdlib.path
        "$ORIGIN/../lib"
    )
}
