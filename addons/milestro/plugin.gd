@tool
extends EditorPlugin

## Editor plugin shell for the Milestro GDExtension.
## The real runtime lives in milestro.gdextension (+ bin/*.dylib).
## This entry makes "Milestro" appear under Project → Project Settings → Plugins.

func _enter_tree() -> void:
	print("Milestro plugin enabled (GDExtension provides class Milestro)")


func _exit_tree() -> void:
	pass
