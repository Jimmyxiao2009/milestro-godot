#include "milestro_unicode_segmenter.h"

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

MilestroUnicodeSegmenter::MilestroUnicodeSegmenter() = default;

MilestroUnicodeSegmenter::~MilestroUnicodeSegmenter() {
	if (native_segmenter) {
		MilestroUnicodeSegmenterDestroy(native_segmenter);
		native_segmenter = nullptr;
	}
}

void MilestroUnicodeSegmenter::_bind_methods() {
	ClassDB::bind_method(D_METHOD("init", "locale", "text"), &MilestroUnicodeSegmenter::init);
	ClassDB::bind_method(D_METHOD("first"), &MilestroUnicodeSegmenter::first);
	ClassDB::bind_method(D_METHOD("next"), &MilestroUnicodeSegmenter::next);
	ClassDB::bind_method(D_METHOD("current"), &MilestroUnicodeSegmenter::current);
	ClassDB::bind_method(D_METHOD("previous"), &MilestroUnicodeSegmenter::previous);
	ClassDB::bind_method(D_METHOD("substring", "start", "len"), &MilestroUnicodeSegmenter::substring);
}

bool MilestroUnicodeSegmenter::init(const String& locale, const String& text) {
	if (locale.is_empty() || text.is_empty()) return false;
	if (native_segmenter) {
		MilestroUnicodeSegmenterDestroy(native_segmenter);
		native_segmenter = nullptr;
	}
	CharString loc = locale.utf8();
	CharString txt = text.utf8();
	int64_t rc = MilestroUnicodeSegmenterCreate(native_segmenter,
			reinterpret_cast<uint8_t*>(const_cast<char*>(loc.get_data())),
			reinterpret_cast<uint8_t*>(const_cast<char*>(txt.get_data())));
	return rc == 0 && native_segmenter != nullptr;
}

int32_t MilestroUnicodeSegmenter::first() {
	if (!native_segmenter) return -1;
	int32_t result = 0;
	MilestroUnicodeSegmenterFirst(native_segmenter, result);
	return result;
}

int32_t MilestroUnicodeSegmenter::next() {
	if (!native_segmenter) return -1;
	int32_t result = 0;
	MilestroUnicodeSegmenterNext(native_segmenter, result);
	return result;
}

int32_t MilestroUnicodeSegmenter::current() {
	if (!native_segmenter) return -1;
	int32_t result = 0;
	MilestroUnicodeSegmenterCurrent(native_segmenter, result);
	return result;
}

int32_t MilestroUnicodeSegmenter::previous() {
	if (!native_segmenter) return -1;
	int32_t result = 0;
	MilestroUnicodeSegmenterPrevious(native_segmenter, result);
	return result;
}

String MilestroUnicodeSegmenter::substring(int32_t start, int32_t len) {
	if (!native_segmenter) return String();
	milestro::game::model::BytesWrapper* wrapper = nullptr;
	int64_t rc = MilestroUnicodeSegmenterSubString(native_segmenter, wrapper, start, len);
	if (rc != 0 || !wrapper) return String();
	return take_bytes(wrapper);
}
