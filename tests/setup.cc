#include "ccunit.h"

#include <string_view>

int create_test_entry() {
	return 100;
}

void update_test_entry_name(int id, std::string_view name) {
	if(name.empty()) {
		throw 1;
	}
}

void delete_test_entry(int id) {

}

class TempEntry {
	public:
		void setup () {
			id_ = create_test_entry();
		}
		void teardown () {
			delete_test_entry(id_);
		}
		int get_id() {
			return id_;
		}
	private:
		int id_;
};

TEST_EX("Test will run setup and teardown code", int) {
	ccunit::SetupAndTeardown<TempEntry> entry;
	update_test_entry_name(entry.get_id(), "");
}
