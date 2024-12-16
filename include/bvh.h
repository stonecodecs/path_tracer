#ifndef BVH_H
#define BVH_H

#include "aabb.h"
#include "hittable.h"
#include "hittable_list.h"
#include <algorithm>

class BVH_node : public Hittable {
    public:
        BVH_node(HittableList list) : BVH_node(list.objects, 0, list.objects.size()) {}
        BVH_node(std::vector<shared_ptr<Hittable>>& objects, size_t start, size_t end) {
            // build bounding box of the span of source objects
            bbox = AABB::empty;
            for (size_t i = start; i < end; i++) {
                bbox = AABB(bbox, objects[i]->bounding_box());
            }

            // int axis = gen_random_int(0, 2);
            // instead of random, better to use longest side
            int axis = bbox.longest_axis();

            size_t range = end - start;
            if (range == 1) { // one object
                left = right = objects[start];
            } else if (range == 2) { // two objects (split)
                left  = objects[start];
                right = objects[start + 1];
            } else { // multiple object case split
                std::sort(std::begin(objects) + start,
                          std::begin(objects) + end,
                          [axis](shared_ptr<Hittable> a, shared_ptr<Hittable> b) {
                            return box_compare(a, b, axis);
                          });

                auto mid = start + range / 2;
                left  = make_shared<BVH_node>(objects, start, mid);
                right = make_shared<BVH_node>(objects, mid, end);
            }

            // bbox = AABB(left->bounding_box(), right->bounding_box());
        }

        // remember 'rec' is info to 'return'
        bool hit(const Ray& r, Interval ray_t, Hit& rec) const override {
            if (!bbox.hit(r, ray_t)) return false; // no hit

            bool hit_left  = left->hit(r, ray_t, rec);
            bool hit_right = right->hit(
                r,
                Interval(ray_t.min, hit_left ? rec.t : ray_t.max),
                rec);
            // if hit left, then we only need to look at this left side of the right box
            // as its closer to the intersection point than the right side (farther away)
            // basically, if L and R do not overlap, right child is skipped
            // if does overlap, only looks as far as L.max to find a closer object (if exists)
            return hit_left || hit_right;
        }

        AABB bounding_box() const override { return bbox; }
    public:
    shared_ptr<Hittable> left;
    shared_ptr<Hittable> right;
    AABB bbox;

    static bool box_compare(
        const shared_ptr<Hittable> a, const shared_ptr<Hittable> b, int axis_index
    ) {
        // comparator for std::sort by earlier start 
        auto a_interval = a->bounding_box().axis_interval(axis_index);
        auto b_interval = b->bounding_box().axis_interval(axis_index);
        return a_interval.min < b_interval.min;
    }
};

#endif