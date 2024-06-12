#version 330 core
layout (location = 0) in vec4 aPos;

out vec3 ourColor;

void main()
{
   gl_Position = vec4(aPos.x, aPos.y, 0.0f, 1.0f);

   ourColor = vec3(0.9f, 0.5f, 0.1f);
}