#pragma once

#include "core/types.h"
#include <glm/glm.hpp>

namespace ngin {

class Camera {
public:
	Camera(
		u32 viewport_width,
		u32 viewport_height,
		const glm::vec2& position = glm::vec2(0.0f),
		f32 rotation = 0.0f,
		f32 zoom = 1.0f
	);
	~Camera();

	void set_position(const glm::vec2& position);
	void set_rotation(f32 rotation);
	void set_zoom(f32 zoom);

	void set_viewport(u32 viewport_width, u32 viewport_height);

	const glm::vec2& get_position() const;
	f32 get_rotation() const;
	f32 get_zoom() const;

	void move(const glm::vec2& offset);
	void rotate(f32 offset);
	void zoom(f32 offset);

	const glm::mat4& get_projection_matrix() const;
	const glm::mat4& get_view_matrix() const;
	const glm::mat4& get_projection_view_matrix() const;

private:
	void recalculate_matrices();

private:
	glm::vec2 m_position;
	f32 m_rotation;
	f32 m_zoom;
	u32 m_viewport_width;
	u32 m_viewport_height;

	glm::mat4 m_projection;
	glm::mat4 m_view;
	glm::mat4 m_projection_view;
};

} // namespace ngin