import qbs

CppApplication
{
    name: "neuro_net_test_app"

    Depends { name: "Qt"; submodules: ["core","widgets","printsupport"] }

    Depends { name: "neuro_net" }

    cpp.cppFlags: ["-std=c++11"]
    cpp.includePaths: ["./", "../qcustomplot/src/"]
    cpp.staticLibraries: "/home/amewo/projects/qcustomplot/src/libqcustomplot.a"

    files: ["neuro_net_test_app.cpp"]
}
