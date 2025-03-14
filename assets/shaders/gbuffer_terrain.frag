#version 330 core

out vec4 FragColor;

in float vertexLight;
in float vertexDistance;

uniform bool wireframeMode;
uniform vec3 wireframeColor;

uniform float fogStart;
uniform float fogEnd;
uniform vec3 fogColor;

void main() {
    if (wireframeMode) {
        FragColor = vec4(wireframeColor, 1.0);
    } else {
        vec4 baseColor = vec4(1.0, 1.0, 1.0, 1.0);

        // Calculate fog value to merge with the base color
        float fogValue = vertexDistance < fogEnd ? smoothstep(fogStart, fogEnd, vertexDistance) : 1.0;

        vec4 finalColor = baseColor * vertexLight;
        FragColor = vec4(mix(finalColor.xyz, fogColor, fogValue), baseColor.a);
    }
}