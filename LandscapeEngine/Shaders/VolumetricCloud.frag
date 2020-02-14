#version 460 core

// Special thanks to Nadir Rom¨¢n Guerrero https://github.com/NadirRoGue for inspiring me doing this work!
// Thanks to WFP from gamedev.net and the rest of the community who partecipated at this topic https://www.gamedev.net/forums/topic/680832-horizonzero-dawn-cloud-system/?page=6
// WFP web site http://roar11.com/
// Thanks to reinder and his beautiful Himalayas https://www.shadertoy.com/view/MdGfzh 
// Reinder https://www.shadertoy.com/user/reinder 
// Thanks to Rikard Olajos https://github.com/rikardolajos/clouds
// Thanks to Clay John https://github.com/clayjohn/realtime_clouds


in vec2 TexCoords;

uniform float FOV;
uniform vec2 iResolution;
uniform float iTime;
uniform mat4 inv_view;
uniform mat4 inv_proj;

uniform mat4 invViewProj;
uniform mat4 oldFrameVP;

uniform float u_cloudType;
uniform float u_coverage;
uniform float u_cloudSpeed;
uniform vec3 u_cloudColor;
uniform vec3 u_cameraPosition;
uniform vec3 u_sunPosition;
uniform int u_samplePoint;
uniform bool u_optimization;
uniform bool u_refresh;
uniform bool u_bayerFilter;

#define SUN_DIR normalize(u_sunPosition - u_cameraPosition)
#define CLOUD_SPEED u_cloudSpeed
#define CLOUDS_COLOR u_cloudColor
#define SAMPLE_POINTS u_samplePoint
#define REFRESH u_refresh
#define BAYER_FILTER u_bayerFilter

uniform sampler3D baseVolumeNoise;
uniform sampler3D erodeVolumeNoise;
uniform sampler2D weatherTex;
uniform sampler2D depthMap;
uniform sampler2D lastFrameAlphaness;
uniform sampler2D lastFrameColor;
uniform int frameIter;

layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec4 alphaness;
layout (location = 2) out vec4 cloudColor;

// Cone sampling random offsets
uniform vec3 noiseKernel[6u] = vec3[] (
	vec3( 0.38051305,  0.92453449, -0.02111345),
	vec3(-0.50625799, -0.03590792, -0.86163418),
	vec3(-0.32509218, -0.94557439,  0.01428793),
	vec3( 0.09026238, -0.27376545,  0.95755165),
	vec3( 0.28128598,  0.42443639, -0.86065785),
	vec3(-0.16852403,  0.14748697,  0.97460106)
);


// Cloud types height density gradients
#define STRATUS_GRADIENT vec4(0.0, 0.1, 0.2, 0.3)
#define STRATOCUMULUS_GRADIENT vec4(0.008, 0.165, 0.48, 0.625)
#define CUMULUS_GRADIENT vec4(0.02, 0.2, 0.88, 0.98)

#define EARTH_RADIUS (700000.0)
#define SPHERE_INNER_RADIUS (EARTH_RADIUS + 5000.0)
#define SPHERE_OUTER_RADIUS (SPHERE_INNER_RADIUS + 17000.0)
#define SPHERE_DELTA float(SPHERE_OUTER_RADIUS - SPHERE_INNER_RADIUS)

#define SUN_COLOR vec3(1.0, 1.0, 1.0)
#define CLOUDS_MIN_TRANSMITTANCE 0.1

// constants for atmospheric scattering
const float PI = 3.141592653589793238462643383279502884197169;

// Henyen-Greenstein
float HG( float sundotrd, float g) {
	const float k = 0.0795774715459; 
	float gg = g * g;
	return k * (1.0 - gg) / pow(1.0 + gg - 2.0 * g * sundotrd, 1.5) + 0.5;
}

vec3 sphereCenter = vec3(0.0, -EARTH_RADIUS, 0.0);
bool raySphereintersection(vec3 rayOrigin, vec3 rayDirection, float radius, out vec3 startPos){
	
	float t;

	// change the sphereCenter with camera
	sphereCenter.xz = u_cameraPosition.xz;

	float radius2 = radius*radius;

	vec3 L = rayOrigin - sphereCenter;
	float a = dot(rayDirection, rayDirection);
	float b = 2.0 * dot(rayDirection, L);
	float c = dot(L,L) - radius2;

	float discr = b*b - 4.0 * a * c;
	if(discr < 0.0) { 
		return false;
	}

	t = max(0.0, (-b + sqrt(discr))/2);
	if(t == 0.0){
		return false;
	}

	startPos = rayOrigin + rayDirection * t;
	return true;
}

// get the height fraction of cloud
float getHeightFraction(vec3 inPos){
	return (length(inPos - sphereCenter) - SPHERE_INNER_RADIUS)/(SPHERE_OUTER_RADIUS - SPHERE_INNER_RADIUS);
}

