extends Control
class_name MilestroInputBoxControl

## Godot Control adapter for the native Milestro InputBox editing model.
## Rendering is kept in an ImageTexture so the native Skia canvas remains isolated
## from Godot's scene tree and input dispatch.

@export var font_family: String = "sans-serif"
@export_range(1.0, 256.0, 0.5) var font_size: float = 24.0
@export var font_path: String = ""
@export var initial_text: String = ""

var _milestro: Milestro
var _input_box: MilestroInputBox
var _texture: ImageTexture

func _ready() -> void:
	focus_mode = Control.FOCUS_ALL
	focus_entered.connect(_on_focus_entered)
	focus_exited.connect(_on_focus_exited)
	_milestro = Milestro.new()
	if not font_path.is_empty():
		var absolute_path: String = ProjectSettings.globalize_path(font_path) if font_path.begins_with("res://") else font_path
		if FileAccess.file_exists(absolute_path):
			_milestro.register_font_from_file(absolute_path)
	var text_style = _milestro.create_text_style()
	text_style.set_font_size(font_size)
	text_style.set_font_families(PackedStringArray([font_family]))
	text_style.set_color(32, 32, 36, 255)
	var para_style = _milestro.create_paragraph_style()
	para_style.set_text_style(text_style)
	_input_box = _milestro.create_input_box(para_style, text_style)
	if _input_box == null:
		return
	_input_box.set_text(initial_text)
	_input_box.set_viewport(size.x, size.y)
	_input_box.set_focused(has_focus())
	_input_box.set_caret_visible(has_focus())
	_redraw_native()

func _notification(what: int) -> void:
	if what == NOTIFICATION_RESIZED and _input_box != null:
		_input_box.set_viewport(size.x, size.y)
		_redraw_native()

func _on_focus_entered() -> void:
	if _input_box != null:
		_input_box.set_focused(true)
		_input_box.set_caret_visible(true)
		_redraw_native()

func _on_focus_exited() -> void:
	if _input_box != null:
		_input_box.set_focused(false)
		_input_box.set_caret_visible(false)
		_redraw_native()

func _gui_input(event: InputEvent) -> void:
	if _input_box == null:
		return
	if event is InputEventMouseButton and event.button_index == MOUSE_BUTTON_LEFT and event.pressed:
		grab_focus()
		_input_box.hit_test(event.position.x, event.position.y)
		_redraw_native()
		accept_event()
		return
	if event is InputEventKey and event.pressed and not event.echo:
		var changed := false
		match event.keycode:
			KEY_BACKSPACE:
				changed = _input_box.delete_backward()
			KEY_DELETE:
				changed = _input_box.delete_forward()
			KEY_LEFT:
				changed = _input_box.move_previous()
			KEY_RIGHT:
				changed = _input_box.move_next()
			KEY_HOME:
				changed = _input_box.move_line_start_extending_selection(event.shift_pressed)
			KEY_END:
				changed = _input_box.move_line_end_extending_selection(event.shift_pressed)
			KEY_ENTER, KEY_KP_ENTER:
				changed = _input_box.insert_text("\n")
			_:
				if event.unicode > 0 and not event.ctrl_pressed and not event.meta_pressed:
					changed = _input_box.insert_text(String.chr(event.unicode))
		if changed:
			_redraw_native()
			accept_event()

func _redraw_native() -> void:
	if _input_box == null or size.x <= 0.0 or size.y <= 0.0:
		return
	var canvas = _milestro.create_canvas(ceili(size.x), ceili(size.y))
	var snapshot = _input_box.create_draw_snapshot()
	if canvas == null or snapshot == null:
		return
	if not snapshot.paint(canvas, 0.0, 0.0, size.x, size.y):
		return
	var pixels: PackedByteArray = canvas.get_texture()
	var image := Image.create_from_data(ceili(size.x), ceili(size.y), false, Image.FORMAT_RGBA8, pixels)
	_texture = ImageTexture.create_from_image(image)
	queue_redraw()

func _draw() -> void:
	if _texture != null:
		draw_texture_rect(_texture, Rect2(Vector2.ZERO, size), false)
