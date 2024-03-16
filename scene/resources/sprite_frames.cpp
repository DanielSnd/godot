/**************************************************************************/
/*  sprite_frames.cpp                                                     */
/**************************************************************************/
/*                         This file is part of:                          */
/*                             GODOT ENGINE                               */
/*                        https://godotengine.org                         */
/**************************************************************************/
/* Copyright (c) 2014-present Godot Engine contributors (see AUTHORS.md). */
/* Copyright (c) 2007-2014 Juan Linietsky, Ariel Manzur.                  */
/*                                                                        */
/* Permission is hereby granted, free of charge, to any person obtaining  */
/* a copy of this software and associated documentation files (the        */
/* "Software"), to deal in the Software without restriction, including    */
/* without limitation the rights to use, copy, modify, merge, publish,    */
/* distribute, sublicense, and/or sell copies of the Software, and to     */
/* permit persons to whom the Software is furnished to do so, subject to  */
/* the following conditions:                                              */
/*                                                                        */
/* The above copyright notice and this permission notice shall be         */
/* included in all copies or substantial portions of the Software.        */
/*                                                                        */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,        */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF     */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. */
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY   */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,   */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE      */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                 */
/**************************************************************************/

#include "sprite_frames.h"

#include "scene/scene_string_names.h"

void SpriteFrames::add_frame(const StringName &p_anim, const Ref<Texture2D> &p_texture, float p_duration, int p_callback, int p_at_pos) {
	HashMap<StringName, Anim>::Iterator E = animations.find(p_anim);
	ERR_FAIL_COND_MSG(!E, "Animation '" + String(p_anim) + "' doesn't exist.");

	p_duration = MAX(SPRITE_FRAME_MINIMUM_DURATION, p_duration);

	Frame frame = { p_texture, p_duration, p_callback };

	if (p_at_pos >= 0 && p_at_pos < E->value.frames.size()) {
		E->value.frames.insert(p_at_pos, frame);
	} else {
		E->value.frames.push_back(frame);
	}

	emit_changed();
}

void SpriteFrames::set_frame(const StringName &p_anim, int p_idx, const Ref<Texture2D> &p_texture, float p_duration, int p_callback) {
	HashMap<StringName, Anim>::Iterator E = animations.find(p_anim);
	ERR_FAIL_COND_MSG(!E, "Animation '" + String(p_anim) + "' doesn't exist.");
	ERR_FAIL_COND(p_idx < 0);
	if (p_idx >= E->value.frames.size()) {
		return;
	}

	p_duration = MAX(SPRITE_FRAME_MINIMUM_DURATION, p_duration);

	Frame frame = { p_texture, p_duration, p_callback };

	E->value.frames.write[p_idx] = frame;

	emit_changed();
}

int SpriteFrames::get_frame_count(const StringName &p_anim) const {
	HashMap<StringName, Anim>::ConstIterator E = animations.find(p_anim);
	ERR_FAIL_COND_V_MSG(!E, 0, "Animation '" + String(p_anim) + "' doesn't exist.");

	return E->value.frames.size();
}

void SpriteFrames::remove_frame(const StringName &p_anim, int p_idx) {
	HashMap<StringName, Anim>::Iterator E = animations.find(p_anim);
	ERR_FAIL_COND_MSG(!E, "Animation '" + String(p_anim) + "' doesn't exist.");

	E->value.frames.remove_at(p_idx);

	emit_changed();
}

void SpriteFrames::clear(const StringName &p_anim) {
	HashMap<StringName, Anim>::Iterator E = animations.find(p_anim);
	ERR_FAIL_COND_MSG(!E, "Animation '" + String(p_anim) + "' doesn't exist.");

	E->value.frames.clear();

	emit_changed();
}

void SpriteFrames::clear_all() {
	animations.clear();
	add_animation("default");
}

void SpriteFrames::add_animation(const StringName &p_anim) {
	ERR_FAIL_COND_MSG(animations.has(p_anim), "SpriteFrames already has animation '" + p_anim + "'.");

	animations[p_anim] = Anim();
}