// remap original value to new value
float remap(float originalValue, float originalMin, float originalMax, float newMin, float newMax)  {
	return newMin + (((originalValue - originalMin) / (originalMax - originalMin)) * (newMax - newMin));
}

// get the density of cloud
// cloudType = 0.0			-- only stratus
// 0.0 < cloudType < 0.5	-- stratus and stratoCumulus
// cloudType = 0.5			-- only stratoCumulus
// 0.5 < cloudType < 1.0	-- stratoCumulus and cumulus
// cloudType = 1.0			-- only cumulus
float getDensityForCloud(float heightFraction, float type) {
	float stratusFactor = 1.0 - clamp(type * 2.0, 0.0, 1.0);
	float stratoCumulusFactor = 1.0 - abs(type - 0.5) * 2.0;
	float cumulusFactor = clamp(type - 0.5, 0.0, 1.0) * 2.0;

	vec4 baseGradient = stratusFactor * STRATUS_GRADIENT + stratoCumulusFactor * STRATOCUMULUS_GRADIENT + cumulusFactor * CUMULUS_GRADIENT;

	// gradicent computation (see Siggraph 2017 Nubis-Decima talk)
	return (smoothstep(baseGradient.x, baseGradient.y, heightFraction) - smoothstep(baseGradient.z, baseGradient.w, heightFraction)) / heightFraction;

}

float threshold(float v, float t) {
	return v > t ? v : 0.0;
}

const vec3 windDirection = normalize(vec3(0.5, 0.0, 0.1));

vec2 getUVProjection(vec3 pos){
	return pos.xz/SPHERE_INNER_RADIUS + 0.5;
//	return pos.xz/SPHERE_OUTER_RADIUS + 0.5;
}

#define CLOUD_SCALE 40.0

// create the shape of cloud
float sampleCloudDensity(vec3 pos, bool expensive){
	float heightFraction = getHeightFraction(pos);
	vec2 uv = getUVProjection(pos);

	// just draw cloud between SPHERE_INNER_RADIUS and SPHERE_OUTER_RADIUS
	if(heightFraction < 0.0 || heightFraction > 1.0){
		return 0.0;
	}

	// use Worley noise at increasing frequencies to remap the Perlin - Worley noise
	vec4 base_noise = texture(baseVolumeNoise, vec3(uv * CLOUD_SCALE, heightFraction));
	float lowFreqFBM = dot(base_noise.gba, vec3(0.625, 0.25, 0.125));
	float base_cloud = remap(base_noise.r, 1.0 - lowFreqFBM, 1.0, 0.0 , 1.0);
	
	// calculate the animation
	vec3 animation =  windDirection * iTime * CLOUD_SPEED;
	vec2 moving_uv = getUVProjection(pos + animation);
	// change coverage 
	vec3 weather_data = texture(weatherTex, moving_uv).rgb;

	float coverageFactor = weather_data.r * u_coverage;
	float typeFactor = getDensityForCloud(heightFraction, u_cloudType);
	float cloud = base_cloud * coverageFactor * typeFactor;

	// whether to use worley noise to modify the base cloud shape
	if(expensive){
		vec3 erode_uv = vec3(moving_uv * CLOUD_SCALE, heightFraction) * 0.1;
		vec3 erodeCloudNoise = texture(erodeVolumeNoise, erode_uv).rgb;
		float highFreqFBM = dot(erodeCloudNoise.rgb, vec3(0.625, 0.25, 0.125));
		float modifyFactor = clamp(heightFraction * 10.0, 0.0, 1.0);
		float modifier = mix(highFreqFBM, 1.0 - highFreqFBM, modifyFactor);

		cloud = cloud - modifier * (1.0 - cloud);
		cloud = remap(cloud * 2.0, modifier * 0.2, 1.0, 0.0, 1.0);
	}

	return clamp(cloud, 0.0, 1.0);
}

float raymarchToLight(vec3 startPos, float stepSize, vec3 lightDir) {
	float ds = stepSize * 0.6;
	vec3 rayStep = lightDir * ds;
	const float CONE_STEP = 1.0/6.0;
	float coneRadius = 1.0; 
	float density = 0.0;
	float coneDensity = 0.0;

	const float absorption = 0.0035;
	float sigma_ds = -ds*absorption;

	vec3 pos;

	float T = 1.0;

	for(int i = 0; i < 6; i++) {
		pos = startPos + coneRadius*noiseKernel[i]*float(i);

		float heightFraction = getHeightFraction(pos);
		if(heightFraction >= 0) {
			
			float cloudDensity = sampleCloudDensity(pos, density > 0.3);
			if(cloudDensity > 0.0) {
				T *= exp(cloudDensity*sigma_ds);
				density += cloudDensity;
			}
		}
		startPos += rayStep;
		coneRadius += CONE_STEP;
	}

	return T;
}

