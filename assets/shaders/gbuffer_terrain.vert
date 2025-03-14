#version 330 core

layout (location = 0) in vec3 aPos;// The position variable has attribute position 0
layout (location = 1) in float aStaticLight;// The position variable has attribute position 1

out float vertexLight;
out float vertexDistance;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

void main() {
    mat4 modelViewMatrix = viewMatrix * modelMatrix;
    gl_Position = projMatrix * modelViewMatrix * vec4(aPos, 1.0);

    vertexLight = aStaticLight;
    vertexDistance = length((modelViewMatrix * vec4(aPos, 1.0)).xyz);
}