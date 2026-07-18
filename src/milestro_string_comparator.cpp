#include "milestro_string_comparator.h"

#include <Milestro/game/milestro_game_interface.h>
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

MilestroStringComparator::MilestroStringComparator() = default;

MilestroStringComparator::~MilestroStringComparator() {
	if (native_comparator) {
		MilestroStringComparatorDestroy(native_comparator);
		native_comparator = nullptr;
	}
}

void MilestroStringComparator::_bind_methods() {
	ClassDB::bind_method(D_METHOD("init", "collation"), &MilestroStringComparator::init);
	ClassDB::bind_method(D_METHOD("compare", "a", "b"), &MilestroStringComparator::compare);
}

bool MilestroStringComparator::init(const String& collation) {
	if (collation.is_empty()) return false;
	if (native_comparator) {
		MilestroStringComparatorDestroy(native_comparator);
		native_comparator = nullptr;
	}
	CharString cs = collation.utf8();
	int64_t rc = MilestroStringComparatorCreate(native_comparator,
			reinterpret_cast<uint8_t*>(const_cast<char*>(cs.get_data())));
	return rc == 0 && native_comparator != nullptr;
}

int32_t MilestroStringComparator::compare(const String& a, const String& b) const {
	if (!native_comparator) return 0;
	CharString a_cs = a.utf8();
	CharString b_cs = b.utf8();
	int32_t result = 0;
	MilestroStringComparatorCompare(native_comparator, result,
			reinterpret_cast<uint8_t*>(const_cast<char*>(a_cs.get_data())),
			reinterpret_cast<uint8_t*>(const_cast<char*>(b_cs.get_data())));
	return result;
}
