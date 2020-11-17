
Texture3D Texture: register(t0);
Texture3D DetailTexture: register(t1);
Texture2D BlueNoise: register(t2);
SamplerState TextureSampler: register(s0);

//cloud property settings
cbuffer constant: register(b6)
{

    float4	 m_cloud_position;
    float4	 m_cloud_size;
    float4   m_sampling_resolution;
    float4   m_sampling_weight;
    float4   m_move_dir;
    float4   m_phase_parameters;
    float4   m_detail_sampling_weight;
    float	 m_cloud_density;
    float	 m_vertical_fade;
    float	 m_horizontal_fade;
    float	 m_per_pixel_fade_threshhold;
    float    m_per_sample_fade_threshhold;
    float	 m_in_scattering_strength;
    float	 m_out_scattering_strength;
    float	 m_time;
    float    m_speed;

    // NEW

    float  m_ray_offset_strength;
    float  m_density_offset;
    float  m_detail_noise_scale;
    float  m_detail_speed;
    float  m_detail_noise_weight;
    float  m_light_stepcount;
    float  m_darkness_threshold;
    float  m_light_absorption_towards_sun;
    float  m_light_absorption_through_cloud;
}

/* NEED TO ADD TO CONSTANT BUFFER

float  m_ray_offset_strength
float4 m_phase_parameters
float  m_density_offset
float  m_detail_noise_scale
float  m_detail_speed
float4 m_detail_sampling_weight
float  m_detail_noise_weight
float  m_light_stepcount (11)
float  m_darkness_threshold
float  m_light_absorption_towards_sun
float  m_light_absorption_through_cloud
*/

float remap(float v, float minOld, float maxOld, float minNew, float maxNew) {
    return minNew + (v - minOld) * (maxNew - minNew) / (maxOld - minOld);
}

// Returns (dstToBox, dstInsideBox). If ray misses box, dstInsideBox will be zero
float2 rayBoxDst(float3 boundsMin, float3 boundsMax, float3 rayOrigin, float3 invRayDir) 
{
    float3 t0 = (boundsMin - rayOrigin) * invRayDir;
    float3 t1 = (boundsMax - rayOrigin) * invRayDir;
    float3 tmin = min(t0, t1);
    float3 tmax = max(t0, t1);

    float dstA = max(max(tmin.x, tmin.y), tmin.z);
    float dstB = min(tmax.x, min(tmax.y, tmax.z));

    // CASE 1: ray intersects box from outside (0 <= dstA <= dstB)
    // dstA is dst to nearest intersection, dstB dst to far intersection

    // CASE 2: ray intersects box from inside (dstA < 0 < dstB)
    // dstA is the dst to intersection behind the ray, dstB is dst to forward intersection

    // CASE 3: ray misses box (dstA > dstB)

    float dstToBox = max(0, dstA);
    float dstInsideBox = max(0, dstB - dstToBox);
    return float2(dstToBox, dstInsideBox);
}

// Henyey-Greenstein
float hg(float a, float g) 
{
    float g2 = g * g;
    return (1 - g2) / (4 * 3.1415 * pow(1 + g2 - 2 * g * (a), 1.5));
}

float phase(float a) 
{
    float blend = .5;
    float hgBlend = hg(a, m_phase_parameters.x) * (1 - blend) + hg(a, -m_phase_parameters.y) * blend;
    return m_phase_parameters.z + hgBlend * m_phase_parameters.w;
}

float beer(float d) 
{
    float beer = exp(-d);
    return beer;
}

float remap01(float v, float low, float high) 
{
    return (v - low) / (high - low);
}



