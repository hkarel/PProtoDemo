import qbs
import QbsUtl
import ProbExt

Product {
    type: "application"
    consoleApplication: true
    destinationDirectory: "bin"

    Depends { name: "cpp" }
  //Depends { name: "cppstdlib" }
    Depends { name: "lib.sodium" }
    Depends { name: "Commands" }
    Depends { name: "PProto" }
    Depends { name: "RapidJson" }
    Depends { name: "SharedLib" }
    Depends { name: "Yaml" }
    Depends { name: "Qt"; submodules: ["core", "network"] }

    lib.sodium.enabled: project.useSodium
    lib.sodium.useSystem: project.useSystemSodium
    lib.sodium.version: project.sodiumVersion
    lib.sodium.dynamicLibraries: project.useSodium || project.useSystemSodium ? original : []

    ProbExt.LibValidationProbe {
        id: libValidationProbe
        checkingLibs: [
            lib.sodium
        ]
    }

    cpp.defines: project.cppDefines
    cpp.cxxLanguageVersion: project.cxxLanguageVersion

    cpp.rpaths: QbsUtl.concatPaths(
        //cppstdlib.path
        lib.sodium.libraryPath
       ,"$ORIGIN/../lib"
    )

    cpp.libraryPaths: QbsUtl.concatPaths(
        lib.sodium.libraryPath
    )

    cpp.dynamicLibraries: QbsUtl.concatPaths(
        lib.sodium.dynamicLibraries
    )
}
