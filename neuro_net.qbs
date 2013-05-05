import qbs

DynamicLibrary
{
    name: "neuro_net"

    Depends { name: "cpp" }

    cpp.cppFlags: ["-std=c++11"]

    cpp.dynamicLibraries: ["boost_system","protobuf"]

    files: ["neuro_net.h", "neuro_net.cpp",
            "neuron.h", "neuron.cpp",
            "neuron_factory.h", "neuron_factory.cpp",
            "patterns.h", "patterns.cpp",
            "states.pb.h","states.pb.cpp",
            "bars.h","bars.cpp"]
}
