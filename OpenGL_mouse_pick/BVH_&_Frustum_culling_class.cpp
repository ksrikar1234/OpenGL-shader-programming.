#include <iostream>
#include <vector>
#include <algorithm>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Vector3 {
public:
    float x, y, z;
};

class BoundingBox {
public:
    Vector3 min_point, max_point;
};

class BVHNode {
public:
    BoundingBox bounding_box;
    BVHNode* left_child;
    BVHNode* right_child;
    std::vector<Vector3> vertices;
};

class Frustum {
public:
    glm::mat4 projection_matrix;
    glm::mat4 view_matrix;

    bool isVertexVisible(const Vector3& vertex) {
        glm::vec4 transformed_vertex = view_matrix * glm::vec4(vertex.x, vertex.y, vertex.z, 1.0f);
        transformed_vertex /= transformed_vertex.w;

        bool inside_left = transformed_vertex.x >= -transformed_vertex.w;
        bool inside_right = transformed_vertex.x <= transformed_vertex.w;
        bool inside_bottom = transformed_vertex.y >= -transformed_vertex.w;
        bool inside_top = transformed_vertex.y <= transformed_vertex.w;
        bool inside_near = transformed_vertex.z >= 0.0f;
        bool inside_far = transformed_vertex.z <= transformed_vertex.w;

        return inside_left && inside_right && inside_bottom && inside_top && inside_near && inside_far;
    }
};

class BVH {
public:
    BVHNode* root;

    BoundingBox calculate_bounding_box(const std::vector<Vector3>& vertices) {
        Vector3 min_point = { FLT_MAX, FLT_MAX, FLT_MAX };
        Vector3 max_point = { -FLT_MAX, -FLT_MAX, -FLT_MAX };

        for (const auto& vertex : vertices) {
            min_point.x = std::min(min_point.x, vertex.x);
            min_point.y = std::min(min_point.y, vertex.y);
            min_point.z = std::min(min_point.z, vertex.z);
            max_point.x = std::max(max_point.x, vertex.x);
            max_point.y = std::max(max_point.y, vertex.y);
            max_point.z = std::max(max_point.z, vertex.z);
        }

        return { min_point, max_point };
    }

    void split_vertices(const std::vector<Vector3>& vertices, int split_axis, std::vector<Vector3>& left_vertices, std::vector<Vector3>& right_vertices) {
        std::vector<Vector3> sorted_vertices = vertices;
        std::sort(sorted_vertices.begin(), sorted_vertices.end(), [split_axis](const Vector3& a, const Vector3& b) {
            return a[split_axis] < b[split_axis];
        });

        int mid = sorted_vertices.size() / 2;
        left_vertices.assign(sorted_vertices.begin(), sorted_vertices.begin() + mid);
        right_vertices.assign(sorted_vertices.begin() + mid, sorted_vertices.end());
    }

    BVHNode* build_bvh_recursive(const std::vector<Vector3>& vertices) {
        BVHNode* node = new BVHNode();
        node->bounding_box = calculate_bounding_box(vertices);

        if (vertices.size() <= 4) {
            node->left_child = nullptr;
            node->right_child = nullptr;
            node->vertices = vertices;
        }
        else {
            int split_axis = node->bounding_box.max_point.x - node->bounding_box.min_point.x > node->bounding_box.max_point.y - node->bounding_box.min_point.y
                ? 0 : 1;
            std::vector<Vector3> left_vertices, right_vertices;
            split_vertices(vertices, split_axis, left_vertices, right_vertices);

            node->left_child = build_bvh_recursive(left_vertices);
            node->right_child = build_bvh_recursive(right_vertices);
        }

        return node;
    }

    void build_bvh(const std::vector<Vector3>& vertices) {
        root = build_bvh_recursive(vertices);
    }

    void traverse_bvh(const BVHNode* node, const Frustum& frustum, std::vector<Vector3>& visible_vertices) {
        if (node == nullptr)
            return;

        if (frustum.isVertexVisible(node->bounding_box.min_point) || frustum.isVertexVisible(node->bounding_box.max_point)) {
            if (node->left_child == nullptr && node->right_child == nullptr) {
                visible_vertices.insert(visible_vertices.end(), node->vertices.begin(), node->vertices.end());
            }
            else {
                traverse_bvh(node->left_child, frustum, visible_vertices);
                traverse_bvh(node->right_child, frustum, visible_vertices);
            }
        }
    }

    std::vector<Vector3> get_visible_vertices(const Frustum& frustum) {
        std::vector<Vector3> visible_vertices;
        traverse_bvh(root, frustum, visible_vertices);
        return visible_vertices;
    }
};

int main() {
    std::vector<Vector3> vertices; // Populate with your vertex data

    Frustum frustum; // Create a Frustum instance representing the view frustum
    BVH bvh;
    bvh.build_bvh(vertices);

    std::vector<Vector3> visible_vertices = bvh.get_visible_vertices(frustum);

    // Use the visible vertices for further processing/rendering

    // Cleanup: Delete the BVH nodes
    // Implement the necessary cleanup code to delete the BVH nodes

    return 0;
}
