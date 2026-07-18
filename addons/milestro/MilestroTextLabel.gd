@tool
extends TextureRect
class_name MilestroTextLabel

## A small Godot-facing control backed by Milestro's native paragraph rasterizer.
## It is intentionally a TextureRect so it works in CanvasItem, Control and 3D UI flows.

@export_multiline var text: String = ""
@export var font_family: String = "sans-serif"
@export_range(1.0, 512.0, 0.5) var font_size: float = 32.0
@export var font_path: String = ""
@export var canvas_size: Vector2i = Vector2i(640, 160)
@export var auto_render: bool = true

var _milestro: Milestro
var _last_signature: String = ""

func _ready() -> void:
	stretch_mode = TextureRect.STRETCH_KEEP_ASPECT_CENTERED
	if auto_render:
		render()

func _process(_delta: float) -> void:
	if Engine.is_editor_hint() and not auto_render:
		return
	if auto_render:
		var signature := "%s|%s|%s|%s|%s" % [text, font_family, font_size, font_path, canvas_size]
		if signature != _last_signature:
			render()

func render() -> bool:
	if text.is_empty() or canvas_size.x <= 0 or canvas_size.y <= 0:
		texture = null
		return false
	if not ClassDB.class_exists("Milestro"):
		return false

	if _milestro == null:
		_milestro = Milestro.new()
	if not font_path.is_empty():
		var absolute_path := ProjectSettings.globalize_path(font_path) if font_path.begins_with("res://") else font_path
		if FileAccess.file_exists(absolute_path):
			_milestro.register_font_from_file(absolute_path)

	var metrics: Dictionary = _milestro.render_text_metrics(text, font_family, font_size, canvas_size.x, canvas_size.y)
	if not metrics.get("ok", false):
		texture = null
		return false
	var image: Image = metrics.get("image", null)
	if image == null or image.is_empty():
		texture = null
		return false
	texture = ImageTexture.create_from_image(image)
	_last_signature = "%s|%s|%s|%s|%s" % [text, font_family, font_size, font_path, canvas_size]
	return true
