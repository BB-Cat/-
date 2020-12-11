#include "Collision.h"
#include <string>

bool Collision::DetectCollision(Collider* col1, Vec3 pos1, Collider* col2, Vec3 pos2)
{
    //first check the bounding boxes. if the bounding boxes miss there is no collision
    Vec3 min1, min2, max1, max2;
    min1 = Vec3(pos1 - col1->getBoundingBox() / 2.0f);
    min2 = Vec3(pos2 - col2->getBoundingBox() / 2.0f);
    max1 = Vec3(pos1 + col1->getBoundingBox() / 2.0f);
    max2 = Vec3(pos2 + col2->getBoundingBox() / 2.0f);

    if (!Collision::DetectAABB(min1, max1, min2, max2)) return false;

    //pointer declarations for possible types
    CubeCollider* c = nullptr;
    SphereCollider* s = nullptr;

    //reinterpret the colliders based on their IDs then collide them
    switch (col1->getType())
    {
    case ColliderTypes::Cube:
        c = reinterpret_cast<CubeCollider*>(col1);
        return interpretCubeDetection(min1, max1, col2, pos2);

    case ColliderTypes::Sphere:
        s = reinterpret_cast<SphereCollider*>(col1);
        return interpretSphereDetection(s, pos1, col2, pos2);

    case ColliderTypes::Capsule:
        //TODO : add capsule support
        break;
    }

    return false;
}

bool Collision::ResolveCollision(Collider* col1, Vec3& pos1, Vec3 prevpos, Collider* col2, Vec3& pos2, float mass)
{
    //first check the bounding boxes. if the bounding boxes miss there is no collision
    Vec3 min1, min2, max1, max2;
    min1 = Vec3(pos1 - col1->getBoundingBox() / 2.0f);
    min2 = Vec3(pos2 - col2->getBoundingBox() / 2.0f);
    max1 = Vec3(pos1 + col1->getBoundingBox() / 2.0f);
    max2 = Vec3(pos2 + col2->getBoundingBox() / 2.0f);

    if (!Collision::DetectAABB(min1, max1, min2, max2)) return false;

    //pointer declarations for possible types
    CubeCollider* c = nullptr;
    SphereCollider* s = nullptr;

    //reinterpret the colliders based on their IDs then collide them
    switch (col1->getType())
    {
    case ColliderTypes::Cube:
        c = reinterpret_cast<CubeCollider*>(col1);
        return interpretCubeResolution(c, min1, max1, pos1, prevpos, col2, pos2, mass);

    case ColliderTypes::Sphere:
        s = reinterpret_cast<SphereCollider*>(col1);
        return interpretSphereResolution(s, pos1, prevpos, col2, pos2, mass);

    case ColliderTypes::Capsule:
        //TODO : add capsule support
        break;
    }

    return false;
}

bool Collision::DetectAABB(Vec3 min1, Vec3 max1, Vec3 min2, Vec3 max2)
{
    if (min1.x < max2.x &&
        max1.x > min2.x &&
        min1.y < max2.y &&
        max1.y > min2.y &&
        min1.z < max2.z &&
        max1.z > min2.z) return true;


    return false;
}

bool Collision::DetectSphereCube(Vec3 s_pos, float radius, Vec3 c_min, Vec3 c_max, Vec3* closest)
{
    //find the closest point to the sphere on the cube
    Vec3 c = s_pos;
    c.x = min(max(c.x, c_min.x), c_max.x);
    c.y = min(max(c.y, c_min.y), c_max.y);
    c.z = min(max(c.z, c_min.z), c_max.z);

    if(closest != nullptr) *closest = c;

    //calculate distance from sphere
    if (Vec3(s_pos - c).length() < radius * radius) return true;

    return false;
}

bool Collision::DetectSphereSphere(Vec3 pos1, float r, Vec3 pos2, float r2)
{
    Vec3 dif = pos2 - pos1;
    float lensq = dif.x * dif.x + dif.y * dif.y + dif.z * dif.z;
    float rsq = (r + r2) * (r + r2);

    return (lensq < rsq);
}

