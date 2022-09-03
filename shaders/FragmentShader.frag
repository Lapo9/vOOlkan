#version 450
#extension GL_KHR_vulkan_glsl : enable
// #GL_ARB_separate_shader_onjects : enable

/*
*	First implementation of the fragment shader, the idea is to start from something simple and update it as i progress.
*	For now the idea I have for the illumination of the pinball's table is to implement 6 to 8 point lights at the border of the table.
*	Another possibility is to do the same with spotlights but it could be tricky because I have to make sure they light the table correctly,
*	as for this possibility they could point to the center of the table (but it could not be enough). For the color of the lights I would opt 
*   for a plain white, but the yellow light could give the game a more "old style" type of vibe.
*	For the lights I must decide the decading factor!!!
*/

//TODO: Decide how to select all the different possible functions for either diffuse term or specular term

layout (location = 0) in vec3 fragPos;	// position of the fragment the shader is working on
layout (location = 1) in vec3 fragNormal; // coordinates of the normal vector of the fragment the shader is working on
layout (location = 2) in vec2 fragUVText; // determines where to read the texel

layout (location = 0) out vec4 outColor;  // vector containing the final color of the fragment

layout (set = 0, binding = 0) uniform sampler2D texSampler; // I assume this is used for attaching the texture to the image

// TODO: fill this struct later on
/*
*	I'm already inserting some lights that I think are what we need to illuminate the flipper's field. These consist of one directional light
*	coming from the top of the room that iluminates the entire pinball and 6 point lights at the corners of the field that simulate some led  
*	lights. Those 6 may be too much but it should be easier to eliminate some than to increase it.
*/
layout (set = 0, binding = 1) uniform GlobalUniformBufferObject {
	//Point lights!
	vec3 lightColor0;
	vec3 lightPosition0;
	vec3 lightColor1;
	vec3 lightPosition1;
	vec3 lightColor2;
	vec3 lightPosition2;
	vec3 lightColor3;
	vec3 lightPosition3;
	vec3 lightColor4;
	vec3 lightPosition4;
	vec3 lightColor5;
	vec3 lightPosition5;

	vec2 pointLightDecayFactors; //vector containing 2 floats: g and beta!

	//Direct light
	vec3 dlightColor;
	vec3 dlightDirection;

	//Ambient light
	vec3 basicAmbient;
	vec3 dxColor;
	vec3 dyColor;
	vec3 dzColor;

	vec3 eyePosition; // vector of the coordinates of the point from which we are seeing the scene
} gubo;

// Function to create the lambert diffuse vector
//TODO: see if u can simplify it in some way
vec3 create_Lambert_diffuse (vec3 N, vec3 DC, mat3 decay0to2, mat3 decay3to5) {
	vec3 result;

	result = gubo.dlightColor * DC * clamp(dot(gubo.dlightDirection, N), 0, 1); // directional light
	result += decay0to2[0] * DC * clamp(dot(normalize(gubo.lightPosition0 - fragPos), N), 0, 1); // PL 0
	result += decay0to2[1] * DC * clamp(dot(normalize(gubo.lightPosition1 - fragPos), N), 0, 1); // PL 1
	result += decay0to2[2] * DC * clamp(dot(normalize(gubo.lightPosition2 - fragPos), N), 0, 1); // PL 2
	result += decay3to5[0] * DC * clamp(dot(normalize(gubo.lightPosition3 - fragPos), N), 0, 1); // PL 3
	result += decay3to5[1] * DC * clamp(dot(normalize(gubo.lightPosition4 - fragPos), N), 0, 1); // PL 4
	result += decay3to5[2] * DC * clamp(dot(normalize(gubo.lightPosition5 - fragPos), N), 0, 1); // PL 5

	return result;
}

