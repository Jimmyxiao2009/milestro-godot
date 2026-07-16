extends Node3D
## 星球大战式开场字幕 Demo — 文字由 Milestro (Skia) 栅格化
## 打开 demos/star_crawl/star_crawl_demo.tscn 运行（F6）
## R 重播 · Esc 退出

const CRAWL_W := 800
const CRAWL_H := 2000
const FONT_SIZE := 40.0

# 经典开场句 + 致敬向原创正文（中英混排展示）
const INTRO_LINE := "A long time ago in a galaxy far,\nfar away...."

const CRAWL_BODY := """EPISODE IV
A NEW HOPE

It is a period of civil war.
Rebel spaceships, striking
from a hidden base, have won
their first victory against
the evil Galactic Empire.

During the battle, Rebel
spies managed to steal secret
plans to the Empire's
ultimate weapon, the DEATH
STAR, an armored space
station with enough power
to destroy an entire planet.

Pursued by the Empire's
sinister agents, Princess
Leia races home aboard her
starship, custodian of the
stolen plans that can save
her people and restore
freedom to the galaxy....


很久很久以前
在一个遥远的银河系……

义军自隐秘基地出击，
取得了对银河帝国的
第一场胜利。

间谍窃取了帝国终极武器
「死星」的设计图——
那座足以摧毁整颗行星
的装甲空间站。

莱娅公主携计划星舰归乡，
身后是帝国爪牙的穷追。

愿原力与你同在。
May the Force be with you.
"""

@onready var crawl_mesh: MeshInstance3D = $CrawlPivot/CrawlPlane
@onready var crawl_pivot: Node3D = $CrawlPivot
@onready var intro_label: Label = $UI/IntroLabel
@onready var logo_label: Label = $UI/LogoLabel
@onready var status_label: Label = $UI/StatusLabel
@onready var dimmer: ColorRect = $UI/Dimmer

var _phase := "boot"
var _t := 0.0
var _crawl_ready := false
var _shader_mat: ShaderMaterial


func _ready() -> void:
	intro_label.modulate.a = 0.0
	logo_label.modulate.a = 0.0
	logo_label.visible = false
	crawl_pivot.visible = false
	dimmer.visible = true
	dimmer.color = Color(0, 0, 0, 1)
	status_label.text = "Milestro 正在渲染字幕…"

	# 黑底星空：关掉全屏 UI 挡板留给 3D（intro 阶段用 dimmer）
	_build_crawl_texture.call_deferred()


func _build_crawl_texture() -> void:
	if not ClassDB.class_exists("Milestro"):
		status_label.text = "FAIL: 未加载 Milestro 插件"
		return

	var m = ClassDB.instantiate("Milestro")
	if m == null:
		status_label.text = "FAIL: 无法创建 Milestro"
		return

	var cjk := _first_existing([
		OS.get_environment("MILESTRO_FONT_PATH"),
		"/Users/JimmyXiao/Milestro/tests/data/font/SourceHanSans-VF.otf.woff2.bytes",
	])
	var emoji := _first_existing([
		OS.get_environment("MILESTRO_EMOJI_FONT_PATH"),
		"/Users/JimmyXiao/Milestro/tests/data/font/NotoColorEmoji.ttf.bytes",
	])
	if not cjk.is_empty():
		m.register_font_from_file(cjk)
	if not emoji.is_empty():
		m.register_font_from_file(emoji)

	var families := PackedStringArray(["Source Han Sans VF", "Noto Color Emoji", "sans-serif"])
	var metrics: Dictionary = m.render_mixed(
		CRAWL_BODY,
		families,
		FONT_SIZE,
		CRAWL_W,
		CRAWL_H,
		PackedStringArray(),
		PackedVector2Array(),
		PackedFloat32Array()
	)

	if not metrics.get("ok", false):
		status_label.text = "FAIL: render_mixed code=%s" % str(metrics.get("code", "?"))
		return

	var img: Image = metrics.get("image", null)
	if img == null or img.is_empty():
		status_label.text = "FAIL: empty image"
		return

	var tex := ImageTexture.create_from_image(img)

	var shader: Shader = load("res://demos/star_crawl/crawl_gold.gdshader")
	_shader_mat = ShaderMaterial.new()
	_shader_mat.shader = shader
	_shader_mat.set_shader_parameter("crawl_tex", tex)
	_shader_mat.set_shader_parameter("gold_color", Color(1.0, 0.91, 0.12))
	crawl_mesh.material_override = _shader_mat

	var quad := QuadMesh.new()
	# 高长卷轴
	var world_w := 12.0
	var world_h := world_w * float(CRAWL_H) / float(CRAWL_W)
	quad.size = Vector2(world_w, world_h)
	crawl_mesh.mesh = quad

	_crawl_ready = true
	status_label.text = ""
	_phase = "intro_wait"
	_t = 0.0
	print("[star_crawl] ready non_zero=", metrics.get("non_zero_pixels", 0))


func _process(delta: float) -> void:
	if not _crawl_ready and _phase != "boot":
		return
	_t += delta

	match _phase:
		"boot":
			pass
		"intro_wait":
			if _t > 0.5:
				_phase = "intro"
				_t = 0.0
		"intro":
			intro_label.text = INTRO_LINE
			intro_label.modulate = Color(0.45, 0.68, 1.0, clampf(_t / 1.4, 0.0, 1.0))
			if _t > 4.2:
				_phase = "intro_out"
				_t = 0.0
		"intro_out":
			intro_label.modulate.a = clampf(1.0 - _t / 0.9, 0.0, 1.0)
			if _t > 1.0:
				intro_label.visible = false
				_phase = "logo"
				_t = 0.0
		"logo":
			logo_label.visible = true
			var u := clampf(_t / 2.8, 0.0, 1.0)
			var s := lerpf(2.4, 0.9, u)
			logo_label.scale = Vector2(s, s)
			var fade_in := clampf(_t / 0.35, 0.0, 1.0)
			var fade_out := clampf((3.0 - _t) / 0.5, 0.0, 1.0)
			logo_label.modulate.a = fade_in * fade_out
			if _t > 3.2:
				logo_label.visible = false
				dimmer.visible = false  # 露出 3D 星空 + crawl
				crawl_pivot.visible = true
				crawl_pivot.position = Vector3(0, 6.0, -6.0)
				_phase = "crawl"
				_t = 0.0
		"crawl":
			# 沿倾斜轴「飞向远方」
			var speed := 1.05
			crawl_pivot.position.y = 6.0 - _t * speed
			crawl_pivot.position.z = -6.0 - _t * 0.35
			if _t > 48.0:
				_phase = "end"
				_t = 0.0
		"end":
			status_label.text = "播放结束 — 按 R 重播"
			if _t > 1.0 and Input.is_action_just_pressed("ui_accept"):
				get_tree().reload_current_scene()


func _unhandled_input(event: InputEvent) -> void:
	if event.is_action_pressed("ui_cancel"):
		get_tree().quit()
	if event is InputEventKey and event.pressed and not event.echo:
		if event.keycode == KEY_R:
			get_tree().reload_current_scene()


func _first_existing(paths: Array) -> String:
	for p in paths:
		var s := str(p)
		if not s.is_empty() and FileAccess.file_exists(s):
			return s
	return ""