bool SpriteFrames::has_animation(const StringName &p_anim) const {
	return animations.has(p_anim);
}

void SpriteFrames::remove_animation(const StringName &p_anim) {
	animations.erase(p_anim);
}

void SpriteFrames::rename_animation(const StringName &p_prev, const StringName &p_next) {
	ERR_FAIL_COND_MSG(!animations.has(p_prev), "SpriteFrames doesn't have animation '" + String(p_prev) + "'.");
	ERR_FAIL_COND_MSG(animations.has(p_next), "Animation '" + String(p_next) + "' already exists.");

	Anim anim = animations[p_prev];
	animations.erase(p_prev);
	animations[p_next] = anim;
}

void SpriteFrames::get_animation_list(List<StringName> *r_animations) const {
	for (const KeyValue<StringName, Anim> &E : animations) {
		r_animations->push_back(E.key);
	}
}

Vector<String> SpriteFrames::get_animation_names() const {
	Vector<String> names;
	for (const KeyValue<StringName, Anim> &E : animations) {
		names.push_back(E.key);
	}
	names.sort();
	return names;
}


void SpriteFrames::add_callback(const StringName &p_anim) {
	ERR_FAIL_COND_MSG(callbacks.has(p_anim), "SpriteFrames already has callback '" + p_anim + "'.");
	callbacks.append(p_anim);
}

bool SpriteFrames::has_callback(const StringName &p_anim) const {
	return callbacks.has(p_anim);
}

void SpriteFrames::remove_callback(const StringName &p_anim) {
	callbacks.erase(p_anim);
}

void SpriteFrames::rename_callback_index(const int p_prev, const StringName &p_next) {
	ERR_FAIL_COND_MSG(p_prev < 0 || p_prev >= callbacks.size(), vformat("SpriteFrames doesn't have callback %d",p_prev));
	ERR_FAIL_COND_MSG(callbacks.has(p_next), "callback '" + String(p_next) + "' already exists.");
	callbacks.set(p_prev,p_next);
}
void SpriteFrames::rename_callback(const StringName &p_prev, const StringName &p_next) {
	ERR_FAIL_COND_MSG(!callbacks.has(p_prev), "SpriteFrames doesn't have callback '" + String(p_prev) + "'.");
	ERR_FAIL_COND_MSG(callbacks.has(p_next), "callback '" + String(p_next) + "' already exists.");

	for (int i = 0; i < callbacks.size(); ++i) {
		if (callbacks[i] == p_prev) {
			callbacks.set(i,p_next);
			break;
		}
	}
}

int SpriteFrames::get_callback_count() const {
	return callbacks.size();
}

void SpriteFrames::get_callback_list(List<StringName> *r_callbacks) const {
	for (const StringName &E : callbacks) {
		r_callbacks->push_back(E);
	}
}

Vector<String> SpriteFrames::get_callback_names() const {
	Vector<String> names;
	for (const StringName &E : callbacks) {
		names.push_back(E);
	}
	return names;
}

void SpriteFrames::set_animation_speed(const StringName &p_anim, double p_fps) {
	ERR_FAIL_COND_MSG(p_fps < 0, "Animation speed cannot be negative (" + itos(p_fps) + ").");
	HashMap<StringName, Anim>::Iterator E = animations.find(p_anim);
	ERR_FAIL_COND_MSG(!E, "Animation '" + String(p_anim) + "' doesn't exist.");
	E->value.speed = p_fps;
}

double SpriteFrames::get_animation_speed(const StringName &p_anim) const {
	HashMap<StringName, Anim>::ConstIterator E = animations.find(p_anim);
	ERR_FAIL_COND_V_MSG(!E, 0, "Animation '" + String(p_anim) + "' doesn't exist.");
	return E->value.speed;
}

void SpriteFrames::set_animation_loop(const StringName &p_anim, bool p_loop) {
	HashMap<StringName, Anim>::Iterator E = animations.find(p_anim);
	ERR_FAIL_COND_MSG(!E, "Animation '" + String(p_anim) + "' doesn't exist.");
	E->value.loop = p_loop;
}

