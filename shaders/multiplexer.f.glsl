#extension GL_EXT_gpu_shader4: enable

uniform sampler2DArray viewTextures;
uniform int numViews;
uniform float angle;

void main(void)
{
    vec4 colorSample;
    vec3 textureCoord;
    float PI = 3.14159265358979323846264;
    gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);
   
    float v_interval = float(numViews) / 3.0 / tan(angle * PI / 180.0);
    float v_view = floor(mod(gl_FragCoord.y - 1.0, v_interval) * float(numViews) / v_interval);
    float h_view = mod((gl_FragCoord.x * 3.0) - 2.0 - v_view, float(numViews));
    
    // Evaluate red pixel 
    int r_view = int(floor(h_view));
    textureCoord.st = gl_TexCoord[0].st;
    textureCoord.p = float(r_view);
    colorSample = texture2DArray(viewTextures, textureCoord);
    gl_FragColor.r = colorSample.r;
    
    // Evaluate green pixel
    int g_view = r_view + 1;
    textureCoord.p = float(g_view);
    colorSample = texture2DArray(viewTextures, textureCoord);
    gl_FragColor.g = colorSample.g;
   
    // Evaluate blue pixel
    int b_view = g_view + 1;
    textureCoord.p = float(b_view);
    colorSample = texture2DArray(viewTextures, textureCoord);
    gl_FragColor.b = colorSample.b;
}