bool Collision::ResolveAABB(CubeCollider* col1, Vec3& pos1, Vec3 prevpos, CubeCollider* col2, Vec3& pos2, float mass)
{
    //movement vectors
    Vec3 move = pos1 - prevpos;
    Vec3 move_n = move;
    move_n.normalize();

    //Remember : these min max values are position INDEPENDENT, unlike the functions that come before this one.
    Vec3 min1 = col1->getBoundingBox() / 2.0f;
    Vec3 min2 = col2->getBoundingBox() / 2.0f;
    Vec3 max1 = col1->getBoundingBox() / 2.0f;
    Vec3 max2 = col2->getBoundingBox() / 2.0f;

//============================================//
    //find the overlap
//============================================//
    //find the time it will take to collide the two cubes on all three axises
    float x1, x2, y1, y2, z1, z2;
    float x_time, y_time, z_time;
    bool _1_is_min;

    //X axis
    _1_is_min = (move.x < 0);
    x1 = prevpos.x - min1.x * (_1_is_min) + max1.x * (!_1_is_min);
    x2 = pos2.x - min2.x * (!_1_is_min) + max2.x * (_1_is_min);
    x_time = (x2 - x1) / move.x;

    //Y axis
    _1_is_min = (move.y < 0);
    y1 = prevpos.y - min1.y * (_1_is_min) + max1.y * (!_1_is_min);
    y2 = pos2.y - min2.y * (!_1_is_min) + max2.y * (_1_is_min);
    y_time = (y2 - y1) / move.y;

    //Z axis
    _1_is_min = (move.z < 0);
    z1 = prevpos.z - min1.z * (_1_is_min) + max1.z * (!_1_is_min);
    z2 = pos2.z - min2.z * (!_1_is_min) + max2.z * (_1_is_min);
    z_time = (z2 - z1) / move.z;

    //take the lowest positive time to find the true axis of collision
    float collision_time = 10000; //large number to make sure xyz time are all lower.
    int adjustment_axis = -1;
    if (x_time > 0)
    {
        adjustment_axis = 0;
        collision_time = x_time;
    }
    if (y_time < collision_time && y_time > 0)
    {
        adjustment_axis = 1;
        collision_time = y_time;
    }
    if (z_time < collision_time && z_time > 0)
    {
        adjustment_axis = 2;
        collision_time = z_time;
    }

#ifdef DEBUG
    if (collision_time <= 1.0f && collision_time >= 0)
    {
        OutputDebugString("A faulty collision was detected outside of the expected timeframe.")
    }
#endif

    //push the two positions based on the mass of the cubes
    float dist;
    switch (adjustment_axis)
    {
    case 0: //X axis
        _1_is_min = (move.x < 0);
        x1 = pos1.x - min1.x * (_1_is_min) + max1.x * (!_1_is_min);
        dist = (x1 - x2) * 1.05f;
        pos1.x -= dist * mass;
        pos2.x += dist * (1.0f - mass);
        break;

    case 1: //Y axis
        _1_is_min = (move.y < 0);
        y1 = pos1.y - min1.y * (_1_is_min) + max1.y * (!_1_is_min);
        dist = (y1 - y2) * 1.05f;
        pos1.y -= dist * mass;
        pos2.y += dist * (1.0f - mass);
        break;

    case 2: //Z axis
        _1_is_min = (move.z < 0);
        z1 = pos1.z - min1.z * (_1_is_min) + max1.z * (!_1_is_min);
        dist = (z1 - z2) * 1.05f;
        pos1.z -= dist * mass;
        pos2.z += dist * (1.0f - mass);
        break;
    }

    return true;
}

bool Collision::ResolveSphereCube(Vec3& s_pos, float radius, Vec3 prevpos, CubeCollider* col2, Vec3& c_pos, float mass, Vec3* closest)
{
    return false;
}

