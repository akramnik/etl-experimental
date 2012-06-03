#include <iostream>
#include <tuple>

template<typename Output, typename Target>
class Pipe {
    Target target;
public:
    Pipe(Target &target) : target(target) { }

    void send(Output output) {
	target.process(output);
    }
};

template<class Target>
class Reader {
public:
    typedef std::tuple<int, double, int, double> OutputType;
    typedef Pipe<OutputType, Target> OutputPipe;

    void start(OutputPipe &pipe) {
	pipe.send(OutputType(3, 3.5, 5, 5.5));
    }
};

template<class Target>
class Transform {
public:
    typedef std::tuple<int, double> OutputType;
    typedef Pipe<OutputType, Target> OutputPipe;

    void process(std::tuple<int, double, int, double> tuple){
	auto result = OutputType(std::get<0>(tuple) + std::get<2>(tuple),
				 std::get<1>(tuple) + std::get<3>(tuple));
//	pipe.send(result);
    }
};

template <typename Input>
class Writer {
    void process(std::tuple<int, double> tuple) {
    }
};

int main() {
    Writer<std::tuple<int, double>> writer;
    Transform<decltype(writer)> transform;
    Reader<decltype(transform)> reader;

    Pipe<Transform<decltype(writer)>::OutputType, decltype(writer)> pipe1(writer);
    Pipe<Reader<decltype(transform)>::OutputType, decltype(transform)> pipe2(transform);

    reader.start(pipe2);

    return 0;
}
