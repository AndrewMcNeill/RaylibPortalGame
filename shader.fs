#version 330

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
in vec4 fragColor;

// Input uniform values
uniform sampler2D texture0;
uniform vec4 colDiffuse;

// Output fragment color
out vec4 finalColor;


uniform vec2 size;
uniform int pixelationAmount;


void main() {
    float pixelWidth = 1.0 / size.x;
    float pixelHeight = 1.0 / size.y;
    float aspect = pixelHeight / pixelWidth;

    float dx = pixelationAmount * pixelWidth;
    float dy = pixelationAmount * pixelHeight;

    vec2 p = fragTexCoord;
    p.x = dx * floor(p.x/dx);
    p.y = dy * floor(p.y/dy);

    finalColor = texture(texture0, p)*colDiffuse*fragColor;
}
