#include "Collision.h"
#include <string>
#include "Matrix4X4.h"

bool Collision::DetectCollision(Collider* col1, Vec3 pos1, Collider* col2, Vec3 pos2)
{
    //first check the bounding boxes. if the bounding boxes miss there is no collision
    Vec3 min1, min2, max1, max2;
    min1 = Vec3(pos1 - col1->getBoundingBox() / 2.0f);
    min2 = Vec3(pos2 - col2->getBoundingBox() / 2.0f);
    max1 = Vec3(pos1 + col1->getBoundingBox() / 2.0f);
    max2 = Vec3(pos2 + col2->getBoundingBox() / 2.0f);

    if (!Collision::DetectAABBDiscrete(min1, max1, min2, max2)) return false;

    //pointer declarations for possible types
    CubeCollider* c = nullptr;
    SphereCollider* s = nullptr;
    CapsuleCollider* cp = nullptr;

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
        cp = reinterpret_cast<CapsuleCollider*>(col1);
        return interpretCapsuleDetection(cp, pos1, col2, pos2);

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

    if (!Collision::DetectAABBDiscrete(min1, max1, min2, max2)) return false;

    //pointer declarations for possible types
    CubeCollider* c = nullptr;
    SphereCollider* s = nullptr;
    CapsuleCollider* cp = nullptr;

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
        cp = reinterpret_cast<CapsuleCollider*>(col1);
        return interpretCapsuleResolution(cp, pos1, prevpos, col2, pos2, mass);
        break;
    }

    return false;
}

bool Collision::DetectAABBDiscrete(Vec3 min1, Vec3 max1, Vec3 min2, Vec3 max2)
{
    if (min1.x < max2.x &&
        max1.x > min2.x &&
        min1.y < max2.y &&
        max1.y > min2.y &&
        min1.z < max2.z &&
        max1.z > min2.z) return true;


    return false;
}

