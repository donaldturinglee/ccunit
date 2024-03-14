#include "ccunit.h"

#include <atomic>
#include <thread>
#include <vector>

using namespace ccunit;

TEST("Test can use additional thread") {
	std::vector<ThreadConfirmException> thread_exs(2);
	std::atomic<int> count{0};
	std::vector<std::thread> threads;
	for(int c = 0; c < 2; ++c) {
		threads.emplace_back([&thread_ex = thread_exs.at(c), &count]() {
			try {
				for(int i = 0; i < 100'000; ++i) {
					++count;
				}
				CONFIRM_EQ(count, Equals(200'001));
			} catch(ConfirmException const& ex) {
				thread_ex.set_failure(ex.get_line(), ex.get_reason());
			}
		});
	}

	for(auto& t : threads) {
		t.join();
	}
	for(auto const& ex : thread_exs) {
		ex.get_check_failure();
	}
	CONFIRM_EQ(count, Equals(200'000));
}