bool SpriteFrames::get_animation_loop(const StringName &p_anim) const {
	HashMap<StringName, Anim>::ConstIterator E = animations.find(p_anim);
	ERR_FAIL_COND_V_MSG(!E, false, "Animation '" + String(p_anim) + "' doesn't exist.");
	return E->value.loop;
}

void SpriteFrames::set_animation_auto_transition(const StringName &p_anim, int p_auto_transition) {
	HashMap<StringName, Anim>::Iterator E = animations.find(p_anim);
	ERR_FAIL_COND_MSG(!E, "Animation '" + String(p_anim) + "' doesn't exist.");
	E->value.auto_transition = p_auto_transition;
}

int SpriteFrames::get_animation_auto_transition(const StringName &p_anim) const {
	HashMap<StringName, Anim>::ConstIterator E = animations.find(p_anim);
	ERR_FAIL_COND_V_MSG(!E, false, "Animation '" + String(p_anim) + "' doesn't exist.");
	return E->value.auto_transition;
}

Array SpriteFrames::_get_animations() const {
	Array anims;

	List<StringName> sorted_names;
	get_animation_list(&sorted_names);
	sorted_names.sort_custom<StringName::AlphCompare>();

	for (const StringName &anim_name : sorted_names) {
		const Anim &anim = animations[anim_name];
		Dictionary d;
		d["name"] = anim_name;
		d["speed"] = anim.speed;
		d["loop"] = anim.loop;
		d["auto_transition"] = anim.auto_transition;
		Array frames;
		for (int i = 0; i < anim.frames.size(); i++) {
			Dictionary f;
			f["texture"] = anim.frames[i].texture;
			f["duration"] = anim.frames[i].duration;
			if (anim.frames[i].callback != 0)
				f["callback"] = anim.frames[i].callback;
			frames.push_back(f);
		}
		d["frames"] = frames;
		anims.push_back(d);
	}

	return anims;
}

void SpriteFrames::_set_animations(const Array &p_animations) {
	animations.clear();
	for (int i = 0; i < p_animations.size(); i++) {
		Dictionary d = p_animations[i];

		ERR_CONTINUE(!d.has("name"));
		ERR_CONTINUE(!d.has("speed"));
		ERR_CONTINUE(!d.has("loop"));
		ERR_CONTINUE(!d.has("frames"));

		Anim anim;
		anim.speed = d["speed"];
		anim.loop = d["loop"];
		anim.auto_transition = d.has("auto_transition") ? d["auto_transition"] : 0;
		Array frames = d["frames"];
		for (int j = 0; j < frames.size(); j++) {
#ifndef DISABLE_DEPRECATED
			// For compatibility.
			Ref<Resource> res = frames[j];
			if (res.is_valid()) {
				Frame frame = { res, 1.0 };
				anim.frames.push_back(frame);
				continue;
			}
#endif

			Dictionary f = frames[j];

			ERR_CONTINUE(!f.has("texture"));
			ERR_CONTINUE(!f.has("duration"));

			Frame frame = { f["texture"], MAX(SPRITE_FRAME_MINIMUM_DURATION, (float)f["duration"]) , f.has("callback") ? f["callback"] : 0};
			anim.frames.push_back(frame);
		}

		animations[d["name"]] = anim;
	}
}


Array SpriteFrames::_get_callbacks() const {
	Array return_callbacks;
	for (auto callbackname: get_callback_names()) {
		return_callbacks.push_back(callbackname);
	}
	return return_callbacks;
}

void SpriteFrames::_set_callbacks(const Array &p_callbacks) {
	callbacks.clear();
	for (int i = 0; i < p_callbacks.size(); i++) {
		callbacks.push_back(StringName(p_callbacks[i]));
	}
}

