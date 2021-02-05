#ifndef _RTCLOUDS

Texture2D<float4> Texture: register(t0);
SamplerState TextureSampler: register(s0);

#else
#include "cloud.fx"
#endif


cbuffer constant: register(b0)
{
	row_major float4x4 m_world;
	row_major float4x4 m_view;
	row_major float4x4 m_proj;
	float4 m_camera_position;
	row_major float4x4 m_inverseVP;
};

cbuffer constant: register(b2)
{
	float3  m_global_light_dir;
	float	m_global_light_strength;
	float3  m_global_light_color;
	float3  m_ambient_light_color;
}


#define MAX_SPHERES (30)
//constant buffer for raymarch scene
cbuffer constant: register(b7)
{
	//system data for UV calculation
	float4		m_resolution;
	float4		m_params; //x = number of traces, y = number of spheres

	//ground data
	float4		m_ground_color;

	//lighting data 
	float4		m_pointlight_pos;
	float4		m_light_color;
	float4		m_directional_light;

	//sphere data 
	float4		m_position_and_radius[MAX_SPHERES];
	float4		m_local_colors[MAX_SPHERES];
	float4		m_local_speculars[MAX_SPHERES];

}

//raymarch functions
#define MAX_STEPS 100

#ifndef _RTCLOUDS
#define MAX_DIST 50
#else
#define MAX_DIST 5000
#endif

#define SURF_DIST 0.001
//#define MAX_SPHERES 30
static const float PI = 3.14159265f;

struct Ray
{
	float3 origin;
	float3 direction;
	float3 energy;
};

Ray createRay(float3 origin, float3 direction)
{
	Ray ray;
	ray.origin = origin;
	ray.direction = direction;
	return ray;
}


#ifndef _RTCLOUDS
struct RayHit
{
	float3 position;
	float distance;
	float3 normal;
	float3 albedo;
	float3 specular;
};

RayHit createRayHit()
{
	RayHit hit;
	hit.position = float3(0, 0, 0);
	hit.distance = MAX_DIST * 100;
	hit.normal = float3(0, 0, 0);
	return hit;
}
#else
struct RayHit
{
	float3 position;
	float distance;
	float far_distance;
	float3 normal;
	float3 albedo;
	float3 specular;
	float total_through;
};

RayHit createRayHit()
{
	RayHit hit;
	hit.position = float3(0, 0, 0);
	hit.distance = MAX_DIST * 100;
	hit.normal = float3(0, 0, 0);
	hit.far_distance = 0;
	hit.total_through = 0;
	return hit;
}
#endif


void intersectGroundPlane(Ray ray, inout RayHit bestHit)
{
	//calculate where the ground will be intersected
	float t = -ray.origin.y / ray.direction.y;

	if (t > 0 && t < bestHit.distance)
	{
		bestHit.distance = t;
		bestHit.position = ray.origin + t * ray.direction;
		bestHit.normal = float3(0, 1, 0);
		bestHit.albedo = m_ground_color.rgb;
		bestHit.specular = 0.03;
	}
}

#ifndef _RTCLOUDS


void intersectSphere(Ray ray, inout RayHit bestHit, int sphereID)
{
	//calculate distance to where sphere is intersected
	float4 sphere = m_position_and_radius[sphereID];
	float3 d = ray.origin - sphere.xyz;
	float p1 = -dot(ray.direction, d);

	float p2sqr = p1 * p1 - dot(d, d) + sphere.w * sphere.w;
	if (p2sqr < 0) return;
	float p2 = sqrt(p2sqr);
	float t = p1 - p2 > 0 ? p1 - p2 : p1 + p2;

	if (t > 0 && t < bestHit.distance)
	{
		bestHit.distance = t;
		bestHit.position = ray.origin + t * ray.direction;
		bestHit.normal = normalize(bestHit.position - sphere.xyz);
		bestHit.albedo = m_local_colors[sphereID].rgb;
		bestHit.specular = m_local_speculars[sphereID].rgb;
	}
}

RayHit trace(Ray ray)
{
	RayHit bestHit = createRayHit();
	intersectGroundPlane(ray, bestHit);

	for (int i = 0; i < min(m_params.y, MAX_SPHERES); i++) intersectSphere(ray, bestHit, i);

	return bestHit;
}

float3 shade(inout Ray ray, RayHit hit)
{
	if (hit.distance < MAX_DIST)
	{
		////return the normal
		//return hit.normal * 0.5 + 0.5;

		float3 specular = float3(0.6, 0.6, 0.6);

		ray.origin = hit.position + hit.normal * 0.001;
		ray.direction = reflect(ray.direction, hit.normal);
		ray.energy *= hit.specular;

		bool shadow = false;
		Ray shadowRay = createRay(hit.position + hit.normal * 0.001, -m_directional_light.xyz);
		RayHit shadowHit = trace(shadowRay);
		if (!(shadowHit.distance > MAX_DIST)) return float3(0, 0, 0);


		return saturate(dot(hit.normal, m_directional_light.xyz) * -1) * hit.albedo;

	}
	else
	{
		ray.energy = 0;
		ray.direction = normalize(ray.direction);
		float theta = acos(ray.direction.y) / PI;
		float phi = atan2(ray.direction.x + 1.388/*adjustment for aspect ratio*/, ray.direction.z) / PI;

		return Texture.SampleLevel(TextureSampler, float2(phi, theta), 0).rgb;
	}
}

