#include "triangle.h"

#include "CGL/CGL.h"
#include "GL/glew.h"

namespace CGL {
namespace SceneObjects {

Triangle::Triangle(const Mesh *mesh, size_t v1, size_t v2, size_t v3) {
  p1 = mesh->positions[v1];
  p2 = mesh->positions[v2];
  p3 = mesh->positions[v3];
  n1 = mesh->normals[v1];
  n2 = mesh->normals[v2];
  n3 = mesh->normals[v3];
  bbox = BBox(p1);
  bbox.expand(p2);
  bbox.expand(p3);

  bsdf = mesh->get_bsdf();
}

BBox Triangle::get_bbox() const { return bbox; }
bool Triangle::has_intersection(const Ray& r) const {
    // 使用 Möller-Trumbore 算法进行射线-三角形相交测试
    const Vector3D edge1 = p2 - p1;
    const Vector3D edge2 = p3 - p1;
    const Vector3D h = cross(r.d, edge2);
    const double a = dot(edge1, h);

    if (a > -EPS_F && a < EPS_F) {
        return false; // 射线与三角形平行
    }

    const double f = 1.0 / a;
    const Vector3D s = r.o - p1;
    const double u = f * dot(s, h);

    if (u < 0.0 || u > 1.0) {
        return false; // 射线在三角形外部
    }

    const Vector3D q = cross(s, edge1);
    const double v = f * dot(r.d, q);

    if (v < 0.0 || u + v > 1.0) {
        return false; // 射线在三角形外部
    }

    const double t = f * dot(edge2, q);

    if (t > r.min_t && t < r.max_t) {
        return true; // 射线与三角形相交
    }

    return false; // 射线与三角形不相交
}

bool Triangle::intersect(const Ray& r, Intersection* isect) const {
    // 使用 Möller-Trumbore 算法进行射线-三角形相交测试
    const Vector3D edge1 = p2 - p1;
    const Vector3D edge2 = p3 - p1;
    const Vector3D h = cross(r.d, edge2);
    const double a = dot(edge1, h);

    if (a > -EPS_F && a < EPS_F) {
        return false; // 射线与三角形平行
    }

    const double f = 1.0 / a;
    const Vector3D s = r.o - p1;
    const double u = f * dot(s, h);

    if (u < 0.0 || u > 1.0) {
        return false; // 射线在三角形外部
    }

    const Vector3D q = cross(s, edge1);
    const double v = f * dot(r.d, q);

    if (v < 0.0 || u + v > 1.0) {
        return false; // 射线在三角形外部
    }

    const double t = f * dot(edge2, q);

    if (t > r.min_t && t < r.max_t) {
        r.max_t = t; // 更新最近的交点

        // 更新交点信息
        isect->t = t;
        isect->n = (1 - u - v) * n1 + u * n2 + v * n3; // 使用重心坐标插值法线
        isect->primitive = this;
        isect->bsdf = get_bsdf();

        return true; // 射线与三角形相交
    }

    return false; // 射线与三角形不相交
}

/*bool Triangle::has_intersection(const Ray& r) const {
  // Part 1, Task 3: implement ray-triangle intersection
  // The difference between this function and the next function is that the next
  // function records the "intersection" while this function only tests whether
  // there is a intersection.


  return true;

}

bool Triangle::intersect(const Ray& r, Intersection* isect) const {
  // Part 1, Task 3:
  // implement ray-triangle intersection. When an intersection takes
  // place, the Intersection data should be updated accordingly


  return true;


}*/

void Triangle::draw(const Color &c, float alpha) const {
  glColor4f(c.r, c.g, c.b, alpha);
  glBegin(GL_TRIANGLES);
  glVertex3d(p1.x, p1.y, p1.z);
  glVertex3d(p2.x, p2.y, p2.z);
  glVertex3d(p3.x, p3.y, p3.z);
  glEnd();
}

void Triangle::drawOutline(const Color &c, float alpha) const {
  glColor4f(c.r, c.g, c.b, alpha);
  glBegin(GL_LINE_LOOP);
  glVertex3d(p1.x, p1.y, p1.z);
  glVertex3d(p2.x, p2.y, p2.z);
  glVertex3d(p3.x, p3.y, p3.z);
  glEnd();
}

} // namespace SceneObjects
} // namespace CGL
