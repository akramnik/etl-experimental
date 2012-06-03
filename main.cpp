#include <functional>
#include <iostream>
#include <tuple>
#include <stdexcept>

template<typename Output, typename Target>
class Queue {
    Target &target;
public:
    Queue(Target &target) : target(target) { }
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

class Pipeline {
public:
    template<typename Left, typename Right>
    void connect(Left &left, Right &right) {
	Queue<typename Left::OutputType, decltype(right)> queue(right);
	left.onOutput([&queue, &right](typename Left::OutputType output) {
		queue.send(output);
	    });
    }
};

int main() {
    Reader reader;
    Transform transform;
    Writer writer;

    Pipeline pipeline;
    pipeline.connect(reader, transform);
    pipeline.connect(transform, writer);
    reader.start();

    return 0;
}