bool Collision::DetectAABBContinuous(Collider* col1, Vec3 pos1, Vec3 prevpos, Collider* col2, Vec3 pos2)
{
    //draw a large box around the minimum value between pos1 and prevpos and the max value of pos1 and prevpos, 
    //run a normal discrete aabb detection between them, then solve for time if they collided to verify if its a collision

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

bool Collision::DetectCapsuleCapsule(CapsuleCollider* col1, Vec3 pos1, CapsuleCollider* col2, Vec3 pos2, Vec3* closest1, Vec3* closest2)
{
    Vec3 col1_cap1, col1_cap2, col2_cap1, col2_cap2;
    col1->getCaps(col1_cap1, col1_cap2);
    col2->getCaps(col2_cap1, col2_cap2);

    col1_cap1 += pos1;
    col1_cap2 += pos1;
    col2_cap1 += pos2;
    col2_cap2 += pos2;

    float dist = segmentSegmentDistance(col1_cap1, col1_cap2, col2_cap1, col2_cap2, closest1, closest2);

    float r1 = col1->getRadius();
    float r2 = col2->getRadius();

    if (r1 + r2 > dist) return true;
    else return false;
}

bool Collision::ResolveAABBContinuous(CubeCollider* col1, Vec3& pos1, Vec3 prevpos, CubeCollider* col2, Vec3& pos2, float mass)
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

bool Collision::ResolveSphereCubeContinuous(Vec3& s_pos, float radius, Vec3 prevpos, CubeCollider* col2, Vec3& c_pos, float mass, Vec3* closest)
{
    //get the size of the cube
    Vec3 cube_dims = col2->getBoundingBox();

    //project the CLOSEST POINT of the cube onto the sphere's direction of travel.
    //this will guaranteed give us a position WITHIN two ranges of the cube, and OUTSIDE one.
    //the value OUTSIDE of the range is the side we compare with.

    //The ONE exception to this is if the sphere is headed directly for an edge of the cube,
    //in which case we simply collide them at that point.
    Vec3 move = s_pos - prevpos;
    float move_len = move.length();

    //get the closest point on the cube to s_pos, then find the projection
    Vec3 c = prevpos;
    Vec3 c_min = c_pos - cube_dims / 2.0f;
    Vec3 c_max = c_pos + cube_dims / 2.0f;

    c.x = min(max(c.x, c_min.x), c_max.x);
    c.y = min(max(c.y, c_min.y), c_max.y);
    c.z = min(max(c.z, c_min.z), c_max.z);

    Vec3 sphere_to_cube_closest = c - s_pos;
    float dot = Vec3::dot(sphere_to_cube_closest, move);

    //apparently this projection vector is checking out mathematically, which I don't understand
    // - Because the sphere's movement is not axis aligned this WON'T work and MIGHT mean my sphere sphere collider
    // is still broken too (it SEEMS okay right now anyways)
    // I'm pretty sure i can do the opposite, eg. project the sphere move vector onto the cube face 
    //in order to find the inverse vector, then just flip it around

    //!!
    //OR i have to do a matrix rotation so that the sphere vector is axis aligned and the cube is rotated.
    //!!

    //build a rotation matrix around the sphere's movement vector

    /////////////////////////////////////////////////
    Matrix4x4 mat;
    mat.lookAt(move, Vec3(0, 0, 0), Vec3(0, 1, 0));
    Matrix4x4 inverse_mat = mat;
    inverse_mat.inverse();



    Vec3 rotated_sphere_to_cube_closest = inverse_mat * sphere_to_cube_closest;
    Vec3 rotated_move = inverse_mat * move;

    Vec3 proj_vec = rotated_move * (dot / (move_len * move_len));

    //proj_vec = mat * proj_vec;

    Vec3 proj_pos = s_pos + proj_vec;
    Vec3 proj_pos_to_cube_closest = c - proj_pos;
    /////////////////////////////////////////////////


    //Vec3 proj_vec = move * (dot / (move_len * move_len));
    //Vec3 proj_pos = s_pos + proj_vec;
    //Vec3 proj_pos_to_cube_closest = c - proj_pos;

    //TODO: we need to check if the length between the cube closest and the projection has a length smaller than
    //the radius of the sphere.  if it does, that means the sphere will collide with the corner and not the face (i think)


    //determine which face we are colliding with and get the distance between the closest point on the sphere and that face
    Vec3 sphere_closest_point = prevpos + proj_pos_to_cube_closest / proj_pos_to_cube_closest.length() * radius;
    float dist_until_possible_collision;
    float collision_time;
    if (proj_pos.x < c_min.x || proj_pos.x > c_max.x)
    {
        //we have potential to collide with the x axis
        if (proj_pos.x < c_min.x) //the face we could collide with is the min side of x
        {
            dist_until_possible_collision = abs(sphere_closest_point.x - c_min.x);
        }
        else //the face we could collide with is the max side of x
        {
            dist_until_possible_collision = abs(sphere_closest_point.x - c_max.x);
        }
        collision_time = dist_until_possible_collision / abs((move / move_len).x);
    }
    else if (proj_pos.y < c_min.y || proj_pos.y > c_max.y)
    {
        //we have potential to collide with the y axis
        if (proj_pos.y < c_min.y) //the face we could collide with is the min side of y
        {
            dist_until_possible_collision = abs(sphere_closest_point.y - c_min.y);
        }
        else //the face we could collide with is the max side of y
        {
            dist_until_possible_collision = abs(sphere_closest_point.y - c_max.y);
        }
        collision_time = dist_until_possible_collision / abs((move / move_len).y);
    }
    else if (proj_pos.z < c_min.z || proj_pos.z > c_max.z)
    {
        //we have potential to collide with the z axis
        if (proj_pos.z < c_min.z) //the face we could collide with is the min side of z
        {
            dist_until_possible_collision = abs(sphere_closest_point.z - c_min.z);
        }
        else //the face we could collide with is the max side of z
        {
            dist_until_possible_collision = abs(sphere_closest_point.z - c_max.z);
        }
        collision_time = dist_until_possible_collision / abs((move / move_len).z);
    }
    else
    {
        OutputDebugString(L"ERROR: There was a problem calculating the axis of collision in a sphere/cube collision");
        return false;
    }
    //get where the sphere is at first possible collision

    //TEST// - set the sphere to where the first possible collision is expected
    s_pos = s_pos + (move / move_len) * (collision_time - 0.1f);

    return true;
}

bool Collision::ResolveSphereSphereContinuous(Vec3& endpos, float r, Vec3 prevpos, Vec3& pos2, float r2, float mass)
{
    Vec3 endpos_to_pos2 = pos2 - endpos;
    float lensq = endpos_to_pos2.x * endpos_to_pos2.x + endpos_to_pos2.y * endpos_to_pos2.y + endpos_to_pos2.z * endpos_to_pos2.z;
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
        endpos_to_pos2.normalize();
        pos2 += endpos_to_pos2 * (1.0f - mass) * (r + r2 - diflen);
        endpos += endpos_to_pos2 * (mass) * (r + r2 - diflen) * -1;
        return true;
    }
    
    //find the time of collision
    float dot = Vec3::dot(endpos_to_pos2, move);
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

bool Collision::ResolveSphereCubeDiscrete(Vec3& s_pos, float radius, CubeCollider* col2, Vec3& c_pos, float mass, Vec3* closest)
{
    //get the size of the cube
    Vec3 cube_dims = col2->getBoundingBox();

    //get the closest point on the cube to s_pos
    Vec3 c = s_pos;
    Vec3 c_min = c_pos - cube_dims / 2.0f;
    Vec3 c_max = c_pos + cube_dims / 2.0f;

    c.x = min(max(c.x, c_min.x), c_max.x);
    c.y = min(max(c.y, c_min.y), c_max.y);
    c.z = min(max(c.z, c_min.z), c_max.z);

    Vec3 sphere_to_closest = c - s_pos;

    //if the sphere is inside the cube, push by the edge of least intersection (this is not a perfect resolution)
    if (c == s_pos)
    {
        float xdif, ydif, zdif;
        xdif = s_pos.x - c_pos.x;
        ydif = s_pos.y - c_pos.y;
        zdif = s_pos.z - c_pos.z;

        float abs_xdif = abs(xdif);
        float abs_ydif = abs(ydif);
        float abs_zdif = abs(zdif);

        if (abs_xdif < abs_ydif && abs_xdif < abs_zdif)
        {
            float move_amount = (radius + (cube_dims.x - abs_xdif));

            if (s_pos.x < c_pos.x)
            {
                s_pos.x -= move_amount * mass;
                c_pos.x += move_amount * (1.0f - mass);
            }
            else
            {
                s_pos.x += move_amount * mass;
                c_pos.x -= move_amount * (1.0f - mass);
            }
        }
        else if (abs_ydif < abs_zdif)
        {
            float move_amount = (radius + (cube_dims.y - abs_ydif));

            if (s_pos.y < c_pos.y)
            {
                s_pos.y -= move_amount * mass;
                c_pos.y += move_amount * (1.0f - mass);
            }
            else
            {
                s_pos.y += move_amount * mass;
                c_pos.y -= move_amount * (1.0f - mass);
            }
        }
        else
        {
            float move_amount = (radius + (cube_dims.z - abs_zdif));

            if (s_pos.z < c_pos.z)
            {
                s_pos.z -= move_amount * mass;
                c_pos.z += move_amount * (1.0f - mass);
            }
            else
            {
                s_pos.z += move_amount * mass;
                c_pos.z -= move_amount * (1.0f - mass);
            }
        }
        return true;
    }


    float move_amount = radius - sphere_to_closest.length();

    if (move_amount <= 0) return false;

    //normalize the movement vector and push the shapes apart
    sphere_to_closest.normalize();

    s_pos = s_pos - sphere_to_closest * move_amount * mass;
    c_pos = c_pos + sphere_to_closest * move_amount * (1.0f - mass);

    return true;
}

bool Collision::ResolveCapsuleCapsuleDiscrete(CapsuleCollider* col1, Vec3& pos1, CapsuleCollider* col2, Vec3& pos2, float mass, Vec3* closest1, Vec3* closest2)
{
    Vec3 c1, c2;
    bool hit = DetectCapsuleCapsule(col1, pos1, col2, pos2, &c1, &c2);
    if (closest1 != nullptr) *closest1 = c1;
    if (closest2 != nullptr) *closest2 = c2;
    if (!hit) return false;


    Vec3 c1_to_c2 = c2 - c1;
    float dist = c1_to_c2.length();
    float move_amount = col1->getRadius() + col2->getRadius() - dist;

    //normalize the seperation vector here since we already calculated length
    c1_to_c2 = c1_to_c2 / dist;

    //adjust the positions
    pos1 += c1_to_c2 * move_amount * -1.0f * mass;
    pos2 += c1_to_c2 * move_amount * (1.0f - mass);

    return true;
}

//========================================================================//
//UTILITY FUNCTIONS
//========================================================================//

float Collision::pointPointDistance(Vec3 p1, Vec3 p2)
{
    return (p1 - p2).length();
}

float Collision::lineLineDistance(Vec3 p1, Vec3 e1, Vec3 p2, Vec3 e2, float& dist_from_l1, float& dist_from_l2)
{
    //calculate to see if parallel or not
    Vec3 v1 = e1 - p1;
    Vec3 v2 = e2 - p2;

    Vec3 v1n = v1.getNormalized();
    Vec3 v2n = v2.getNormalized();

    //float dot1 = Vec3::dot(p2 - p1, v1n);
    //float dot2 = Vec3::dot(p2 - p1, v2n);

    Vec3 cross_n = Vec3::cross(v1n, v2n);
    float cross_len = cross_n.length();

    //if the lines are parallel, return their distance
    if (cross_len < 0.000001f)
    {
        float len = pointLineDistance(p1, p2, e2, dist_from_l2);
        dist_from_l1 = 0.0f;

        return len;
    }

    float v1len = v1.length();
    float v2len = v2.length();

    float dot_v1v2 = Vec3::dot(v1, v2);
    float v1len_sq = v1len * v1len;
    float v2len_sq = v2len * v2len;
    Vec3 p2p1 = p1 - p2;
    dist_from_l1 = (dot_v1v2 * Vec3::dot(v2, p2p1) - v2len_sq * Vec3::dot(v1, p2p1)) / (v1len_sq * v2len_sq - dot_v1v2 * dot_v1v2);
    Vec3 finalp1 = p1 + v1 * dist_from_l1;
    dist_from_l2 = Vec3::dot(v2, finalp1 - p2) / v2len_sq;
    Vec3 finalp2 = p2 + v2 * dist_from_l2;

    return (finalp2 - finalp1).length();
}

float Collision::segmentSegmentDistance(Vec3 p1, Vec3 e1, Vec3 p2, Vec3 e2, Vec3* closest1, Vec3* closest2)
{
    float closest1_len, closest2_len;

    //calculate to see if parallel or not
    Vec3 v1 = e1 - p1;
    Vec3 v2 = e2 - p2;
    Vec3 v1n = v1.getNormalized();
    Vec3 v2n = v2.getNormalized();

    //float dot1 = Vec3::dot(p2 - p1, v1n);
    //float dot2 = Vec3::dot(p2 - p1, v2n);

    Vec3 cross_n = Vec3::cross(v1n, v2n);
    float dV = cross_n.length();

    //if the lines are parallel, return their distance
    if (dV < 0.000001f)
    {
        closest1_len = 0.0f;
        float len = pointSegmentDistance(p1, p2, e2, closest2_len);
        if (0.0f <= closest2_len && closest2_len <= 1.0f) 
            return len;
    }

    float line_dist = lineLineDistance(p1, e1, p2, e2, closest1_len, closest2_len);
    if (0.0f <= closest1_len && closest1_len <= 1.0f && 0.0f <= closest2_len && closest2_len <= 1.0f)
    {
        if (closest1 != nullptr) *closest1 = p1 + v1 * closest1_len;
        if (closest2 != nullptr) *closest2 = p2 + v2 * closest2_len;

        return line_dist;
    }


    //find the closest distance on line one to line two.
    closest1_len = max(min(closest1_len, 1.0f), 0.0f);
    Vec3 p1final = p1 + v1 * closest1_len;
    float len = pointSegmentDistance(p1final, p2, e2, closest2_len);
    //if the closest distance to line two is inside segment 2, return
    if (0.0f <= closest2_len && closest2_len <= 1.0f)
    {
        if (closest1 != nullptr) *closest1 = p1 + v1 * closest1_len;
        if (closest2 != nullptr) *closest2 = p2 + v2 * closest2_len;

        return len;
    }

    //if the closest distance was outside of segment 2, clamp segment 2 
    //and check the closest point on line 1
    closest2_len = max(min(closest2_len, 1.0f), 0.0f);
    Vec3 p2final = p2 + v2 * closest2_len;
    len = pointSegmentDistance(p2final, p1, e1, closest1_len);
    //if it was inside segment 1, return
    if (0.0f <= closest1_len && closest1_len <= 1.0f)
    {
        if (closest1 != nullptr) *closest1 = p1 + v1 * closest1_len;
        if (closest2 != nullptr) *closest2 = p2 + v2 * closest2_len;

        return len;
    }

    //otherwise clamp segment 1 and we have our closest points
    closest1_len = max(min(closest1_len, 1.0f), 0.0f);
    p1final = p1 + v1 * closest1_len;
    p2final = p2 + v2 * closest2_len;

    if (closest1 != nullptr) *closest1 = p1 + v1 * closest1_len;
    if (closest2 != nullptr) *closest2 = p2 + v2 * closest2_len;

    return (p2final - p1final).length();
}

float Collision::pointLineDistance(Vec3 point, Vec3 line_start, Vec3 line_end, float& dist_from_l1)
{
    Vec3 line = line_end - line_start;
    float line_len = line.length();
    float line_len_sq = line_len * line_len;
    dist_from_l1 = 0.0f;
    if (line_len_sq > 0.0f) dist_from_l1 = Vec3::dot((point - line_start), line) / line_len_sq;

    Vec3 line_closest = line_start + line * dist_from_l1;

    return (point - line_closest).length();
}

float Collision::pointSegmentDistance(Vec3 point, Vec3 line_start, Vec3 line_end, float& dist_from_l1)
{
    float len = pointLineDistance(point, line_start, line_end, dist_from_l1);

    if (isAcute((point - line_start), (line_end - line_start)) == false)
    {
        return (line_start - point).length();
    }
    else if (isAcute((point - line_end), (line_start - line_end)) == false)
    {
        return (line_end - point).length();
    }

    return len;
}

bool Collision::isAcute(const Vec3& v1, const Vec3& v2)
{
    return (Vec3::dot(v1, v2) >= 0.0f);
}

//========================================================================//
//INTERPRETATION FUNCTIONS
//========================================================================//


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

bool Collision::interpretCapsuleDetection(CapsuleCollider* col1, Vec3 pos1, Collider* col2, Vec3 pos2)
{
    //pointer declarations for possible types
    CubeCollider* c = nullptr;
    SphereCollider* s = nullptr;
    CapsuleCollider* cp = nullptr;

    //variables in case of a cube collider
    Vec3 min;
    Vec3 max;

    switch (col2->getType())
    {
    case ColliderTypes::Cube:
        //TEMP
        c = reinterpret_cast<CubeCollider*>(col2);
        min = pos2 - c->getBoundingBox() / 2.0f;
        max = pos2 + c->getBoundingBox() / 2.0f;
        return DetectSphereCube(pos1, col1->getRadius(), min, max);

    case ColliderTypes::Sphere:
        //TEMP
        s = reinterpret_cast<SphereCollider*>(col2);
        return DetectSphereSphere(pos1, col1->getRadius(), pos2, s->getRadius());

    case ColliderTypes::Capsule:
        cp = reinterpret_cast<CapsuleCollider*>(col2);
        return DetectCapsuleCapsule(col1, pos1, cp, pos2);
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
        return ResolveAABBContinuous(col1, pos1, prevpos, c, pos2, mass);

    case ColliderTypes::Sphere:
        s = reinterpret_cast<SphereCollider*>(col2);
        /*we have to provide inverse mass here because the sphere 
        is considered the first collider in ResolveSphereCube function*/
        //return ResolveSphereCube(pos2, s->getRadius(), prevpos, min1, max1, pos1, 1.0f - mass);
        return ResolveSphereCubeDiscrete(pos2, s->getRadius(), col1, pos1, 1.0f - mass);

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
        return ResolveSphereCubeDiscrete(pos1, col1->getRadius(), c, pos2, mass);
        //return false;

    case ColliderTypes::Sphere:
        s = reinterpret_cast<SphereCollider*>(col2);
        return ResolveSphereSphereContinuous(pos1, col1->getRadius(), prevpos, pos2, s->getRadius(), mass);

    case ColliderTypes::Capsule:
        //TODO : add capsule support
        //return
        break;
    }

    return false;
}

bool Collision::interpretCapsuleResolution(CapsuleCollider* col1, Vec3& pos1, Vec3 prevpos, Collider* col2, Vec3& pos2, float mass)
{
    //pointer declarations for possible types
    CubeCollider* c = nullptr;
    SphereCollider* s = nullptr;
    CapsuleCollider* cp = nullptr;

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
        return ResolveSphereCubeDiscrete(pos1, col1->getRadius(), c, pos2, mass);
        //return false;

    case ColliderTypes::Sphere:
        s = reinterpret_cast<SphereCollider*>(col2);
        return ResolveSphereSphereContinuous(pos1, col1->getRadius(), prevpos, pos2, s->getRadius(), mass);

    case ColliderTypes::Capsule:
        cp = reinterpret_cast<CapsuleCollider*>(col2);
        return ResolveCapsuleCapsuleDiscrete(col1, pos1, cp, pos2, mass);
    }

    return false;
}
