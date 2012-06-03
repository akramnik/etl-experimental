#include <functional>
#include <iostream>
#include <tuple>
#include <stdexcept>

template<typename Output, typename Target>
class Pipe {
    Target &target;
public:
    Pipe(Target &target) : target(target) { }
    void send(Output output) {
	target.process(output);
    }
};

template<typename Output>
class Component {
public:
    typedef Output OutputType;
    void onOutput(std::function<void(Output)> outputCallback) {
	this->outputCallback = outputCallback;
    }
private:
    std::function<void(Output)> outputCallback;
protected:
    void send(Output output) { outputCallback(output); }
};

class Reader : public Component<std::tuple<int, double, int, double>> {
public:
    void start() {
	send(OutputType(3, 3.3, 5, 5.4));
    }
};

class Transform : public Component<std::tuple<int, double>> {
public:
    void process(std::tuple<int, double, int, double> tuple){
	send(OutputType(std::get<0>(tuple) + std::get<2>(tuple),
			std::get<1>(tuple) + std::get<3>(tuple)));
    }
};

class Writer {
public:
    void process(std::tuple<int, double> tuple) {
	std::cout << std::get<0>(tuple) << ", " << std::get<1>(tuple) << std::endl;
    }
};

int main() {
    Reader reader;
    Transform transform;
    Writer writer;

    Pipe<Reader::OutputType, decltype(transform)> pipe1(transform);
    reader.onOutput([&pipe1](Reader::OutputType output) {
	    pipe1.send(output);
	});

    Pipe<Transform::OutputType, decltype(writer)> pipe2(writer);
    transform.onOutput([&pipe2](Transform::OutputType output) {
	    pipe2.send(output);
	});

    reader.start();

    return 0;
}
