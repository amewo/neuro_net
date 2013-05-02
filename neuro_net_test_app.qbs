import qbs

CppApplication
{
    name: "neuro_net_test_app"

    Depends { name: "neuro_net" }

    cpp.cppFlags: ["-std=c++11"]
    cpp.includePaths: ["./"]

    files: ["neuro_net_test_app.cpp"]
}

