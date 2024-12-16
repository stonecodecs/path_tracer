#ifndef HITTABLE_LIST_H
#define HITTABLE_LIST_H

#include "hittable.h"
#include "rtweekend.h"
#include "aabb.h"

// using std::make_shared;
// using std::shared_ptr; // count for each function stack; when 0, deleted

// stores a list of Hittables
class HittableList : public Hittable {
  public:
    std::vector<shared_ptr<Hittable>> objects;

    HittableList() {}
    HittableList(shared_ptr<Hittable> object) { add(object); }

    void clear() { objects.clear(); }

    void add(shared_ptr<Hittable> object) {
        objects.push_back(object);
        bbox = AABB(bbox, object->bounding_box());
        // ok for bbox to be empty; it gets overwritten by object box
    }

    bool hit(const Ray& r, Interval ray_t, Hit& rec) const override {
        Hit temp_rec;
        bool hit_anything = false;
        auto closest_so_far = ray_t.max;

        // go over all objects and check hit on the closest one
        for (const auto& object : objects) {
            if (object->hit(r, Interval(ray_t.min, closest_so_far), temp_rec)) {
                hit_anything = true;
                closest_so_far = temp_rec.t;
                rec = temp_rec;
            }
        }

        return hit_anything;
    }

    AABB bounding_box() const override { return bbox; }

    private:
        AABB bbox; // scene content bbox (if miss, doesn't check anything else)
};

#endif