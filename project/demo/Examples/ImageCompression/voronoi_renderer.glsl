#[compute]
#version 460
#extension GL_EXT_shader_atomic_float : enable

layout(set = 0, binding = 0) uniform sampler2D inputSampler;
layout(set = 0, binding = 1, rgba8) restrict uniform writeonly image2D outputImage;
layout(set = 0, binding = 2, std430) restrict readonly buffer PointBuffer {
    float pointData[];
};
layout(set = 0, binding = 3, std430) coherent buffer ErrorBuffer { float error; };

layout(set = 0, binding = 4, std430) coherent buffer Settings { 
    float WIDTH;
    float HEIGHT;
    float POINT_COUNT;
};

struct VoronoiPoint {
    vec2 position;
    vec3 color;
};

float distSquared( vec3 A, vec3 B )
{
    vec3 C = A - B;
    return dot( C, C );
}

// Function to decode a VoronoiPoint from pointData
VoronoiPoint getPoint(int index) {
    VoronoiPoint point;
    int baseIndex = index * 5; // 5 floats per Point
    point.position = vec2(
        pointData[baseIndex],
        pointData[baseIndex + 1]
    );
    point.color = vec3(
        pointData[baseIndex + 2],
        pointData[baseIndex + 3],
        pointData[baseIndex + 4]
    );
    return point;
}

// Function to find the closest Voronoi point
vec3 findClosestPointColor(vec2 pos) {
    float minDist = 1e10; // Large initial value
    vec3 closestColor = vec3(0.0);
    for (int i = 0; i < POINT_COUNT; ++i) {
        VoronoiPoint point = getPoint(i);
        float dist = distance(pos, point.position);
        if (dist < minDist) {
            minDist = dist;
            closestColor = point.color;
        }
    }
    return closestColor;
}

layout(local_size_x = 32, local_size_y = 32, local_size_z = 1) in;
void main() {
    ivec2 pos = ivec2(gl_GlobalInvocationID.xy);
    if (pos.x >= WIDTH || pos.y >= HEIGHT) return;

    vec2 normalized_pos = vec2(pos) / vec2(WIDTH, HEIGHT);
    vec3 color = findClosestPointColor(normalized_pos);

    vec3 input_color = texture(inputSampler, normalized_pos).rgb; 
    float local_error = distSquared(color, input_color); 
    // Use atomic add to accumulate the error 
    atomicAdd(error, local_error);

    color = clamp(color, 0.0, 1.0);    
    imageStore(outputImage, pos, vec4(color, 1.0));
}
