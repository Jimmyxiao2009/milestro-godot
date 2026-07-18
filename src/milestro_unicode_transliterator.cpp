#include "milestro_unicode_transliterator.h"

#include <Milestro/game/milestro_game_interface.h>
#include <Milestro/game/milestro_game_model.h>
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

namespace {
String take_bytes(milestro::game::model::BytesWrapper *wrapper) {
	if (!wrapper) return String();
	uint8_t *ptr = nullptr;
	uint64_t size = 0;
	const int64_t rc = MilestroGameModelBytesWrapperCStr(wrapper, ptr, size);
	String result;
	if (rc == 0 && ptr && size > 0) result = String::utf8(reinterpret_cast<const char *>(ptr), static_cast<int>(size));
	milestro::game::model::DataEnvelop *envelop = wrapper;
	MilestroGameModelDataEnvelopDestroy(envelop);
	return result;
}
}

MilestroUnicodeTransliterator::MilestroUnicodeTransliterator() = default;

MilestroUnicodeTransliterator::~MilestroUnicodeTransliterator() {
	if (native_transliterator) {
		MilestroUnicodeTransliteratorDestroy(native_transliterator);
		native_transliterator = nullptr;
	}
}

void MilestroUnicodeTransliterator::_bind_methods() {
	ClassDB::bind_method(D_METHOD("init", "id", "direction"), &MilestroUnicodeTransliterator::init);
	ClassDB::bind_method(D_METHOD("transliterate", "text"), &MilestroUnicodeTransliterator::transliterate);
}

bool MilestroUnicodeTransliterator::init(const String& id, int32_t direction) {
	if (id.is_empty()) return false;
	if (native_transliterator) {
		MilestroUnicodeTransliteratorDestroy(native_transliterator);
		native_transliterator = nullptr;
	}
	CharString cs = id.utf8();
	int64_t rc = MilestroUnicodeTransliteratorCreate(native_transliterator,
			reinterpret_cast<uint8_t*>(const_cast<char*>(cs.get_data())), direction);
	return rc == 0 && native_transliterator != nullptr;
}

String MilestroUnicodeTransliterator::transliterate(const String& text) const {
	if (!native_transliterator || text.is_empty()) return String();
	CharString cs = text.utf8();
	milestro::game::model::BytesWrapper* wrapper = nullptr;
	int64_t rc = MilestroUnicodeTransliteratorTransliterate(native_transliterator, wrapper,
			reinterpret_cast<uint8_t*>(const_cast<char*>(cs.get_data())));
	if (rc != 0 || !wrapper) return String();
	return take_bytes(wrapper);
}
