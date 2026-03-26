#pragma once

#include <glm/glm.hpp>

namespace ngin {

class Transform {
public:
	Transform();
	~Transform();

	void set_position(const glm::vec3& position);
	void set_rotation(const glm::vec3& rotation);
	void set_scale(const glm::vec3& scale);

	void get_position(const glm::vec3& position);
	void get_rotation(const glm::vec3& rotation);
	void get_scale(const glm::vec3& scale);

	void move(const glm::vec3& offset);
	void rotate(const glm::vec3& offset);
	void scale(const glm::vec3& offset);

	const glm::mat4& get_transformation_matrix() const;

private:
	void recalculate_matrices();

private:
	glm::vec3 m_position;
	glm::vec3 m_rotation;
	glm::vec3 m_scale;

	glm::mat4 m_transformation_matrix;
};

} // namespace ngin