#define BAYER_FACTOR 1.0/16.0
uniform float bayerFilter[16u] = float[] (
	0.0*BAYER_FACTOR, 8.0*BAYER_FACTOR, 2.0*BAYER_FACTOR, 10.0*BAYER_FACTOR,
	12.0*BAYER_FACTOR, 4.0*BAYER_FACTOR, 14.0*BAYER_FACTOR, 6.0*BAYER_FACTOR,
	3.0*BAYER_FACTOR, 11.0*BAYER_FACTOR, 1.0*BAYER_FACTOR, 9.0*BAYER_FACTOR,
	15.0*BAYER_FACTOR, 7.0*BAYER_FACTOR, 13.0*BAYER_FACTOR, 5.0*BAYER_FACTOR
);

vec4 raymarchToCloud(vec3 startPos, vec3 endPos, vec3 bg){
	vec3 path = endPos - startPos;
	float len = length(path);

	// max steps
	const int nSteps = SAMPLE_POINTS;
	
	float ds = len/nSteps;
	vec3 dir = path/len;
	dir *= ds;

	vec4 col = vec4(0.0);
	if(BAYER_FILTER) {
		// reduce banding artifacts that might appear due to undersampling
		int a = int(gl_FragCoord.x) % 4;
		int b = int(gl_FragCoord.y) % 4;
		startPos += dir * bayerFilter[a * 4 + b];
	}

	vec3 pos = startPos;

	float density = 0.0;

	float lightDotEye = dot(normalize(SUN_DIR), normalize(dir));

	float T = 1.0;
	const float absorption = 0.01;
	float sigma_ds = -ds*absorption;

	for(int i = 0; i < nSteps; ++i) {	
		// calculate the density of the sample point
		float density_sample = sampleCloudDensity(pos, true);

		if(density_sample > 0.0) {
			float height = getHeightFraction(pos);

			float light_density = raymarchToLight(pos, ds, SUN_DIR);
			
			float phase = HG(lightDotEye, 0.08);

			vec3 baseColor = mix(CLOUDS_COLOR ,mix( bg, SUN_COLOR, 0.8), light_density);
			float attenuation = exp(density_sample * sigma_ds);// beer's law
			float powder = 1 - attenuation;
			float lightOut = T * powder * phase;

			col.rgb += lightOut * baseColor;
			// light attenuation
			T *= attenuation;
		}

		if( T <= CLOUDS_MIN_TRANSMITTANCE ) break;

		pos += dir;
	}
	
	col.a = 1.0 - T;
	
	return col;
}

// iResolution stores the resolution of screen
// gl_FragCoord represents the coordinates of the current pixel
vec3 computeClipSpaceCoord(vec2 pixelCoord){
	// calculate the nds coordinate
	vec2 ray_nds = 2.0 * pixelCoord / iResolution - 1.0;
	return vec3(ray_nds, 1.0);
}

// change ndc's (-1. 1) to screen's£¨0, 1£©
vec2 computeScreenPos(vec2 ndc){
	return (ndc * 0.5 + 0.5);
}

// decide whether you to render the cloud
float computeFogAmount(in vec3 startPos, in float factor = 0.00006){
	float dist = length(startPos - u_cameraPosition);
	float radius = (u_cameraPosition.y - sphereCenter.y) * 0.3;
	float alpha = (dist / radius);

	float amount = 1.0 - exp(-dist * alpha * factor);
	return amount;
}

uniform int bayerMatrix16[16] = int[] (
	0, 8, 2, 10,
	12, 4, 14, 6,
	3, 11, 1, 9,
	15, 7, 13, 5
);


bool writePixel(){
	int index = bayerMatrix16[frameIter];
	ivec2 icoord = ivec2(gl_FragCoord.xy);
    return ((icoord.x + 4*icoord.y) % 16 == index);
}

bool intersectCubeMap(vec3 o, vec3 d, out vec3 minT, out vec3 maxT) {		
	vec3 cubeMin = vec3(-0.5, -0.5, -0.5);
	vec3 cubeMax = vec3(0.5, 1 + cubeMin.y, 0.5);

	// compute intersection of ray with all six bbox planes
	vec3 invR = 1.0 / d;
	vec3 tbot = invR * (cubeMin - o);
	vec3 ttop = invR * (cubeMax - o);
	// re-order intersections to find smallest and largest on each axis
	vec3 tmin = min (ttop, tbot);
	vec3 tmax = max (ttop, tbot);
	// find the largest tmin and the smallest tmax
	vec2 t0 = max (tmin.xx, tmin.yz);
	float tnear = max (t0.x, t0.y);
	t0 = min (tmax.xx, tmax.yz);
	float tfar = min (t0.x, t0.y);
	
	// check for hit
	bool hit;
	if ((tnear > tfar) || tfar < 0.0) {
		hit = false;
	} else {
		hit = true;
	}

	// if we are inside the bb, start point is cam pos
	minT = tnear < 0.0? o : o + d * tnear; 
	maxT = o + d * tfar;

	return hit;
}

