#include "Transform.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>


namespace ngin {

Transform::Transform() :
	m_position(glm::vec3(0.0f)),
	m_rotation(glm::vec3(0.0f)),
	m_scale(glm::vec3(1.0f)) {
}

void Transform::set_position(const glm::vec3& position) {
	m_position = position;
}
void Transform::set_rotation(const glm::vec3& rotation) {
	m_rotation = rotation;
}
void Transform::set_scale(const glm::vec3& scale) {
	m_scale = scale;
}

void Transform::get_position(const glm::vec3& position) {
	m_position = position;
}
void Transform::get_rotation(const glm::vec3& rotation) {
	m_rotation = rotation;
}
void Transform::get_scale(const glm::vec3& scale) {
	m_scale = scale;
}

void Transform::move(const glm::vec3& offset) {
	m_position += offset;
}
void Transform::rotate(const glm::vec3& offset) {
	m_rotation += offset;
}
void Transform::scale(const glm::vec3& offset) {
	m_scale *= offset;
}

const glm::mat4& Transform::get_transformation_matrix() const {
	glm::mat4 transformation_matrix = glm::mat4(1.0f);
	transformation_matrix *= glm::translate(glm::mat4(1.0f), m_position);
	if (m_rotation != glm::vec3(0.0f))
		transformation_matrix *= glm::toMat4(glm::quat(m_rotation));
	transformation_matrix *= glm::scale(glm::mat4(1.0f), m_scale);

	return transformation_matrix;
}

} // namespace engine