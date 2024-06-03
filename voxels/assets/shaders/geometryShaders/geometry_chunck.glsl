#version 330 core
layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

// uniforms 
uniform mat4 view;
uniform mat4 projection;
//uniform vec3 lightPos;
uniform float blockSize;
uniform vec3 colors[256];

in VS_OUT{
    int blockType;
    int blockOrientation;
} gs_in[];

    out vec3 Normal;
	//out vec3 FragPos;
	//out vec3 LightPos;
	out vec3 objectColor;
    out float opacity;

void main() {    
    float sd = blockSize / 2.0; 

    objectColor = colors[gs_in[0].blockType];
    //objectColor = vec3(1.0, 1.0, 1.0);
    
    opacity = 1.0;
    if(gs_in[0].blockType == 1) opacity = 0.75;

    vec4 tmp = gl_in[0].gl_Position * blockSize;
    tmp.w = 1.0;

    if(gs_in[0].blockOrientation == 0){
        // zBACK

        Normal = vec3(0.0, 0.0, 1.0);

        // vertex 1
        vec4 aPos = tmp + vec4(-sd, -sd, sd, 0.0);
        gl_Position = projection * view * aPos;
        //FragPos = vec3(aPos);
        EmitVertex();

        // vertex 2
        aPos = tmp +   vec4(sd, -sd, sd, 0.0);
        gl_Position = projection * view * aPos;
        //FragPos = vec3(aPos);
        EmitVertex();

         // vertex 3
        aPos = tmp +   vec4(-sd, sd, sd, 0.0);
        gl_Position = projection * view * aPos;
        //FragPos = vec3(aPos);
        EmitVertex();

         // vertex 4
        aPos = tmp +   vec4(sd, sd, sd, 0.0);
        gl_Position = projection * view * aPos;
        //FragPos = vec3(aPos);
        EmitVertex();

        EndPrimitive();

    }
    else if(gs_in[0].blockOrientation == 1){
        // zFRONT

        Normal = vec3(0.0, 0.0, -1.0);

        // vertex 1
        vec4 aPos = tmp +   vec4(sd, -sd, -sd, 0.0);
        gl_Position = projection * view * aPos;
        //FragPos = vec3(aPos);
        EmitVertex();

        // vertex 2
        aPos = tmp +   vec4(-sd, -sd, -sd, 0.0);
        gl_Position = projection * view * aPos;
        //FragPos = vec3(aPos);
        EmitVertex();

         // vertex 3
        aPos = tmp +   vec4(sd, sd, -sd, 0.0);
        gl_Position = projection * view * aPos;
        //FragPos = vec3(aPos);
        EmitVertex();

         // vertex 4
        aPos = tmp +   vec4(-sd, sd, -sd, 0.0);
        gl_Position = projection * view * aPos;
        //FragPos = vec3(aPos);
        EmitVertex();

        EndPrimitive();
    }
    else if(gs_in[0].blockOrientation == 2){
        // yBOTTOM

        Normal = vec3(0.0, -1.0, 0.0);

        // vertex 1
        vec4 aPos = tmp +   vec4(sd, -sd, sd, 0.0);
        gl_Position = projection * view * aPos;
        //FragPos = vec3(aPos);
        EmitVertex();

        // vertex 2
        aPos = tmp +   vec4(-sd, -sd, sd, 0.0);
        gl_Position = projection * view * aPos;
        //FragPos = vec3(aPos);
        EmitVertex();

         // vertex 3
        aPos = tmp +   vec4(sd, -sd, -sd, 0.0);
        gl_Position = projection * view * aPos;
        //FragPos = vec3(aPos);
        EmitVertex();

         // vertex 4
        aPos = tmp +   vec4(-sd, -sd, -sd, 0.0);
        gl_Position = projection * view * aPos;
        //FragPos = vec3(aPos);
        EmitVertex();

        EndPrimitive();
    }
    else if(gs_in[0].blockOrientation == 3){
        // yUP

        Normal = vec3(0.0, 1.0, 0.0);

        // vertex 1
        vec4 aPos = tmp +   vec4(-sd, sd, sd, 0.0);
        gl_Position = projection * view * aPos;
        //FragPos = vec3(aPos);
        EmitVertex();

        // vertex 2
        aPos = tmp +   vec4(sd, sd, sd, 0.0);
        gl_Position = projection * view * aPos;
        //FragPos = vec3(aPos);
        EmitVertex();

         // vertex 3
        aPos = tmp +   vec4(-sd, sd, -sd, 0.0);
        gl_Position = projection * view * aPos;
        //FragPos = vec3(aPos);
        EmitVertex();

         // vertex 4
        aPos = tmp +   vec4(sd, sd, -sd, 0.0);
        gl_Position = projection * view * aPos;
        //FragPos = vec3(aPos);
        EmitVertex();

        EndPrimitive();
    }
    else if(gs_in[0].blockOrientation == 4){
        // xRIGHT

        Normal = vec3(1.0, 0.0, 0.0);

        // vertex 1
        vec4 aPos = tmp +   vec4(sd, -sd, sd, 0.0);
        gl_Position = projection * view * aPos;
        //FragPos = vec3(aPos);
        EmitVertex();

        // vertex 2
        aPos = tmp +   vec4(sd, -sd, -sd, 0.0);
        gl_Position = projection * view * aPos;
        //FragPos = vec3(aPos);
        EmitVertex();

         // vertex 3
        aPos = tmp +   vec4(sd, sd, sd, 0.0);
        gl_Position = projection * view * aPos;
        //FragPos = vec3(aPos);
        EmitVertex();

         // vertex 4
        aPos = tmp +   vec4(sd, sd, -sd, 0.0);
        gl_Position = projection * view * aPos;
        //FragPos = vec3(aPos);
        EmitVertex();

        EndPrimitive();
    }
    else{
        // xLEFT

        Normal = vec3(-1.0, 0.0, 0.0);

        // vertex 1
        vec4 aPos = tmp +   vec4(-sd, -sd, -sd, 0.0);
        gl_Position = projection * view * aPos;
        //FragPos = vec3(aPos);
        EmitVertex();

        // vertex 2
        aPos = tmp +   vec4(-sd, -sd, sd, 0.0);
        gl_Position = projection * view * aPos;
        //FragPos = vec3(aPos);
        EmitVertex();

         // vertex 3
        aPos = tmp +   vec4(-sd, sd, -sd, 0.0);
        gl_Position = projection * view * aPos;
        //FragPos = vec3(aPos);
        EmitVertex();

         // vertex 4
        aPos = tmp +   vec4(-sd, sd, sd, 0.0);
        gl_Position = projection * view * aPos;
        //FragPos = vec3(aPos);
        EmitVertex();

        EndPrimitive();
    }
} 