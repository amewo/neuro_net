import qbs

DynamicLibrary
{
    name: "neuro_net"

    Depends { name: "cpp" }

    cpp.cppFlags: ["-std=c++11"]

    files: ["neuro_net.h", "neuro_net.cpp",
            "neuron.h","neuron.cpp"]
}
