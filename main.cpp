#include <functional>
#include <iostream>
#include <tuple>
#include <stdexcept>

#include <boost/make_shared.hpp>

template<typename Output, typename Target>
class Queue {
    Target &target;
public:
    Queue(Target &target) : target(target) { }

    void send(Output output) {
	target.process(output);
    }

    void start() {
    }
};

template<typename Output>
class OutputComponent {
public:
    typedef Output OutputType;
    void onOutput(std::function<void(Output)> outputCallback) {
	_outputCallback = outputCallback;
    }
private:
    std::function<void(Output)> _outputCallback;
protected:
    void send(Output output) { _outputCallback(output); }
};

class Reader : public OutputComponent<std::tuple<int, double, int, double>> {
public:
    void start() {
	send(OutputType(3, 3.3, 5, 5.4));
    }
};

class Transform : public OutputComponent<std::tuple<int, double>> {
public:
    void start() { }
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
    void start() { }
};

class Pipeline {
private:
    std::vector<std::function<void()>> commands;

public:
    template<typename Type, typename...Args>
    boost::shared_ptr<Type> create(Args...args) {
	auto component = boost::make_shared<Type>(std::move(args)...);
	commands.push_back([component] {
		component->start();
	    });
	return component;
    }

    template<typename Left, typename Right>
    void connect(boost::shared_ptr<Left> &left, boost::shared_ptr<Right> &right) {
	auto queue = boost::make_shared<Queue<typename Left::OutputType, Right>>(*right);
	left->onOutput([queue, &right](typename Left::OutputType output) {
		queue->send(output);
	    });
	commands.push_back([queue] {
		queue->start();
	    });
    }

    void start() {
	std::for_each(commands.begin(), commands.end(), [] (const std::function<void()> &command) {
		command();
	    });
    }
};

int main() {
    Pipeline pipeline;

    auto reader = pipeline.create<Reader>();
    auto transform = pipeline.create<Transform>();
    auto writer = pipeline.create<Writer>();

    pipeline.connect(reader, transform);
    pipeline.connect(transform, writer);
    pipeline.start();

    return 0;
}
