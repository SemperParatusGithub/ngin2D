#include "graphics/camera.h"

#include "core/assert.h"

#include <glm/gtc/matrix_transform.hpp>

namespace ngin {

Camera::Camera(
	u32 viewport_width,
	u32 viewport_height,
	const glm::vec2& position,
	f32 rotation,
	f32 zoom
) :
	m_position(position),
	m_rotation(rotation),
	m_zoom(zoom),
	m_viewport_width(viewport_width),
	m_viewport_height(viewport_height)
{
	recalculate_matrices();
}
Camera::~Camera()
{
}

void Camera::set_position(const glm::vec2& position) {
	m_position = position;
	recalculate_matrices();
}
void Camera::set_rotation(f32 rotation) {
	m_rotation = rotation;
	recalculate_matrices();
}
void Camera::set_zoom(f32 zoom) {
	NGIN_ASSERT_MSG(zoom > 0.0f, "Camera zoom must be greater than 0.");
	if (zoom <= 0.0f) {
		return;
	}

	m_zoom = zoom;
	recalculate_matrices();
}

const glm::vec2& Camera::get_position() const {
	return m_position;
}
f32 Camera::get_rotation() const {
	return m_rotation;
}
f32 Camera::get_zoom() const {
	return m_zoom;
}

void Camera::set_viewport(u32 viewport_width, u32 viewport_height) {
	m_viewport_width = viewport_width;
	m_viewport_height = viewport_height;
	recalculate_matrices();
}

void Camera::move(const glm::vec2& offset) {
	m_position += offset;
	recalculate_matrices();
}
void Camera::rotate(f32 offset) {
	m_rotation += offset;
	recalculate_matrices();
}
void Camera::zoom(f32 offset) {
	NGIN_ASSERT_MSG(offset > 0.0f, "Camera zoom multiplier must be greater than 0.");
	if (offset <= 0.0f) {
		return;
	}

	m_zoom *= offset;
	recalculate_matrices();
}

const glm::mat4& Camera::get_projection_matrix() const {
	return m_projection;
}
const glm::mat4& Camera::get_view_matrix() const {
	return m_view;
}
const glm::mat4& Camera::get_projection_view_matrix() const {
	return m_projection_view;
}

void Camera::recalculate_matrices() {
	const f32 half_width = static_cast<f32>(m_viewport_width) * 0.5f;
	const f32 half_height = static_cast<f32>(m_viewport_height) * 0.5f;
	const f32 zoomed_half_width = half_width / m_zoom;
	const f32 zoomed_half_height = half_height / m_zoom;

	m_projection = glm::ortho(
		-zoomed_half_width,
		zoomed_half_width,
		-zoomed_half_height,
		zoomed_half_height,
		-1.0f,
		1.0f
	);

	glm::mat4 transform(1.0f);

	transform = glm::translate(
		transform,
		glm::vec3(m_position, 0.0f)
	);

	transform = glm::rotate(
		transform,
		m_rotation,
		glm::vec3(0, 0, 1)
	);

	m_view = glm::inverse(transform);

	m_projection_view = m_projection * m_view;
}

} // namespace ngin
