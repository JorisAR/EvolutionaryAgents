#[compute]
#version 460
#extension GL_EXT_shader_atomic_float : enable

const int gridWidth = 1024;
const int SPLAT_COUNT = 100;

layout(set = 0, binding = 0) uniform sampler2D inputSampler;
layout(set = 0, binding = 1, rgba8) restrict uniform writeonly image2D outputImage;
layout(set = 0, binding = 2, std430) restrict readonly buffer SplatBuffer {
    float splatData[];
};
layout(set = 0, binding = 3, std430) coherent buffer ErrorBuffer { float error; };

struct Splat {
    vec4 color;
    vec2 position;
    float rotation;
    vec2 scale;
};

// Function to decode a Splat from splatData
Splat getSplat(int index) {
    Splat splat;
    int baseIndex = index * 9; // 8 floats per Splat
    splat.color = vec4(
        splatData[baseIndex],
        splatData[baseIndex + 1],
        splatData[baseIndex + 2],
        splatData[baseIndex + 3]
    );
    splat.position = vec2(
        splatData[baseIndex + 4],
        splatData[baseIndex + 5]
    );
    splat.rotation = splatData[baseIndex + 6];
    splat.scale = vec2(
        splatData[baseIndex + 7],
        splatData[baseIndex + 8]
    );
    return splat;
}

// Check if a point is inside a rotated rectangle
bool isInSplat(Splat splat, vec2 point) {
    return length((point - splat.position) ) < 0.1;
    //return length((point - splat.position) / splat.scale ) < 1;

}

layout(local_size_x = 32, local_size_y = 32, local_size_z = 1) in;
void main() {
    ivec2 pos = ivec2(gl_GlobalInvocationID.xy);
    if (pos.x >= gridWidth || pos.y >= gridWidth) return;

    vec2 normalized_pos = vec2(pos) / gridWidth;
    vec3 color = vec3(0);
    for (int i = 0; i < SPLAT_COUNT; ++i) { 
        Splat splat = getSplat(i);
        if (isInSplat(splat, normalized_pos)) {
            color += splat.color.rgb * splat.color.a; // Sum the color
        }
    }

    vec3 input_color = texture(inputSampler, normalized_pos).rgb; 
    float local_error = distance(color, input_color); 
    // Use atomic add to accumulate the error 
    atomicAdd(error, local_error);

    color = clamp(color, 0.0, 1.0);    
    imageStore(outputImage, pos, vec4(color, 1));
}
