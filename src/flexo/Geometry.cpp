#include "Geometry.hpp"

namespace geom
{
    Edge::Edge(glm::vec3 tail, glm::vec3 head)
        : tail(tail)
        , head(head)
    {
    }

    glm::vec3 Edge::ClosestPointTo(glm::vec3 point)
    {
        using glm::dot;

        glm::vec3 vec = head - tail;
        float len = dot(vec, vec);

        float portion = -1.0f;
        if (len != 0.0f) {
            portion = dot(point - tail, vec) / len;
        }

        if (portion > 1.0f) {
            return head;
        } else if (portion < 0.0f) {
            return tail;
        } else {
            return tail + portion * vec;
        }
    }

    Triangle::Triangle(glm::vec3 A, glm::vec3 B, glm::vec3 C)
        : A(A)
        , B(B)
        , C(C)
        , BC(B, C)
        , CA(C, A)
        , AB(A, B)
    {
        normal = glm::normalize(glm::cross(B - A, C - A));
    }

    glm::vec3 Triangle::ClosestPointTo(glm::vec3 point)
    {
        using glm::dot;

        glm::vec3 P = point + -normal * dot(point - A, normal);
        glm::vec3 weights = BarycentricCoordinates(P);

        // BC, CA, AB
        if (weights.x < 0.0f) {
            return BC.ClosestPointTo(P);
        } else if (weights.y < 0.0f) {
            return CA.ClosestPointTo(P);
        } else if (weights.z < 0.0f) {
            return AB.ClosestPointTo(P);
        }

        return P;
    }

    glm::vec3 Triangle::BarycentricCoordinates(glm::vec3 point)
    {
        using glm::dot, glm::cross;

        // Calculate the area ratios
        // The vertices are in counter-clockwise order.
        float oppA = dot(normal, cross(B - point, C - point));
        float oppB = dot(normal, cross(C - point, A - point));
        float total = dot(normal, cross(B - A, C - A));
        float u = oppA / total;
        float v = oppB / total;
        glm::vec3 weights = { u, v, 1 - u - v };
        return weights;
    }

    float SquaredDistance(glm::vec3 p1, glm::vec3 p2)
    {
        glm::vec3 v = p1 - p2;
        return glm::dot(v, v);
    }
}
