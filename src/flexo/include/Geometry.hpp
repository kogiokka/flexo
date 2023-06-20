#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <glm/glm.hpp>

namespace geom
{
    class Edge
    {
    public:
        Edge(glm::vec3 tail, glm::vec3 head);
        glm::vec3 ClosestPointTo(glm::vec3 point);

    private:
        glm::vec3 tail;
        glm::vec3 head;
    };

    class Triangle
    {
    public:
        Triangle(glm::vec3 A, glm::vec3 B, glm::vec3 C);

        glm::vec3 ClosestPointTo(glm::vec3 point);
        glm::vec3 BarycentricCoordinates(glm::vec3 point);

    private:
        using Vertex = glm::vec3;
        Vertex A;
        Vertex B;
        Vertex C;
        Edge BC;
        Edge CA;
        Edge AB;
        glm::vec3 normal;
    };

    float SquaredDistance(glm::vec3 p1, glm::vec3 p2);
}

#endif