//Function to create the toon diffuse vector
//I have doubts for this function!
vec3 create_Toon_diffuse(vec3 N, vec3 DC, float thr, mat3 decay0to2, mat3 decay3to5) {
	vec3 result;

	//directional light
	float toConfront = dot(gubo.dlightDirection, N);
	if (toConfront <= 0) 
		return vec3(0.0);
	else if (toConfront >= thr)
		result = gubo.dlightColor;
	else 
		result = gubo.dlightColor * 0.2; // number to define for intermediate colors

	//point lights
	toConfront = dot(normalize(gubo.lightPosition0 - fragPos), N);
	if (toConfront <= 0) 
		return vec3(0.0);
	else if (toConfront >= thr)
		result += decay0to2[0];
	else 
		result += decay0to2[0] * 0.2;

	toConfront = dot(normalize(gubo.lightPosition1 - fragPos), N);
	if (toConfront <= 0) 
		return vec3(0.0);
	else if (toConfront >= thr)
		result += decay0to2[1];
	else 
		result += decay0to2[1] * 0.2;

	toConfront = dot(normalize(gubo.lightPosition2 - fragPos), N);
	if (toConfront <= 0) 
		return vec3(0.0);
	else if (toConfront >= thr)
		result += decay0to2[2];
	else 
		result += decay0to2[2] * 0.2;

	toConfront = dot(normalize(gubo.lightPosition3 - fragPos), N);
	if (toConfront <= 0) 
		return vec3(0.0);
	else if (toConfront >= thr)
		result += decay3to5[0];
	else 
		result += decay3to5[0] * 0.2;

	toConfront = dot(normalize(gubo.lightPosition4 - fragPos), N);
	if (toConfront <= 0) 
		return vec3(0.0);
	else if (toConfront >= thr)
		result += decay3to5[1];
	else 
		result += decay3to5[1] * 0.2;

	toConfront = dot(normalize(gubo.lightPosition5 - fragPos), N);
	if (toConfront <= 0) 
		return vec3(0.0);
	else if (toConfront >= thr)
		result += decay3to5[2];
	else 
		result += decay3to5[2] * 0.2;

	return result;
}

//	Function to create the Blinn specular vector
vec3 create_Blinn_specular (vec3 eyePos, vec3 N, vec3 SC, float gamma) {
	vec3 result;

	vec3 halfVectorD = normalize(gubo.dlightDirection + eyePos);
	vec3 halfVector0 = normalize(normalize(gubo.lightPosition0 - fragPos) + eyePos);
	vec3 halfVector1 = normalize(normalize(gubo.lightPosition1 - fragPos) + eyePos);
	vec3 halfVector2 = normalize(normalize(gubo.lightPosition2 - fragPos) + eyePos);
	vec3 halfVector3 = normalize(normalize(gubo.lightPosition3 - fragPos) + eyePos);
	vec3 halfVector4 = normalize(normalize(gubo.lightPosition4 - fragPos) + eyePos);
	vec3 halfVector5 = normalize(normalize(gubo.lightPosition5 - fragPos) + eyePos);

	result = gubo.dlightColor * SC * pow(clamp(dot(N, halfVectorD), 0, 1), gamma);
	result += gubo.lightColor0 * SC * pow(clamp(dot(N, halfVector0), 0, 1), gamma);
	result += gubo.lightColor1 * SC * pow(clamp(dot(N, halfVector1), 0, 1), gamma);
	result += gubo.lightColor2 * SC * pow(clamp(dot(N, halfVector2), 0, 1), gamma);
	result += gubo.lightColor3 * SC * pow(clamp(dot(N, halfVector3), 0, 1), gamma);
	result += gubo.lightColor4 * SC * pow(clamp(dot(N, halfVector4), 0, 1), gamma);
	result += gubo.lightColor5 * SC * pow(clamp(dot(N, halfVector5), 0, 1), gamma);

	return result;
}