bool Collision::ResolveSphereSphere(Vec3& endpos, float r, Vec3 prevpos, Vec3& pos2, float r2, float mass)
{
    Vec3 dif = pos2 - endpos;
    float lensq = dif.x * dif.x + dif.y * dif.y + dif.z * dif.z;
    float rsq = (r + r2) * (r + r2);

    //no collision
    if (!(lensq < rsq)) return false;

    //in the event that the two spheres were already overlapping before the movement, adjust their positions first
    /* This can happen when another collision pushes two spheres together between collision updates */
    Vec3 initial_check = (prevpos - pos2);
    float initial_len = initial_check.length();
    float overlap = (r + r2) - initial_len;

    if (initial_len < r + r2)
    {
        initial_check.normalize();
        prevpos += initial_check * overlap * mass;
        //move the endpos as well so the movement vector is unaffected
        endpos += initial_check * overlap * mass;
        pos2 -= initial_check * overlap *  (1.0f - mass);
    }


    Vec3 move = endpos - prevpos;
    float move_len = move.length();
    if (move_len < 0.00001f)
    {
        float diflen = sqrt(lensq);
        dif.normalize();
        pos2 += dif * (1.0f - mass) * (r + r2 - diflen);
        endpos += dif * (mass) * (r + r2 - diflen) * -1;
        return true;
    }
    
    //find the time of collision
    float dot = Vec3::dot(dif, move);
    //projection vector from sphere2 to sphere1's movement vector
    Vec3 proj_vec = move * (dot / (move_len * move_len));
    //get the projected position
    Vec3 proj_pos = endpos + proj_vec;

    //use the length between the projected position and sphere 2 
    //with the length of the radii of two spheres to find point of collision with Pythagorean's Theorum
    Vec3 pos2_to_proj_pos = proj_pos - pos2;

    //if the collision is direct, simply solve collision
    if (pos2_to_proj_pos.length() < 0.00001f)
    {
        float len_to_collision_from_proj = (r + r2);
        Vec3 collision_pos = pos2 + (move / move_len) * -1 * len_to_collision_from_proj;
        Vec3 col_to_prevpos = prevpos - collision_pos;
        float remaining_movement = (move_len - col_to_prevpos.length()) * (1.0f - mass);
        
        pos2 = pos2 + (move / move_len) * remaining_movement;
        endpos = pos2 + (move / move_len) * (r + r2) * -1;

        return true;
    }

    float pos2_to_proj_pos_len = (pos2_to_proj_pos).length();
    //in the event that the spheres are on a tangent course, it is a false positive
    float len_to_collision_from_proj_sq = (r + r2) - (pos2_to_proj_pos_len);
    if (len_to_collision_from_proj_sq < 0.00001f) return false;

    float len_to_collision_from_proj = sqrt(len_to_collision_from_proj_sq);
    
    //find the exact point of collision
    float prev_to_proj_pos_len = (proj_pos - prevpos).length();
    Vec3 collision_pos = prevpos + move * ((prev_to_proj_pos_len - len_to_collision_from_proj) / prev_to_proj_pos_len);
    //Vec3 collision_pos = prevpos + (move / move_len) * (prev_to_proj_pos_len - len_to_collision_from_proj);
    //find the vector between point of collision and sphere2
    Vec3 col_to_pos2 = pos2 - collision_pos;
    Vec3 col_to_prevpos = prevpos - collision_pos;




    //take a proportion of the reflection vector and a proportion of the original vector based on mass
    float remaining_movement = move_len - col_to_prevpos.length();

    //calculate how much to offset the second sphere.
    //since this function is not physics based and is for colliders, we are "shoving" not "rolling" the spheres.
    //find how close to the projection point sphere1 will actually come, then find the "true" intersection amount.
    float collision_reference_point_dist = max(min(1.0f, remaining_movement / len_to_collision_from_proj), 0);
    float collision_amount = collision_reference_point_dist * len_to_collision_from_proj_sq;
    Vec3 final_ref_pos = collision_pos + (move / move_len) * collision_amount;
    Vec3 sphere2_move = pos2 - final_ref_pos;

    //find the tangent vector to the direction the second ball moved
    Vec3 collision_binormal = Vec3::cross(move, sphere2_move);
    Vec3 collision_tangent = Vec3::cross(sphere2_move, collision_binormal);

    Vec3 p1_adjusted_move = collision_tangent * mass + move * (1.0f - mass);
    p1_adjusted_move.normalize();

    endpos = collision_pos + p1_adjusted_move * remaining_movement;
    


    float adjustment_length_p2 = ((r + r2) - sphere2_move.length());
    sphere2_move.normalize();
    pos2 += sphere2_move * adjustment_length_p2 * (1.0f - mass);

    


    //pos2 = pos2 + sphere2_move * adjustment_length_p2 * (1.0f - mass);


    float finallen = (endpos - pos2).length();
    OutputDebugString((L"Final Length :" + std::to_wstring(finallen) + L"\n").c_str());
    return true;
}