float sampleDensity(float3 ray_pos, float3 size, float3 bounds_min, float3 bounds_max) 
{
    const float base_scale = 1 / 1000.0;
    const float offsetSpeed = 1 / 100.0;

    // Calculate texture sample positions
    float time = m_time * m_speed;
    //float3 size = bounds_max - bounds_min;
    //float3 bounds_center = center;
    float3 uvw = (size * .5 + ray_pos) * base_scale * m_sampling_resolution.x;
    //float3 shapeSamplePos = uvw + shapeOffset * offsetSpeed + float3(time, time * 0.1, time * 0.2) * baseSpeed;
    float3 shape_sample_pos = uvw + float3(time, time * 0.1, time * 0.2) * m_speed;


    // Calculate falloff at along x/z edges of the cloud container
    const float edge_fade_distance = 50;
    float edgeX_dist = min(edge_fade_distance, min(ray_pos.x - bounds_min.x, bounds_max.x - ray_pos.x));
    float edgeZ_dist = min(edge_fade_distance, min(ray_pos.z - bounds_min.z, bounds_max.z - ray_pos.z));
    float edge_weight = min(edgeZ_dist, edgeX_dist) / edge_fade_distance;

    // Calculate height gradient from weather map
    //float2 weatherUV = (size.xz * .5 + (rayPos.xz-boundsCentre.xz)) / max(size.x,size.z);
    //float weatherMap = WeatherMap.SampleLevel(samplerWeatherMap, weatherUV, mipLevel).x;
    float gMin = .2;
    float gMax = .7;
    float height_percent = (ray_pos.y - bounds_min.y) / size.y;
    float height_gradient = saturate(remap(height_percent, 0.0, gMin, 0, 1)) * saturate(remap(height_percent, 1, gMax, 0, 1));
    //float height_gradient = 
    height_gradient *= edge_weight;

    // Calculate base shape density
    float4 shape_noise = Texture.SampleLevel(TextureSampler, shape_sample_pos, 0); //no mip levels for our 3d textures
    float4 normalized_shape_weights = m_sampling_weight / dot(m_sampling_weight, 1);
    float shapeFBM = dot(shape_noise, normalized_shape_weights) * height_gradient;
    float base_shape_density = shapeFBM + m_density_offset * 0.1;

    // If the density is 0 there is no reason to check detailed textures
    if (base_shape_density > 0) 
    {
        // Sample detail noise
        //float3 detail_sample_pos = uvw * m_detail_noise_scale + detailOffset * offsetSpeed + float3(time * .4, -time, time * 0.1) * detailSpeed;
        float3 detail_sample_pos = uvw * m_detail_noise_scale + float3(time * .4, -time, time * 0.1) * m_detail_speed;

        float4 detail_noise = DetailTexture.SampleLevel(TextureSampler, detail_sample_pos, 0); //no mip levels for our 3D textures
        float3 normalized_detail_weights = m_detail_sampling_weight / dot(m_detail_sampling_weight, 1);
        float detailFBM = dot(detail_noise, normalized_detail_weights);

        // Subtract detail noise from base shape (weighted by inverse density so that edges get eroded more than centre)
        float one_minus_shape = 1 - shapeFBM;
        float detail_erode_weight = one_minus_shape * one_minus_shape * one_minus_shape;
        float cloud_density = base_shape_density - (1 - detailFBM) * detail_erode_weight * m_detail_noise_weight;

        return cloud_density * m_cloud_density * 0.1;
    }
    return 0;
}


// Calculate proportion of light that reaches the given point from the lightsource
float lightmarch(float3 position, float3 inverse_light_dir, float3 bounds_min, float3 bounds_max)
{
    float3 dir_to_light = inverse_light_dir;
    float dist_inside_box = rayBoxDst(bounds_min, bounds_max, position, 1 / dir_to_light).y;

    float step_size = dist_inside_box / m_light_stepcount;
    float total_density = 0;

    float size = bounds_max - bounds_min;

    for (int step = 0; step < m_light_stepcount; step++)
    {
        position += dir_to_light * step_size;
        total_density += max(0, sampleDensity(position, size, bounds_min, bounds_max) * step_size);
    }

    float transmittance = exp(-total_density * m_light_absorption_towards_sun);
    return m_darkness_threshold + transmittance * (1 - m_darkness_threshold);
}








//float4 debugDrawNoise(float2 uv) 
//{
//
//    float4 channels = 0;
//    float3 samplePos = float3(uv.x, uv.y, debugNoiseSliceDepth);
//
//    if (debugViewMode == 1) {
//        channels = NoiseTex.SampleLevel(samplerNoiseTex, samplePos, 0);
//    }
//    else if (debugViewMode == 2) {
//        channels = DetailNoiseTex.SampleLevel(samplerDetailNoiseTex, samplePos, 0);
//    }
//    else if (debugViewMode == 3) {
//        channels = WeatherMap.SampleLevel(samplerWeatherMap, samplePos.xy, 0);
//    }
//
//    if (debugShowAllChannels) {
//        return channels;
//    }
//    else {
//        float4 maskedChannels = (channels * debugChannelWeight);
//        if (debugGreyscale || debugChannelWeight.w == 1) {
//            return dot(maskedChannels, 1);
//        }
//        else {
//            return maskedChannels;
//        }
//    }
//}