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
uniform vec2 portalPositions[8];
uniform int portalDistanceOrder[8];
uniform int numPortals;
uniform vec2 playerPosition;


void main() {
    float pixelWidth = 1.0 / size.x;
    float pixelHeight = 1.0 / size.y;
    float aspect = pixelHeight / pixelWidth;
    vec2 p = fragTexCoord;
    vec2 playerUVPosition = vec2(playerPosition.x * pixelWidth, playerPosition.y * pixelHeight);

    int numSolutions = -1;
    float dx, dy, A, B, C, det, t;
    vec2 intersection1, intersection2;
    float radius = 30*pixelWidth;
    bool solutionFound = false;

    for (int i = 0; i < numPortals; i++) {
        int j = portalDistanceOrder[i];

        float cx = portalPositions[j].x * pixelWidth;
        float cy = portalPositions[j].y * pixelHeight;

        

        if (!solutionFound) {
            dx = playerUVPosition.x - p.x;
            dy = playerUVPosition.y - p.y;
            
            A = dx * dx + dy * dy;
            B = 2 * (dx * (p.x - cx) + dy * (p.y - cy));
            C = (p.x - cx) * (p.x - cx) + (p.y - cy) * (p.y - cy) - radius * radius;

            det = B * B - 4 * A * C;
            
            if ((A <= 0.0000001) || (det < 0))
            {
                // No real solutions.
                numSolutions = 0;
            }
            else if (det == 0)
            {
                // One solution.
                t = -B / (2 * A);
                intersection1 = vec2(p.x + t * dx, p.y + t * dy);
                numSolutions = 1;
            }
            else
            {
                // Two solutions.
                t = ((-B + sqrt(det)) / (2 * A));
                intersection1 = vec2(p.x + t * dx, p.y + t * dy);
                t = ((-B - sqrt(det)) / (2 * A));
                intersection2 = vec2(p.x + t * dx, p.y + t * dy);
                numSolutions = 2;
            }

            if (numSolutions >= 1 && 
                abs(p.x-playerUVPosition.x) >= abs(playerUVPosition.x-intersection2.x) && 
                abs(p.y-playerUVPosition.y) >= abs(playerUVPosition.y-intersection2.y) &&
                abs(p.x-intersection2.x) < abs(p.x-playerUVPosition.x) &&
                abs(p.y-intersection2.y) < abs(p.y-playerUVPosition.y)){
                
                
                solutionFound = true;
            }
        }

        /*if (abs(playerUVPosition.x - cx) <= radius && abs(playerUVPosition.y - cy) <= radius) {
            solutionFound = true;
        }*/

        if (solutionFound) {
            int portalIndexOffset = ((j % 2) * 2 - 1)*-1;
            vec2 newPos = vec2(
                p.x - cx + portalPositions[j+portalIndexOffset].x * pixelWidth,
                p.y - cy + portalPositions[j+portalIndexOffset].y * pixelHeight);
            if (newPos.x < 0 || newPos.x > 1 || newPos.y < 0 || newPos.y > 1) {
                finalColor = vec4(0.0,0.0,0.0,1.0);
            } else {
                finalColor = texture(texture0, newPos)*colDiffuse*fragColor;
            }

            break;
        }
    }
    if (!solutionFound) {
        finalColor = texture(texture0, p)*colDiffuse*fragColor;
    }
}
