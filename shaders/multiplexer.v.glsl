attribute vec3 v_color;
void main(void)
{
    gl_TexCoord[0] = gl_MultiTexCoord0;

    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
