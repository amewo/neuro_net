import qbs

DynamicLibrary
{
    name: "neuro_net"

    Depends { name: "cpp" }

    cpp.cppFlags: ["-std=c++11"]

    cpp.dynamicLibraries: ["boost_system"]

    files: ["neuro_net.h", "neuro_net.cpp",
            "neuron.h", "neuron.cpp",
            "neuron_factory.h", "neuron_factory.cpp"]
}
