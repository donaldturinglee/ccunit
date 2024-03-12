#include "ccunit.h"

#include <string>
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

std::string create_test_table() {
	return "test_data_01";
}

void drop_test_table(std::string_view name) {

}

class TempTable {
public:
	void setup() {
		name_ = create_test_table();
	}
	void teardown() {
		drop_test_table(name_);
	}
	std::string get_table_name() {
		return name_;
	}
private:
	std::string name_;
};

TEST_EX("Test will run setup and teardown code", int) {
	ccunit::SetupAndTeardown<TempEntry> entry;
	update_test_entry_name(entry.get_id(), "");
}

TEST("Test will run multiple setup and teardown code") {
	ccunit::SetupAndTeardown<TempEntry> entry1;
	ccunit::SetupAndTeardown<TempEntry> entry2;
	update_test_entry_name(entry1.get_id(), "abc");
	update_test_entry_name(entry2.get_id(), "def");
}

ccunit::TestSuiteSetupAndTeardown<TempTable>
global_table1("Test suite setup/teardown 1", "Suite 1");

ccunit::TestSuiteSetupAndTeardown<TempTable>
global_table2("Test suite setup/teardown 2", "Suite 2");

TEST_SUITE("Test part 1 of suite", "Suite 1") {
	CONFIRM("test_data_01", global_table1.get_table_name());
	CONFIRM("test_data_01", global_table2.get_table_name());
}

TEST_SUITE_EX("Test part 2 of suite", "Suite 1", int) {
	throw 1;
}