// calculate the color of background sky    
vec4 colorCubeMap(vec3 endPos, vec3 d) {
	float sun = clamp( dot(SUN_DIR, d), 0.0, 1.0 );
	vec3 col = vec3(0.6, 0.71, 0.85) - endPos.y * 0.2 * vec3(1.0,0.5,1.0) + 0.15*0.5;

	// calculate the sun color
	col += 0.8 * vec3(1.0, 0.6, 0.1) * pow(sun, 256.0);
	return vec4(col, 1.0);
}

void main() {
	// compute ray direction in world space
	vec4 ray_clip = vec4(computeClipSpaceCoord(gl_FragCoord.xy), 1.0);
	vec4 ray_view = inv_proj * ray_clip;
	ray_view = vec4(ray_view.xy, -1.0, 0.0);
	vec3 worldDir = normalize((inv_view * ray_view).xyz);

	// get the position of camera in world space 
	vec4 camToWorldPos = invViewProj * ray_clip;
	camToWorldPos /= camToWorldPos.w;

	// for picking previous frame color -- temporal projection
	vec4 pPrime = oldFrameVP * camToWorldPos;
	pPrime /= pPrime.w;
	vec2 prevFrameScreenPos = computeScreenPos(pPrime.xy); 
	
	// compute background color
	vec3 stub, cubeMapEndPos;
	intersectCubeMap(vec3(0.0, 0.0, 0.0), worldDir, stub, cubeMapEndPos);
	vec4 bg = colorCubeMap(cubeMapEndPos, worldDir);
	vec3 red = vec3(1.0, 0.8, 0.8);
	bg = mix( mix(red.rgbr, vec4(SUN_COLOR.rgb, 1.0), SUN_DIR.y), bg, pow( max(cubeMapEndPos.y, 0), 0.2));


	// compute raymarching starting and ending point (starting point and ending point to render cloud)
	vec3 startPos, endPos;
	raySphereintersection(u_cameraPosition, worldDir, SPHERE_INNER_RADIUS, startPos);
	raySphereintersection(u_cameraPosition, worldDir, SPHERE_OUTER_RADIUS, endPos);

	// compute fog amount and early exit if over a certain value
	float fogAmount = computeFogAmount(startPos, 0.00002);
	if(fogAmount > 0.965){
		fragColor = bg;
		return;
	}

	//early exit -- search for low alphaness areas
	float oldFrameAlphaness = 1.0;
	bool isOut = any(greaterThan(abs(prevFrameScreenPos - 0.5) , vec2(0.5)));
	if(!isOut){
		oldFrameAlphaness = texture(lastFrameAlphaness, prevFrameScreenPos).r;
	}

	vec4 volume = vec4(0.0);

	if(u_optimization) {
		if(oldFrameAlphaness >= 0.0 || frameIter == 0){
			if(isOut || REFRESH || writePixel()) {
				// if the pixel must be drawn
				volume = raymarchToCloud(startPos,endPos, bg.rgb);
				cloudColor = volume;
			} else {
				// else do temporal reprojection
				volume = texture(lastFrameColor, prevFrameScreenPos);  
				cloudColor = volume;
			}
		} else {
			// else do temporal reprojection
			volume = texture(lastFrameColor, prevFrameScreenPos);  
			cloudColor = volume;
		}
	} else {
		// if optimization is disabled, then only this function will be executed
		volume = raymarchToCloud(startPos,endPos, bg.rgb);
		cloudColor = volume;
	}

	float cloudAlphaness = threshold(volume.a, 0.2);
	// contrast-illumination tuning
	volume.rgb = volume.rgb * 1.8 - 0.1; 


	// use current position distance to center as action radius
    volume.rgb = mix(volume.rgb, bg.rgb * volume.a, clamp(fogAmount, 0.0, 1.0));

	// add sun glare to clouds
	float sun = clamp(dot(SUN_DIR, normalize(endPos - startPos)), 0.0, 1.0);
	vec3 s = 0.8 * vec3(1.0, 0.4, 0.2) * pow(sun, 256.0);
	volume.rgb += s * volume.a;

	// blend clouds and background
	bg.rgb = bg.rgb * (1.0 - volume.a) + volume.rgb;
	bg.a = 1.0;

	fragColor = bg;
	alphaness = vec4(cloudAlphaness, 0.0, 0.0, 1.0); // alphaness buffer

	// erase pixel if it's occluded
	if(texture(depthMap, TexCoords).r < 1.0) {
		fragColor = vec4(0.0);
		return;
	}
}