//	Function to create the Phong specular vector
vec3 create_Phong_specular(vec3 N, vec3 eyePos, vec3 SC, float gamma) {
	vec3 result;

	//	Reflex direction for both direct and point lights
	vec3 refD = 2 * N * dot(gubo.dlightDirection, N) - gubo.dlightDirection;
	vec3 ref0 = 2 * N * dot(normalize(gubo.lightPosition0 - fragPos), N) - normalize(gubo.lightPosition0 - fragPos);
	vec3 ref1 = 2 * N * dot(normalize(gubo.lightPosition1 - fragPos), N) - normalize(gubo.lightPosition1 - fragPos);
	vec3 ref2 = 2 * N * dot(normalize(gubo.lightPosition2 - fragPos), N) - normalize(gubo.lightPosition2 - fragPos);
	vec3 ref3 = 2 * N * dot(normalize(gubo.lightPosition3 - fragPos), N) - normalize(gubo.lightPosition3 - fragPos);
	vec3 ref4 = 2 * N * dot(normalize(gubo.lightPosition4 - fragPos), N) - normalize(gubo.lightPosition4 - fragPos);
	vec3 ref5 = 2 * N * dot(normalize(gubo.lightPosition5 - fragPos), N) - normalize(gubo.lightPosition5 - fragPos);

	result = SC * pow(clamp(dot(eyePos, refD), 0.0f, 1.0f), gamma);
	result += SC * pow(clamp(dot(eyePos, ref0), 0.0f, 1.0f), gamma);
	result += SC * pow(clamp(dot(eyePos, ref1), 0.0f, 1.0f), gamma);
	result += SC * pow(clamp(dot(eyePos, ref2), 0.0f, 1.0f), gamma);
	result += SC * pow(clamp(dot(eyePos, ref3), 0.0f, 1.0f), gamma);
	result += SC * pow(clamp(dot(eyePos, ref4), 0.0f, 1.0f), gamma);
	result += SC * pow(clamp(dot(eyePos, ref5), 0.0f, 1.0f), gamma);

	return result;
}

//	Function to create the Toon specular vector
vec3 create_Toon_specular (vec3 N, vec3 eyePos, vec3 SC, float thr) {
	vec3 result = vec3(0,0,0);

	vec3 refD = 2 * N * dot(gubo.dlightDirection, N) - gubo.dlightDirection;
	float toConfront = dot(eyePos, refD);
	if (toConfront >= thr)
		result += SC * toConfront;

	vec3 ref0 = 2 * N * dot(normalize(gubo.lightPosition0 - fragPos), N) - normalize(gubo.lightPosition0 - fragPos);
	toConfront = dot(eyePos, ref0);
	if (toConfront >= thr)
		result += SC * toConfront;

	vec3 ref1 = 2 * N * dot(normalize(gubo.lightPosition1 - fragPos), N) - normalize(gubo.lightPosition1 - fragPos);
	toConfront = dot(eyePos, ref1);
	if (toConfront >= thr)
		result += SC * toConfront;

	vec3 ref2 = 2 * N * dot(normalize(gubo.lightPosition2 - fragPos), N) - normalize(gubo.lightPosition2 - fragPos);
	toConfront = dot(eyePos, ref2);
	if (toConfront >= thr)
		result += SC * toConfront;

	vec3 ref3 = 2 * N * dot(normalize(gubo.lightPosition3 - fragPos), N) - normalize(gubo.lightPosition3 - fragPos);
	toConfront = dot(eyePos, ref3);
	if (toConfront >= thr)
		result += SC * toConfront;

	vec3 ref4 = 2 * N * dot(normalize(gubo.lightPosition4 - fragPos), N) - normalize(gubo.lightPosition4 - fragPos);
	toConfront = dot(eyePos, ref4);
	if (toConfront >= thr)
		result += SC * toConfront;

	vec3 ref5 = 2 * N * dot(normalize(gubo.lightPosition5 - fragPos), N) - normalize(gubo.lightPosition5 - fragPos);
	toConfront = dot(eyePos, ref5);
	if (toConfront >= thr)
		result += SC * toConfront;

	return result;
}

