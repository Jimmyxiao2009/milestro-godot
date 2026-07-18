#include "milestro_unicode_normalizer.h"

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

MilestroUnicodeNormalizer::MilestroUnicodeNormalizer() = default;

MilestroUnicodeNormalizer::~MilestroUnicodeNormalizer() {
	if (native_normalizer) {
		MilestroUnicodeNormalizerDestroy(native_normalizer);
		native_normalizer = nullptr;
	}
}

void MilestroUnicodeNormalizer::_bind_methods() {
	ClassDB::bind_method(D_METHOD("init", "name", "mode"), &MilestroUnicodeNormalizer::init);
	ClassDB::bind_method(D_METHOD("normalize", "text"), &MilestroUnicodeNormalizer::normalize);
}

bool MilestroUnicodeNormalizer::init(const String& name, int32_t mode) {
	if (name.is_empty()) return false;
	if (native_normalizer) {
		MilestroUnicodeNormalizerDestroy(native_normalizer);
		native_normalizer = nullptr;
	}
	// ICU expects canonical names such as "nfc"; accept the documented
	// uppercase spellings from Godot callers as well.
	CharString cs = name.to_lower().utf8();
	int64_t rc = MilestroUnicodeNormalizerCreate(native_normalizer,
			reinterpret_cast<uint8_t*>(const_cast<char*>(cs.get_data())), mode);
	return rc == 0 && native_normalizer != nullptr;
}

String MilestroUnicodeNormalizer::normalize(const String& text) const {
	if (!native_normalizer || text.is_empty()) return String();
	CharString cs = text.utf8();
	milestro::game::model::BytesWrapper* wrapper = nullptr;
	int64_t rc = MilestroUnicodeNormalizerNormalize(native_normalizer, wrapper,
			reinterpret_cast<uint8_t*>(const_cast<char*>(cs.get_data())));
	if (rc != 0 || !wrapper) return String();
	return take_bytes(wrapper);
}
