#include "camera.h"

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
	m_zoom(zoom)
{
	m_projection = glm::ortho(
		0.0f,
		static_cast<f32>(viewport_width),
		0.0f,
		static_cast<f32>(viewport_height),
		-1.0f,
		1.0f
	);
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
	m_projection = glm::ortho(
		0.0f,
		static_cast<f32>(viewport_width),
		0.0f,
		static_cast<f32>(viewport_height),
		-1.0f,
		1.0f
	);
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

	transform = glm::scale(
		transform,
		glm::vec3(m_zoom, m_zoom, 1.0f)
	);

	m_view = glm::inverse(transform);

	m_projection_view = m_projection * m_view;
}

} // namespace ngin