//	Function to create the vectors that contain the color of the light after applying the decay
mat3 create_lights_decay_colors(float num) {
	mat3 mat;

	if (num == 0) {
		mat[0] = gubo.lightColor0 * pow((gubo.pointLightDecayFactors.x/length(gubo.lightPosition0 - fragPos)), gubo.pointLightDecayFactors.y);
		mat[1] = gubo.lightColor1 * pow((gubo.pointLightDecayFactors.x/length(gubo.lightPosition1 - fragPos)), gubo.pointLightDecayFactors.y);
		mat[2] = gubo.lightColor2 * pow((gubo.pointLightDecayFactors.x/length(gubo.lightPosition2 - fragPos)), gubo.pointLightDecayFactors.y);
	}
	else {
		mat[0] = gubo.lightColor3 * pow((gubo.pointLightDecayFactors.x/length(gubo.lightPosition3 - fragPos)), gubo.pointLightDecayFactors.y);
		mat[1] = gubo.lightColor4 * pow((gubo.pointLightDecayFactors.x/length(gubo.lightPosition4 - fragPos)), gubo.pointLightDecayFactors.y);
		mat[2] = gubo.lightColor5 * pow((gubo.pointLightDecayFactors.x/length(gubo.lightPosition5 - fragPos)), gubo.pointLightDecayFactors.y);
	}

	return mat;
}

void main() {

	vec3 normal = normalize(fragNormal); // Normalizes the normals that comes from the vertex shader which have been affected by a transformation
	vec3 viewDirection = normalize(gubo.eyePosition - fragPos); // Calculates a unitary vector that points from the fragment in xyz coordinates to the point from which we are looking it
																// Is it gubo.eyePosition.xyz?
    vec3 diffuseColor = texture(texSampler, fragUVText).rgb;

	vec3 diffuseBRDF, specularBRDF, ambientColor;
	/* 
	*	Here I put the computations for the rendering equation. I will put different models and then we can either select the one that suites
	*	best or implement some sort of function to decide which to choose while the app is running. 
	*	In the following I'll put the code for both computing the BRDF function including:
	*		- DIFFUSE part, for which I will discard Oren_Nayar a priori due to the nature of the material that compose the pinball;
	*		- SPECULAR part, for which I will discard the Ward anisotropic model;
	*		- AMBIENT part
	*/

	//	DIFFUSE FUNCTIONS

	// calcuation of the color of the pointlight after the decay comes into play, I put it into 2 3x3 matrices for convenience
	mat3 lightDecay0to2 = create_lights_decay_colors(0);
	mat3 lightDecay3to5 = create_lights_decay_colors(1);

	//	Lambert diffuse
	diffuseBRDF = create_Lambert_diffuse(normal, diffuseColor, lightDecay0to2, lightDecay3to5);

	//  Toon diffuse (thr to define!)
	//diffuseBRDF = create_Toon_diffuse(normal, diffuseColor, 0.5f, lightDecay0to2, lightDecay3to5);

	
	//	SPECULAR FUNCTIONS. Note: for specular color I'm not sure about it but for now I'll go with white (the color I thought for the lights).
	vec3 specularColor = vec3(1.0, 1.0, 1.0);
	
	//	Blinn specular
	float blinnExponent = 100.0f; // exponent to decide!
	//specularBRDF = create_Blinn_specular(viewDirection, normal, specularColor, blinnExponent);

	//	Phong specular
	float phongExponent = 100.0f;
	specularBRDF = create_Phong_specular(normal, viewDirection, specularColor, phongExponent);

	//	Toon specular (thr to define!)
	//specularBRDF = create_Toon_specular(normal, viewDirection, specularColor, 0.5f);


	//	AMBIENT COLOR FUNCTION
	//	For ambient lighting I think that spherical harmonics suits best our project but I still have to figure out how to get the proper colors!
	ambientColor = gubo.basicAmbient * (gubo.basicAmbient + normal.x * gubo.dxColor + normal.y * gubo.dyColor + normal.z * gubo.dzColor);

	//outColor = vec4(diffuseBRDF + specularBRDF + ambientColor, 1.0); 

	outColor = vec4(diffuseBRDF, 1.0f);

	//outColor = vec4(normal*0.5f+0.5f, 1.0f);

	//outColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
}

