#version 330 core
out vec4 FragColor;  
// in vec3 ourColor;
in vec3 currentPosition;
void main()
{
    FragColor = vec4(currentPosition, 1.0);
}
