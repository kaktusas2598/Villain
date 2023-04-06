#ifndef __INTERSECT_DATA__
#define __INTERSECT_DATA__

namespace Villain {

    class IntersectData {
        public:
            IntersectData(const bool intersect, const float dist) :
                intersected(intersect), distance(dist) {}

            inline bool isIntersecting() const { return intersected; }
            inline float getDistance() const { return distance; }
        private:
            const bool intersected;
            const float distance;
    };
}

#endif // __INTERSECT_DATA__