#ifdef TOOLS_ENABLED
void SpriteFrames::get_argument_options(const StringName &p_function, int p_idx, List<String> *r_options) const {
	const String pf = p_function;
	if (p_idx == 0) {
		if (pf == "has_animation" || pf == "remove_animation" || pf == "rename_animation" ||
				pf == "set_animation_speed" || pf == "get_animation_speed" ||
				pf == "set_animation_loop" || pf == "get_animation_loop" ||
				pf == "add_frame" || pf == "set_frame" || pf == "remove_frame" ||
				pf == "get_frame_count" || pf == "get_frame_texture" || pf == "get_frame_duration" ||
				pf == "clear") {
			for (const String &E : get_animation_names()) {
				r_options->push_back(E.quote());
			}
		}
	}
	Resource::get_argument_options(p_function, p_idx, r_options);
}
#endif

void SpriteFrames::_bind_methods() {
	ClassDB::bind_method(D_METHOD("add_animation", "anim"), &SpriteFrames::add_animation);
	ClassDB::bind_method(D_METHOD("has_animation", "anim"), &SpriteFrames::has_animation);
	ClassDB::bind_method(D_METHOD("remove_animation", "anim"), &SpriteFrames::remove_animation);
	ClassDB::bind_method(D_METHOD("rename_animation", "anim", "newname"), &SpriteFrames::rename_animation);

	ClassDB::bind_method(D_METHOD("get_animation_names"), &SpriteFrames::get_animation_names);

	ClassDB::bind_method(D_METHOD("set_animation_speed", "anim", "fps"), &SpriteFrames::set_animation_speed);
	ClassDB::bind_method(D_METHOD("get_animation_speed", "anim"), &SpriteFrames::get_animation_speed);

	ClassDB::bind_method(D_METHOD("set_animation_loop", "anim", "loop"), &SpriteFrames::set_animation_loop);
	ClassDB::bind_method(D_METHOD("get_animation_loop", "anim"), &SpriteFrames::get_animation_loop);

	ClassDB::bind_method(D_METHOD("set_animation_auto_transition", "anim", "auto_transition"), &SpriteFrames::set_animation_auto_transition);
	ClassDB::bind_method(D_METHOD("get_animation_auto_transition", "anim"), &SpriteFrames::get_animation_auto_transition);

	ClassDB::bind_method(D_METHOD("add_frame", "anim", "texture", "duration","callback", "at_position"), &SpriteFrames::add_frame, DEFVAL(1.0),DEFVAL(0), DEFVAL(-1));
	ClassDB::bind_method(D_METHOD("set_frame", "anim", "idx", "texture", "duration","callback"), &SpriteFrames::set_frame, DEFVAL(1.0),DEFVAL(0));
	ClassDB::bind_method(D_METHOD("remove_frame", "anim", "idx"), &SpriteFrames::remove_frame);

	ClassDB::bind_method(D_METHOD("get_frame_count", "anim"), &SpriteFrames::get_frame_count);
	ClassDB::bind_method(D_METHOD("get_frame_texture", "anim", "idx"), &SpriteFrames::get_frame_texture);
	ClassDB::bind_method(D_METHOD("get_frame_duration", "anim", "idx"), &SpriteFrames::get_frame_duration);
	ClassDB::bind_method(D_METHOD("get_frame_callback", "anim", "idx"), &SpriteFrames::get_frame_callback);

	ClassDB::bind_method(D_METHOD("clear", "anim"), &SpriteFrames::clear);
	ClassDB::bind_method(D_METHOD("clear_all"), &SpriteFrames::clear_all);

	// `animations` property is for serialization.

	ClassDB::bind_method(D_METHOD("_set_animations", "animations"), &SpriteFrames::_set_animations);
	ClassDB::bind_method(D_METHOD("_get_animations"), &SpriteFrames::_get_animations);
	ClassDB::bind_method(D_METHOD("_set_callbacks", "callbacks"), &SpriteFrames::_set_callbacks);
	ClassDB::bind_method(D_METHOD("_get_callbacks"), &SpriteFrames::_get_callbacks);

	ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "animations", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NO_EDITOR | PROPERTY_USAGE_INTERNAL), "_set_animations", "_get_animations");

	ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "callbacks", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NO_EDITOR | PROPERTY_USAGE_INTERNAL), "_set_callbacks", "_get_callbacks");
}

SpriteFrames::SpriteFrames() {
	add_animation(SceneStringNames::get_singleton()->_default);
}
