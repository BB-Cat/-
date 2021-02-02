Texture2D<float4> Texture: register(t0);
SamplerState TextureSampler: register(s0);

cbuffer constant: register(b0)
{
	row_major float4x4 m_world;
	row_major float4x4 m_view;
	row_major float4x4 m_proj;
	float4 m_camera_position;
};

cbuffer constant: register(b2)
{
	float3  m_global_light_dir;
	float	m_global_light_strength;
	float3  m_global_light_color;
	float3  m_ambient_light_color;
}


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
	float4		m_position_and_radius[12];
	float4		m_local_colors[12];
	float4		m_local_speculars[12];

}

//raymarch functions
#define MAX_STEPS 100
#define MAX_DIST 50
#define SURF_DIST 0.001
#define MAX_SPHERES 12
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
		////signal the shader to sample skybox
		//return -1;

		ray.energy = 0;

		ray.direction * 0.5f + 0.5f;
		float theta = acos(ray.direction.y) / PI;
		float phi = atan2(ray.direction.x + 1.0, -ray.direction.z) / PI * 0.5;
		return Texture.SampleLevel(TextureSampler, float2(phi, theta), 0).rgb;
	}
}

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