#else

float sphereRayDist(float3 ro, float3 rd, float4 sphere, out float max_t)
{
	float3 d = ro - sphere.xyz;
	float p1 = -dot(rd, d);

	float p2sqr = p1 * p1 - dot(d, d) + sphere.w * sphere.w;
	if (p2sqr < 0) return MAX_DIST * 100.0;
	float p2 = sqrt(p2sqr);
	float t;

	if (p1 - p2 > 0)
	{
		t = p1 - p2;
		max_t = p1 + p2;
	}
	else
	{
		t = p1 + p2;
		max_t = p1 - p2;
	}

	return t;
}

float3 shade(inout Ray ray, RayHit hit);


void intersectSphereLight(Ray ray, inout RayHit bestHit, int sphereID)
{
	float max_t = 0;
	float4 sphere = m_position_and_radius[sphereID];
	float t = sphereRayDist(ray.origin, ray.direction, sphere, max_t);


	bestHit.far_distance = max(bestHit.far_distance, max_t);

	if (t < max_t)
	{
		float3 hitpos = ray.origin + t * ray.direction;
		float3 posnormal = normalize(hitpos - sphere.xyz);

		if (t > 0 && t < bestHit.distance)
		{
			bestHit.distance = t;
			bestHit.position = hitpos;
			bestHit.normal = posnormal;
			bestHit.albedo = 0;
			bestHit.specular = 0;
		}


		//sample the cloud density through the sphere
		float d = max(0, -dot(ray.direction, posnormal));
		float center_bias = pow(d, 3);

		float through_dist = d * sphere.w * 2;
		float step_size = through_dist / 5.0;
		float3 p = hitpos;

		float3 light_dir = float3(0, -1, 0);

		for (int i = 0; i < 5; i++)
		{
			bestHit.total_through += 0.2 * Texture.SampleLevel(TextureSampler, p / 200, 0).x * center_bias;
			p += ray.direction * step_size;
		}

		//we need: start position of lightray, the direction of the light

		//bestHit.total_through = shade(ray, bestHit);
		//bestHit.total_through += pow(d, 3) * sphere.w * 2;
	}
}

void intersectSphereOR(Ray ray, inout RayHit bestHit, int sphereID)
{
	float max_t = 0;
	float4 sphere = m_position_and_radius[sphereID];
	float t = sphereRayDist(ray.origin, ray.direction, sphere, max_t);


	bestHit.far_distance = max(bestHit.far_distance, max_t);

	if (t < max_t)
	{
		float3 hitpos = ray.origin + t * ray.direction;
		float3 posnormal = normalize(hitpos - sphere.xyz);

		if (t > 0 && t < bestHit.distance)
		{
			bestHit.distance = t;
			bestHit.position = hitpos;
			bestHit.normal = posnormal;
			bestHit.albedo = 0;
			bestHit.specular = 0;
		}


		//sample the cloud density through the sphere
		float d = max(0, -dot(ray.direction, posnormal));
		float center_bias = pow(d, 3);

		float through_dist = d * sphere.w * 2;
		float step_size = through_dist / 5.0;
		float3 p = hitpos;

		float3 light_dir = float3(0, -1, 0);

		for (int i = 0; i < 5; i++)
		{
			bestHit.total_through += 0.2 * Texture.SampleLevel(TextureSampler, p / 200, 0).x * center_bias;

			RayHit lightHit = createRayHit();
			Ray lightRay = createRay(p, -light_dir);


			p += ray.direction * step_size;
		}

		//we need: start position of lightray, the direction of the light

		//bestHit.total_through = shade(ray, bestHit);
		//bestHit.total_through += pow(d, 3) * sphere.w * 2;
	}
}

void intersectSphereAND(Ray ray, inout RayHit bestHit, int sphereID)
{
	float max_t = 0;
	float4 sphere = m_position_and_radius[sphereID];
	float t = sphereRayDist(ray.origin, ray.direction, sphere, max_t);


	bestHit.far_distance = max(bestHit.far_distance, max_t);

	if (t > 0 && t > bestHit.distance && bestHit.distance < MAX_DIST)
	{
		bestHit.distance = t;
		bestHit.position = ray.origin + t * ray.direction;
		bestHit.normal = normalize(bestHit.position - sphere.xyz);
		bestHit.albedo = 0;
		bestHit.specular = 0;

		//bestHit.total_through ;
	}
}

RayHit traceLight(Ray ray)
{
	RayHit bestHit = createRayHit();

	for (int i = 0; i < m_params.y; i++)
	{
		intersectSphereOR(ray, bestHit, i);
	}

	return bestHit;
}


