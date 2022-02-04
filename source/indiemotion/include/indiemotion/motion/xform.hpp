#pragma once
#include <indiemotion/common.hpp>

namespace indiemotion {
    struct Vector3 {
        double x = 0.0f;
        double y = 0.0f;
        double z = 0.0f;

        Vector3() {}
        Vector3(const Vector3 &other) : x(other.x), y(other.y), z(other.z) {}
        Vector3(Vector3 &&other) : x(std::move(other.x)), y(std::move(other.y)), z(std::move(other.z)) {}

        Vector3 &operator=(Vector3 &&rhs) {
            x = std::move(rhs.x);
            y = std::move(rhs.y);
            z = std::move(rhs.z);

            return *this;
        }

        Vector3 &operator=(const Vector3 &rhs) {
            x = rhs.x;
            y = rhs.y;
            z = rhs.z;

            return *this;
        }

        bool operator==(const Vector3 &rhs) const {
            return x == rhs.x && y == rhs.y && z == rhs.z;
        }

        static Vector3 zero() {
            auto ctn = Vector3::create(0, 0, 0);
            return ctn;
        }

        static Vector3 create(double x, double y, double z) {
            auto ctn = Vector3();
            ctn.x = x;
            ctn.y = y;
            ctn.z = z;
            return std::move(ctn);
        }
    };

    /**
     * @brief Transformation Data
     *
     */
    struct MotionXForm {
        Vector3 translation;
        Vector3 orientation;

        MotionXForm() {
            translation = Vector3::zero();
            orientation = Vector3::zero();
        }

        MotionXForm(const MotionXForm &other) {
            translation = other.translation;
            orientation = other.orientation;
        }

        MotionXForm &operator=(const MotionXForm &rhs) {
            translation = rhs.translation;
            orientation = rhs.orientation;
            return *this;
        }

        MotionXForm(MotionXForm &&other) {
            swap(std::move(other));
        }

        MotionXForm &operator=(MotionXForm &&rhs) {
            swap(std::move(rhs));
            return *this;
        }

        bool operator==(const MotionXForm &rhs) const {
            return translation == rhs.translation && orientation == rhs.orientation;
        }

        void swap(MotionXForm &&rhs) {
            std::swap(translation, rhs.translation);
            std::swap(orientation, rhs.orientation);
        }

		std::string description() const  {
			std::stringstream stream;
			stream << "t:("
			       << translation.x << ", "
				   << translation.y << ", "
				   << translation.z << ") "
				   << "o:("
				   << orientation.x << ", "
				   << orientation.y << ", "
				   << orientation.z << ")";
			return stream.str();
		}

        static MotionXForm zero() {
            auto xform = MotionXForm();
            xform.translation = Vector3::zero();
            xform.orientation = Vector3::zero();
            return xform;
        }

        static MotionXForm create(double tx, double ty, double tz,
                                  double ox, double oy, double oz) {
            auto xform = MotionXForm();
            xform.translation = Vector3::create(tx, ty, tz);
            xform.orientation = Vector3::create(ox, oy, oz);
            return xform;
        }
    };
}