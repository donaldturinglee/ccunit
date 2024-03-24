#ifndef BENCHMARK_H
#define BENCHMARK_H

#include <chrono>
#include <iostream>
#include <string_view>
#include <sstream>
#include <functional>
#include <iomanip>

namespace ccunit {

class BenchmarkBase {
public:
	BenchmarkBase(std::string_view name, std::function<void()> task) : name_(name), task_(std::move(task)) {}
	virtual ~BenchmarkBase() = default;
	void run(std::ostream& os) {
		auto start = std::chrono::high_resolution_clock::now();
		task_();
    	auto end = std::chrono::high_resolution_clock::now();
    	std::chrono::duration<double, std::nano> elapsed_time = end - start;
		std::stringstream ss;
		ss << std::fixed << std::setprecision(2) << elapsed_time.count();
		os << std::left << std::setw(22 - name_.size()) << name_;
		std::string str = ss.str() + " ns";
		os << std::left << str << "\n";
	}
	std::string_view get_name() const {
		return name_;
	}
private:
	std::string name_;
	std::function<void()> task_;
};

inline std::vector<BenchmarkBase*>& get_benchmarks() {
	static std::vector<BenchmarkBase*> benchmarks;
	return benchmarks;
}

inline void run_benchmarks(std::ostream& os) {
	os << "------------------------------------------------------\n";
	os << std::left << std::setw(21) << "Benchmark";
	os << std::left << std::setw(17) << "Time";
	os << "\n";
	os << "------------------------------------------------------\n";
	for(auto* benchmark : get_benchmarks()) {
		benchmark->run(os);
	}
}

} // namespace ccunit

#define CCUNIT_BENCHMARK_CLASS_FINAL(line) Benchmark ## line
#define CCUNIT_BENCHMARK_CLASS_RELAY(line) CCUNIT_BENCHMARK_CLASS_FINAL(line)
#define CCUNIT_BENCHMARK_CLASS CCUNIT_BENCHMARK_CLASS_RELAY(__LINE__)

#define CCUNIT_BENCHMARK_INSTANCE_FINAL(line) benchmark ## line
#define CCUNIT_BENCHMARK_INSTANCE_RELAY(line) CCUNIT_BENCHMARK_INSTANCE_FINAL(line)
#define CCUNIT_BENCHMARK_INSTANCE CCUNIT_BENCHMARK_INSTANCE_RELAY(__LINE__)

#define BENCHMARK(benchmark_name, task) \
class CCUNIT_BENCHMARK_CLASS : public ccunit::BenchmarkBase { \
public: \
	CCUNIT_BENCHMARK_CLASS(std::string_view name, std::function<void()> task) : BenchmarkBase(name, std::move(task)) { \
		ccunit::get_benchmarks().push_back(this); \
	} \
}; \
CCUNIT_BENCHMARK_CLASS CCUNIT_BENCHMARK_INSTANCE(benchmark_name, task); \

#endif // BENCHMARK_H
