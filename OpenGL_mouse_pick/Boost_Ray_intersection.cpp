#include <boost/compute/core.hpp>
#include <boost/compute/algorithm/fill.hpp>
#include <boost/compute/algorithm/transform.hpp>
#include <boost/compute/container/vector.hpp>
#include <boost/compute/functional/math.hpp>
#include <boost/compute/functional/functional.hpp>
#include <boost/compute/types/fundamental.hpp>

namespace compute = boost::compute;

typedef struct {
    compute::float3_ origin;
    compute::float3_ direction;
} Ray;

typedef struct {
    compute::float3_ v0;
    compute::float3_ v1;
    compute::float3_ v2;
} Triangle;

struct ray_triangle_intersection {
    Triangle triangle;

    ray_triangle_intersection(const Triangle& tri) : triangle(tri) {}

    compute::float_ operator()(const Ray& ray) const {
        compute::float3_ edge1 = triangle.v1 - triangle.v0;
        compute::float3_ edge2 = triangle.v2 - triangle.v0;

        compute::float3_ h = cross(ray.direction, edge2);
        compute::float_ a = dot(edge1, h);

        if (a > -0.00001f && a < 0.00001f)
            return 0.0f;

        compute::float_ f = 1.0f / a;
        compute::float3_ s = ray.origin - triangle.v0;
        compute::float_ u = f * dot(s, h);

        if (u < 0.0f || u > 1.0f)
            return 0.0f;

        compute::float3_ q = cross(s, edge1);
        compute::float_ v = f * dot(ray.direction, q);

        if (v < 0.0f || u + v > 1.0f)
            return 0.0f;

        compute::float_ t = f * dot(edge2, q);

        if (t > 0.00001f) {
            return 1.0f;
        }

        return 0.0f;
    }
};

int main() {
    // Create a compute context and command queue
    compute::device device = compute::system::default_device();
    compute::context context(device);
    compute::command_queue queue(context, device);

    // Define input data
    std::vector<Ray> rays = { /* your rays here */ };
    std::vector<Triangle> triangles = { /* your triangles here */ };
    size_t numRays = rays.size();
    size_t numTriangles = triangles.size();

    // Create compute buffers for rays, triangles, and intersections
    compute::vector<Ray> rayBuffer(rays.begin(), rays.end(), context);
    compute::vector<Triangle> triangleBuffer(triangles.begin(), triangles.end(), context);
    compute::vector<int> intersectionBuffer(numRays, context);

    // Create a transform iterator to compute intersections
    auto rayBegin = rayBuffer.begin();
    auto intersectionBegin = intersectionBuffer.begin();
    auto intersectionEnd = intersectionBuffer.end();

    // Perform ray-triangle intersection
    compute::transform(rayBegin, rayBegin + numRays, intersectionBegin, ray_triangle_intersection(triangleBuffer)); // we need to pass the function as functor

    // Copy intersection results back to the host
    std::vector<int> intersections(numRays);
    compute::copy(intersectionBegin, intersectionEnd, intersections.begin(), queue);

    // Print intersection results
    for (size_t i = 0; i < numRays; i++) {
        std::cout << "Ray " << i << " intersects: " << intersections[i] << std::endl;
    }

    return 0;
}
