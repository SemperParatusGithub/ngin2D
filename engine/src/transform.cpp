#include "transform.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>


namespace ngin {

Transform::Transform() :
	m_position(glm::vec3(0.0f)),
	m_rotation(glm::vec3(0.0f)),
	m_scale(glm::vec3(1.0f)) {
		recalculate_matrices();
	}

Transform::~Transform() = default;

void Transform::set_position(const glm::vec3& position) {
	m_position = position;
	recalculate_matrices();
}
void Transform::set_rotation(const glm::vec3& rotation) {
	m_rotation = rotation;
	recalculate_matrices();
}
void Transform::set_scale(const glm::vec3& scale) {
	m_scale = scale;
	recalculate_matrices();
}

const glm::vec3& Transform::get_position() {
	return m_position;
}
const glm::vec3& Transform::get_rotation() {
	return m_rotation;
}
const glm::vec3& Transform::get_scale() {
	return m_scale;
}

void Transform::move(const glm::vec3& offset) {
	m_position += offset;
	recalculate_matrices();
}
void Transform::rotate(const glm::vec3& offset) {
	m_rotation += offset;
	recalculate_matrices();
}
void Transform::scale(const glm::vec3& offset) {
	m_scale *= offset;
	recalculate_matrices();
}

void Transform::recalculate_matrices() {
	m_transformation_matrix = glm::mat4(1.0f);
	m_transformation_matrix *= glm::translate(glm::mat4(1.0f), m_position);
	if (m_rotation != glm::vec3(0.0f))
		m_transformation_matrix *= glm::toMat4(glm::quat(m_rotation));
	m_transformation_matrix *= glm::scale(glm::mat4(1.0f), m_scale);
}

const glm::mat4& Transform::get_transformation_matrix() const {
	return m_transformation_matrix;
}

} // namespace ngin