bool Collision::interpretCubeDetection(Vec3 min, Vec3 max, Collider* col2, Vec3 pos2)
{
    //pointer declarations for possible types
    SphereCollider* s = nullptr;

    switch (col2->getType())
    {
    case ColliderTypes::Cube:
        //in the case of two cubes, we have already confirmed collision so simply return true
        return true;

    case ColliderTypes::Sphere:
        s = reinterpret_cast<SphereCollider*>(col2);
        return DetectSphereCube(pos2, s->getRadius(), min, max);
 
    case ColliderTypes::Capsule:
        //TODO : add capsule support
        break;
    }

    return false;
}

bool Collision::interpretSphereDetection(SphereCollider* col1, Vec3 pos1, Collider* col2, Vec3 pos2)
{
    //pointer declarations for possible types
    CubeCollider* c = nullptr;
    SphereCollider* s = nullptr;

    //variables in case of a cube collider
    Vec3 min;
    Vec3 max;

    switch (col2->getType())
    {
    case ColliderTypes::Cube:
        c = reinterpret_cast<CubeCollider*>(col2);
        min = pos2 - c->getBoundingBox() / 2.0f;
        max = pos2 + c->getBoundingBox() / 2.0f;
        return DetectSphereCube(pos1, col1->getRadius(), min, max);

    case ColliderTypes::Sphere:
        s = reinterpret_cast<SphereCollider*>(col2);
        return DetectSphereSphere(pos1, col1->getRadius(), pos2, s->getRadius());

    case ColliderTypes::Capsule:
        //TODO : add capsule support
        //return
        break;
    }

    return false;
}

bool Collision::interpretCubeResolution(CubeCollider* col1, Vec3 min1, Vec3 max1, Vec3& pos1, Vec3 prevpos, Collider* col2, Vec3& pos2, float mass)
{
    //pointer declarations for possible types
    CubeCollider* c = nullptr;
    SphereCollider* s = nullptr;

    switch (col2->getType())
    {
    case ColliderTypes::Cube:
        c = reinterpret_cast<CubeCollider*>(col2);
        return ResolveAABB(col1, pos1, prevpos, c, pos2, mass);

    case ColliderTypes::Sphere:
        s = reinterpret_cast<SphereCollider*>(col2);
        /*we have to provide inverse mass here because the sphere 
        is considered the first collider in ResolveSphereCube function*/
        //return ResolveSphereCube(pos2, s->getRadius(), prevpos, min1, max1, pos1, 1.0f - mass);
        return ResolveSphereCube(pos2, s->getRadius(), prevpos, col1, pos1, mass);

    case ColliderTypes::Capsule:
        //TODO : add capsule support
        break;
    }

    return false;
}

bool Collision::interpretSphereResolution(SphereCollider* col1, Vec3& pos1, Vec3 prevpos, Collider* col2, Vec3& pos2, float mass)
{
    //pointer declarations for possible types
    CubeCollider* c = nullptr;
    SphereCollider* s = nullptr;

    //variables in case of a cube collider
    Vec3 min;
    Vec3 max;

    switch (col2->getType())
    {
    case ColliderTypes::Cube:
        c = reinterpret_cast<CubeCollider*>(col2);
        min = pos2 - c->getBoundingBox() / 2.0f;
        max = pos2 + c->getBoundingBox() / 2.0f;
        //return ResolveSphereCube(pos1, col1->getRadius(), prevpos, min, max, pos2, mass);
        return false;

    case ColliderTypes::Sphere:
        s = reinterpret_cast<SphereCollider*>(col2);
        return ResolveSphereSphere(pos1, col1->getRadius(), prevpos, pos2, s->getRadius(), mass);

    case ColliderTypes::Capsule:
        //TODO : add capsule support
        //return
        break;
    }

    return false;
}
