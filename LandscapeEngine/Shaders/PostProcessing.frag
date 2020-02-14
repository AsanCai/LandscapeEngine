#version 460 core
out vec4 FragColor;
  
in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform sampler2D cloudTEX;
uniform vec2 resolution;

void main(){
	vec4 cloud = texture(cloudTEX, TexCoords);
	vec4 bg = texture(screenTexture, TexCoords);
	
	vec4 col = mix(bg, cloud, cloud.a);

    vec2 uv = gl_FragCoord.xy / resolution;
	col.rgb *= pow( 16.0*uv.x*uv.y*(1.0-uv.x)*(1.0-uv.y), 0.11 );
	FragColor = col;
}  