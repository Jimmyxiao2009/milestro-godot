extends Node

## Headless/editor verification entry for the Milestro GDExtension.
## Exit codes: 0 OK, 1 version fail, 2 font fail, 3 render fail, 4 blank raster, 10 extension missing.

const EXPECTED_MAJOR := 1
const EXPECTED_MINOR := 0
const EXPECTED_PATCH := 0
const SAMPLE_TEXT := "Hello 你好 Milestro"
const FONT_FAMILY := "Source Han Sans VF"

func _ready() -> void:
	var log_path := OS.get_environment("MILESTRO_VERIFY_LOG")
	if log_path.is_empty():
		log_path = "user://godot-load-verify.log"

	var lines: PackedStringArray = []
	var code := _run_verify(lines)
	var body := "\n".join(lines) + "\n"
	print(body)

	var f := FileAccess.open(log_path, FileAccess.WRITE)
	if f:
		f.store_string(body)
		f.close()
		print("wrote_log=", log_path)
	else:
		push_warning("could not write log to %s" % log_path)

	# Also try absolute scratch path if provided.
	var scratch_log := OS.get_environment("MILESTRO_SCRATCH_LOG")
	if not scratch_log.is_empty():
		var sf := FileAccess.open(scratch_log, FileAccess.WRITE)
		if sf:
			sf.store_string(body)
			sf.close()

	get_tree().quit(code)


func _run_verify(lines: PackedStringArray) -> int:
	if not ClassDB.class_exists("Milestro"):
		lines.append("FAIL: ClassDB missing Milestro (GDExtension did not load)")
		return 10

	var m = ClassDB.instantiate("Milestro")
	if m == null:
		lines.append("FAIL: could not instantiate Milestro")
		return 10

	var ver: Dictionary = m.get_version()
	lines.append("version=%s" % str(ver))
	if not ver.get("ok", false):
		lines.append("FAIL: get_version not ok")
		return 1
	if int(ver.get("major", -1)) != EXPECTED_MAJOR \
			or int(ver.get("minor", -1)) != EXPECTED_MINOR \
			or int(ver.get("patch", -1)) != EXPECTED_PATCH:
		lines.append("FAIL: version mismatch expected %d.%d.%d" % [EXPECTED_MAJOR, EXPECTED_MINOR, EXPECTED_PATCH])
		return 1
	lines.append("PASS: version matches VERSION file 1.0.0")

	var font_path := _resolve_font_path()
	lines.append("font_path=%s" % font_path)
	if font_path.is_empty() or not FileAccess.file_exists(font_path):
		lines.append("FAIL: font file missing")
		return 2

	var reg: int = m.register_font_from_file(font_path)
	lines.append("register_font_rc=%d" % reg)

	var metrics: Dictionary = m.render_text_metrics(SAMPLE_TEXT, FONT_FAMILY, 36.0, 512, 256)
	lines.append("render_metrics=%s" % str(metrics))
	if not metrics.get("ok", false):
		lines.append("FAIL: render_text_metrics not ok code=%s" % str(metrics.get("code", "?")))
		return 3

	var h := float(metrics.get("height", 0.0))
	var longest := float(metrics.get("longest_line", 0.0))
	var nonzero := int(metrics.get("non_zero_pixels", 0))
	var byte_size := int(metrics.get("byte_size", 0))
	lines.append("height=%.3f longest_line=%.3f non_zero_pixels=%d byte_size=%d" % [h, longest, nonzero, byte_size])

	if h <= 0.0 or longest <= 0.0:
		lines.append("FAIL: zero layout dimensions")
		return 3
	if nonzero <= 0 or byte_size <= 0:
		lines.append("FAIL: blank or empty raster")
		return 4

	var img: Image = metrics.get("image", null)
	if img == null or img.is_empty():
		lines.append("FAIL: image empty")
		return 4

	var png_out := OS.get_environment("MILESTRO_PNG_OUT")
	if not png_out.is_empty():
		var err := img.save_png(png_out)
		lines.append("save_png=%s err=%d" % [png_out, err])

	lines.append("PASS: text render end-to-end")
	lines.append("GODOT_VERIFY_OK")
	return 0


func _resolve_font_path() -> String:
	var env := OS.get_environment("MILESTRO_FONT_PATH")
	if not env.is_empty() and FileAccess.file_exists(env):
		return env
	# Prefer absolute path used by this machine's clone.
	var candidates := [
		"/Users/JimmyXiao/Milestro/tests/data/font/SourceHanSans-VF.otf.woff2.bytes",
		ProjectSettings.globalize_path("res://fonts/SourceHanSans-VF.otf.woff2.bytes"),
	]
	for c in candidates:
		if FileAccess.file_exists(c):
			return c
	return ""