RayHit trace(Ray ray)
{
	RayHit bestHit = createRayHit();

	int a = 0;
	for (int i = 0; i < m_params.y; i++)
	{
		intersectSphereOR(ray, bestHit, i);
	}

	return bestHit;
}

float3 shade(inout Ray ray, RayHit hit)
{
	//if (hit.distance < MAX_DIST)
	//{
	//	float3 specular = float3(0.6, 0.6, 0.6);

	//	ray.origin = hit.position + hit.normal * 0.001;
	//	ray.direction = reflect(ray.direction, hit.normal);
	//	ray.energy *= hit.specular;

	//	bool shadow = false;
	//	Ray shadowRay = createRay(hit.position + hit.normal * 0.001, -m_directional_light.xyz);
	//	RayHit shadowHit = trace(shadowRay);
	//	if (!(shadowHit.distance > MAX_DIST)) return float3(0, 0, 0);


	//	return saturate(dot(hit.normal, m_directional_light.xyz) * -1) * hit.albedo;

	//}

	return 1;
}

#endif

//Ray createCameraRay(float2 uv, float3 campos)
//{
//	// Transform the camera origin to world space
//	float3 origin = campos;//mul(_CameraToWorld, float4(0.0f, 0.0f, 0.0f, 1.0f)).xyz;
//
//	// Invert the perspective projection of the view-space position
//	float3 direction = mul(_CameraInverseProjection, float4(uv, 0.0f, 1.0f)).xyz;
//	// Transform the direction from camera to world space and normalize
//	direction = mul(_CameraToWorld, float4(direction, 0.0f)).xyz;
//	direction = normalize(direction);
//
//	Ray ray;
//	ray.origin = origin;
//	ray.direction = direction;
//
//	return ray;
//}

float getDist(float3 p)
{
	//float4 s = float4(0, 1, 6, 1);
	float3 s = m_position_and_radius[0].xyz;
	float r = 0.5;

	//float sphere_dist = length(p - s.xyz) - s.w;
	float sphere_dist = length(p - s) - r;
	float plane_dist = p.y;
	float d = min(sphere_dist, plane_dist);
	return d;
}

//returns the id of the hit surface and the distance to it.
float2 getDistSpheres(float3 p)
{
	float sphere_dist = 1000;
	float next_dist;
	int id = 1000;

	for (int i = 0; i < MAX_SPHERES; i++)
	{
		//next_dist = length(p - m_position_and_radius[i].xyz) - ((float[4])(m_radii[i / 4]))[i % 4];
		next_dist = length(p - m_position_and_radius[i].xyz) - (0.2 + i * 0.1f);

		if (next_dist < sphere_dist)
		{
			sphere_dist = next_dist;
			id = i;
		}
	}

	float plane_dist = p.y;
	float d = min(sphere_dist, plane_dist);
	//if the plane was hit, switch the id to a negative value denoting to use the plane color
	if (plane_dist < sphere_dist) id = -1;

	return float2(d, id);
}

float3 getNormal(float3 p)
{
	float d = getDist(p);
	float2 e = float2(0.01, 0);

	float3 n = d - float3(
		getDist(p - e.xyy),
		getDist(p - e.yxy),
		getDist(p - e.yyx)
		);

	return normalize(n);
}

//function which checks against the spheres version of our raymarcher
float3 getNormalSpheres(float3 p)
{
	float d = getDistSpheres(p).x;
	float2 e = float2(0.01, 0);

	float3 n = d - float3(
		getDistSpheres(p - e.xyy).x,
		getDistSpheres(p - e.yxy).x,
		getDistSpheres(p - e.yyx).x
		);

	return normalize(n);
}

//point light
float getLight(float3 p)
{
	float3 l = normalize(m_pointlight_pos.xyz - p);
	float3 n = getNormal(p);

	float dif = dot(n, l);

	return dif;
}

//checks for light against spheres version of raymarcher
float getLightSpheres(float3 p)
{
	float3 l = normalize(m_pointlight_pos.xyz - p);
	float3 n = getNormalSpheres(p);

	float dif = dot(n, l);

	return max(dif, 0);
}

float rayMarch(float3 pos, float3 dir)
{
	float dO = 0;

	for (int i = 0; i < MAX_STEPS; i++)
	{
		float3 p = pos + dir * dO;
		float dS = getDist(p);
		dO += dS;
		if (dO > MAX_DIST || dS < SURF_DIST) break;
	}

	return dO;
}


//returns the distance in w and the color of the surface in rgb
float4 rayMarchSpheres(float3 pos, float3 dir)
{
	float dO = 0;
	float3 p;
	float2 dS;

	for (int i = 0; i < MAX_STEPS; i++)
	{
		p = pos + dir * dO;
		dS = getDistSpheres(p);
		dO += dS.x;
		if (dO > MAX_DIST || dS.x < SURF_DIST) break;
	}

	float4 final;
	final.w = dO;
	if (dS.y > -1) final.rgb = m_local_colors[dS.y].rgb;
	else final.rgb = m_ground_color.rgb;

	return final;
}