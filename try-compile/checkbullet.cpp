
#include <BulletCollision/CollisionShapes/btSphereShape.h>
int main(int argc, char** argv)
{
    btSphereShape shape(1.0);
    btScalar mass(1.0);
    btVector3 inertia;
    shape.calculateLocalInertia(mass, inertia);
    return 0;
}
