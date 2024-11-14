#include "bvh.h"

#include "CGL/CGL.h"
#include "triangle.h"

#include <iostream>
#include <stack>

using namespace std;

namespace CGL {
namespace SceneObjects {

BVHAccel::BVHAccel(const std::vector<Primitive *> &_primitives,
                   size_t max_leaf_size) {

  primitives = std::vector<Primitive *>(_primitives);
  root = construct_bvh(primitives.begin(), primitives.end(), max_leaf_size);
}

BVHAccel::~BVHAccel() {
  if (root)
    delete root;
  primitives.clear();
}

BBox BVHAccel::get_bbox() const { return root->bb; }

void BVHAccel::draw(BVHNode *node, const Color &c, float alpha) const {
  if (node->isLeaf()) {
    for (auto p = node->start; p != node->end; p++) {
      (*p)->draw(c, alpha);
    }
  } else {
    draw(node->l, c, alpha);
    draw(node->r, c, alpha);
  }
}

void BVHAccel::drawOutline(BVHNode *node, const Color &c, float alpha) const {
  if (node->isLeaf()) {
    for (auto p = node->start; p != node->end; p++) {
      (*p)->drawOutline(c, alpha);
    }
  } else {
    drawOutline(node->l, c, alpha);
    drawOutline(node->r, c, alpha);
  }
}
BVHNode* BVHAccel::construct_bvh(std::vector<Primitive*>::iterator start,
    std::vector<Primitive*>::iterator end,
    size_t max_leaf_size) {

    // 计算当前图元集合的包围盒
    BBox bbox;
    for (auto p = start; p != end; p++) {
        BBox bb = (*p)->get_bbox();
        bbox.expand(bb);
    }

    // 创建一个新的 BVHNode 节点
    BVHNode* node = new BVHNode(bbox);
    node->start = start;
    node->end = end;

    // 如果图元数量小于或等于 max_leaf_size，则创建叶节点
    size_t num_primitives = end - start;
    if (num_primitives <= max_leaf_size) {
        return node;
    }

    // 选择一个轴和分割点，将图元分为左右两部分
    int axis = 0; // 选择 x 轴
    Vector3D centroid_sum;
    for (auto p = start; p != end; p++) {
        centroid_sum += (*p)->get_bbox().centroid();
    }
    Vector3D centroid_avg = centroid_sum / num_primitives;
    auto mid = std::partition(start, end, [axis, centroid_avg](Primitive* p) {
        return p->get_bbox().centroid()[axis] < centroid_avg[axis];
        });

    // 如果分割失败（所有图元在同一边），则强制分割
    if (mid == start || mid == end) {
        mid = start + num_primitives / 2;
    }

    // 递归地构建左右子节点
    node->l = construct_bvh(start, mid, max_leaf_size);
    node->r = construct_bvh(mid, end, max_leaf_size);

    return node;
}
/*BVHNode* BVHAccel::construct_bvh(std::vector<Primitive*>::iterator start,
                                 std::vector<Primitive *>::iterator end,
                                 size_t max_leaf_size) {

  // TODO (Part 2.1):
  // Construct a BVH from the given vector of primitives and maximum leaf
  // size configuration. The starter code build a BVH aggregate with a
  // single leaf node (which is also the root) that encloses all the
  // primitives.


  BBox bbox;

  for (auto p = start; p != end; p++) {
    BBox bb = (*p)->get_bbox();
    bbox.expand(bb);
  }

  BVHNode *node = new BVHNode(bbox);
  node->start = start;
  node->end = end;

  return node;


}*/

bool BVHAccel::has_intersection(const Ray &ray, BVHNode *node) const {
  // TODO (Part 2.3):
  // Fill in the intersect function.
  // Take note that this function has a short-circuit that the
  // Intersection version cannot, since it returns as soon as it finds
  // a hit, it doesn't actually have to find the closest hit.



  for (auto p : primitives) {
    total_isects++;
    if (p->has_intersection(ray))
      return true;
  }
  return false;


}

bool BVHAccel::intersect(const Ray &ray, Intersection *i, BVHNode *node) const {
  // TODO (Part 2.3):
  // Fill in the intersect function.



  bool hit = false;
  for (auto p : primitives) {
    total_isects++;
    hit = p->intersect(ray, i) || hit;
  }
  return hit;


}

} // namespace SceneObjects
} // namespace CGL
