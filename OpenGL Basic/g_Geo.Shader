#version 330 core

layout (triangles) in;
//layout (triangle_strip, max_vertices = 3) out; for explosive normal
layout (line_strip, max_vertices = 6) out;

out vec3 Normal;
out vec2 texCoord;
out vec3 currentPos;

in DATA
{
	vec3 Normal;
    vec2 texCoord;
    mat4 proj;
} data_in[];

/* Visualize normals*/
void main()
{
    //For each vertex of the triangle, add another vertex in the direction of the normal vector => line
    gl_Position = data_in[0].proj * gl_in[0].gl_Position;
    EmitVertex();
    gl_Position = data_in[0].proj * (gl_in[0].gl_Position + 0.1f * vec4(data_in[0].Normal, 0.0f));
    EmitVertex();
    //end primitive so it wont connect to the next vertex
    EndPrimitive();

    gl_Position = data_in[1].proj * gl_in[1].gl_Position;
    EmitVertex();
    gl_Position = data_in[1].proj * (gl_in[1].gl_Position + 0.1f * vec4(data_in[1].Normal, 0.0f));
    EmitVertex();
    EndPrimitive();

    gl_Position = data_in[2].proj * gl_in[2].gl_Position;
    EmitVertex();
    gl_Position = data_in[2].proj * (gl_in[2].gl_Position + 0.1f * vec4(data_in[2].Normal, 0.0f));
    EmitVertex();
    //end primitive so it wont connect to the next vertex
    EndPrimitive();




}


/* Explosive normal
void main()
{
    vec3 v0 = vec3(gl_in[0].gl_Position - gl_in[1].gl_Position);
    vec3 v1 = vec3(gl_in[2].gl_Position - gl_in[1].gl_Position);
    vec4 surfaceNormal = vec4(normalize(cross(v0, v1)), 1.0f);

    //currentPos = vec3(gl_in[0].glPosition);
    gl_Position = data_in[0].proj * (gl_in[0].gl_Position + surfaceNormal);
    Normal = data_in[0].Normal;
    texCoord = data_in[0].texCoord;
    EmitVertex();

    //currentPos = vec3(gl_in[1].glPosition);
    gl_Position = data_in[1].proj * (gl_in[1].gl_Position + surfaceNormal);
    Normal = data_in[1].Normal;
    texCoord = data_in[1].texCoord;
    EmitVertex();

    //currentPos = vec3(gl_in[2].glPosition);
    gl_Position = data_in[2].proj * (gl_in[2].gl_Position + surfaceNormal);
    Normal = data_in[2].Normal;
    texCoord = data_in[2].texCoord;
    EmitVertex();

    EndPrimitive();
}
*/