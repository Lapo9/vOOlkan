#version 450
#extension GL_KHR_vulkan_glsl : enable
// #GL_ARB_separate_shader_onjects : enable

const int toon_color_lvls = 5;
const float toon_scale_factor = 1.0 / toon_color_lvls;
/*
*	First implementation of the fragment shader, the idea is to start from something simple and update it as i progress.
*	For now the idea I have for the illumination of the pinball's table is to implement 6 to 8 point lights at the border of the table.
*	Another possibility is to do the same with spotlights but it could be tricky because I have to make sure they light the table correctly,
*	as for this possibility they could point to the center of the table (but it could not be enough). For the color of the lights I would opt 
*   for a plain white, but the yellow light could give the game a more "old style" type of vibe.
*	For the lights I must decide the decading factor!!!
*
*	The final decision has been to use only the directional light for the field and use 8 pointlights in total!
*	5 of which are used to light up the 5 bumpers, the remaining 3 are used to light up the balls.
*/

layout (location = 0) in vec3 fragPos;	// position of the fragment the shader is working on
layout (location = 1) in vec3 fragNormal; // coordinates of the normal vector of the fragment the shader is working on
layout (location = 2) in vec2 fragUVText; // determines where to read the texel

layout (location = 0) out vec4 outColor;  // vector containing the final color of the fragment

layout (set = 0, binding = 0) uniform sampler2D texSampler; // This is used for attaching the texture to the image

/*
*	I'm already inserting some lights that I think are what we need to illuminate the flipper's field. These consist of one directional light
*	coming from the top of the room that iluminates the entire pinball and 6 point lights at the corners of the field that simulate some led  
*	lights. Those 6 may be too much but it should be easier to eliminate some than to increase it.
*
*	In the end we decided to use 1 directional light and 8 point lights. The number of point lights is 9 in total in case we want to use one more
*/
layout (set = 0, binding = 1) uniform GlobalUniformBufferObject {
	//Point lights for lights inside the pinball
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

	vec2 pointLightDecayFactors0to4; //vector containing 2 floats: g and beta!
	vec2 pointLightDecayFactors5to8; //vector containing 2 floats: g and beta!

	//Auxiliary point lights for other elements in the scene
	vec3 auxLightColor0;
	vec3 auxLightPos0;
	vec3 auxLightColor1;
	vec3 auxLightPos1;
	vec3 auxLightColor2;
	vec3 auxLightPos2;

	//Direct light for the light coming from the top
	vec3 dlightColor;
	vec3 dlightDirection;

	//Ambient light
	vec3 basicAmbient; //Basic ambient color	 RGB: 225, 225, 225
	vec3 floorColor; //Color of the base of the pinball		RGB: 229, 215, 166
	vec3 skyColor;	//Color of the sky	RGB: 125, 59, 240
	vec3 definingDirection; //Direction accounted for the computation of the ambient color

	vec3 eyePosition; // vector of the coordinates of the point from which we are seeing the scene

	 /*
	 *	This vector is used to decide which light functions to use:
	 *	functionDecider.x : 0.0 is used for Lambert diffuse, 1.0 is used for Toon diffuse
	 *	functionDecider.y : 0.0 is used for Blinn specular, 1.0 is used for Toon specular, 2.0 is used for Phong specular
	 */
	vec2 functionDecider;
} gubo;

// Function to create the lambert diffuse vector
vec3 create_Lambert_diffuse (vec3 N, vec3 DC, mat3 decay0to2, mat3 decay3to5, mat3 auxDecay0to2) {
	vec3 result;

	result = gubo.dlightColor * DC * clamp(dot(gubo.dlightDirection, N), 0, 1); // directional light
	result += decay0to2[0] * DC * clamp(dot(normalize(gubo.lightPosition0 - fragPos), N), 0, 1); // PL 0
	result += decay0to2[1] * DC * clamp(dot(normalize(gubo.lightPosition1 - fragPos), N), 0, 1); // PL 1
	result += decay0to2[2] * DC * clamp(dot(normalize(gubo.lightPosition2 - fragPos), N), 0, 1); // PL 2
	result += decay3to5[0] * DC * clamp(dot(normalize(gubo.lightPosition3 - fragPos), N), 0, 1); // PL 3
	result += decay3to5[1] * DC * clamp(dot(normalize(gubo.lightPosition4 - fragPos), N), 0, 1); // PL 4
	result += decay3to5[2] * DC * clamp(dot(normalize(gubo.lightPosition5 - fragPos), N), 0, 1); // PL 5
	result += auxDecay0to2[0] * DC * clamp(dot(normalize(gubo.auxLightPos0 - fragPos), N), 0, 1); // APL 0
	result += auxDecay0to2[1] * DC * clamp(dot(normalize(gubo.auxLightPos1 - fragPos), N), 0, 1); // APL 1
	result += auxDecay0to2[2] * DC * clamp(dot(normalize(gubo.auxLightPos2 - fragPos), N), 0, 1); // APL 2

	return result;
}

//Function to create the toon diffuse vector
vec3 create_Toon_diffuse(vec3 N, vec3 DC, float thr, mat3 decay0to2, mat3 decay3to5, mat3 ALD) {
	vec3 result = vec3(0.0);
	
	//directional light
	float toConfront = dot(N, gubo.dlightDirection);
	if (toConfront <= 0.0) 
		result += vec3(0.0);
	else
		result += (floor(toConfront * toon_color_lvls) * toon_scale_factor) * gubo.dlightColor * DC;

	//point lights
	toConfront = dot(normalize(gubo.lightPosition0 - fragPos), N);
	if (toConfront <= 0) 
		result += vec3(0.0);
	else 
		result += (floor(toConfront * toon_color_lvls) * toon_scale_factor) * decay0to2[0] * DC;

	toConfront = dot(normalize(gubo.lightPosition1 - fragPos), N);
	if (toConfront <= 0) 
		result += vec3(0.0);
	else 
		result += (floor(toConfront * toon_color_lvls) * toon_scale_factor) * decay0to2[1] * DC;

	toConfront = dot(normalize(gubo.lightPosition2 - fragPos), N);
	if (toConfront <= 0) 
		result += vec3(0.0);
	else 
		result += (floor(toConfront * toon_color_lvls) * toon_scale_factor) * decay0to2[2] * DC;

	toConfront = dot(normalize(gubo.lightPosition3 - fragPos), N);
	if (toConfront <= 0) 
		result += vec3(0.0);
	else 
		result += (floor(toConfront * toon_color_lvls) * toon_scale_factor) * decay3to5[0] * DC;

	toConfront = dot(normalize(gubo.lightPosition4 - fragPos), N);
	if (toConfront <= 0) 
		result += vec3(0.0);
	else 
		result += (floor(toConfront * toon_color_lvls) * toon_scale_factor) * decay3to5[1] * DC;

	toConfront = dot(normalize(gubo.lightPosition5 - fragPos), N);
	if (toConfront <= 0) 
		result += vec3(0.0);
	else 
		result += (floor(toConfront * toon_color_lvls) * toon_scale_factor) * decay3to5[2] * DC;

		//aux lights
	toConfront = dot(normalize(gubo.auxLightPos0 - fragPos), N);
	if (toConfront <= 0) 
		result += vec3(0.0);
	else 
		result += (floor(toConfront * toon_color_lvls) * toon_scale_factor) * ALD[0] * DC;

	toConfront = dot(normalize(gubo.lightPosition1 - fragPos), N);
	if (toConfront <= 0) 
		result += vec3(0.0);
	else 
		result += (floor(toConfront * toon_color_lvls) * toon_scale_factor) * ALD[1] * DC;

	toConfront = dot(normalize(gubo.lightPosition2 - fragPos), N);
	if (toConfront <= 0) 
		result += vec3(0.0);
	else 
		result += (floor(toConfront * toon_color_lvls) * toon_scale_factor) * ALD[2] * DC;

	return result;
}

//	Function to create the Blinn specular vector
vec3 create_Blinn_specular (vec3 eyePos, vec3 N, vec3 SC, mat3 MSC0, mat3 MSC1, mat3 ASC, float gamma) {
	vec3 result;

	vec3 halfVectorD = normalize(gubo.dlightDirection + eyePos);
	vec3 halfVector0 = normalize(normalize(gubo.lightPosition0 - fragPos) + eyePos);
	vec3 halfVector1 = normalize(normalize(gubo.lightPosition1 - fragPos) + eyePos);
	vec3 halfVector2 = normalize(normalize(gubo.lightPosition2 - fragPos) + eyePos);
	vec3 halfVector3 = normalize(normalize(gubo.lightPosition3 - fragPos) + eyePos);
	vec3 halfVector4 = normalize(normalize(gubo.lightPosition4 - fragPos) + eyePos);
	vec3 halfVector5 = normalize(normalize(gubo.lightPosition5 - fragPos) + eyePos);
	vec3 auxHalfVector0 = normalize(normalize(gubo.auxLightPos0 - fragPos) + eyePos);
	vec3 auxHalfVector1 = normalize(normalize(gubo.auxLightPos1 - fragPos) + eyePos);
	vec3 auxHalfVector2 = normalize(normalize(gubo.auxLightPos2 - fragPos) + eyePos);

	result = gubo.dlightColor * SC * pow(clamp(dot(N, halfVectorD), 0, 1), gamma);
	result += gubo.lightColor0 * MSC0[0] * pow(clamp(dot(N, halfVector0), 0, 1), gamma);
	result += gubo.lightColor1 * MSC0[1] * pow(clamp(dot(N, halfVector1), 0, 1), gamma);
	result += gubo.lightColor2 * MSC0[2] * pow(clamp(dot(N, halfVector2), 0, 1), gamma);
	result += gubo.lightColor3 * MSC1[0] * pow(clamp(dot(N, halfVector3), 0, 1), gamma);
	result += gubo.lightColor4 * MSC1[1] * pow(clamp(dot(N, halfVector4), 0, 1), gamma);
	result += gubo.lightColor5 * MSC1[2] * pow(clamp(dot(N, halfVector5), 0, 1), gamma);
	result += gubo.auxLightColor0 * ASC[0] * pow(clamp(dot(N, auxHalfVector0), 0, 1), gamma);
	result += gubo.auxLightColor1 * ASC[1] * pow(clamp(dot(N, auxHalfVector1), 0, 1), gamma);
	result += gubo.auxLightColor2 * ASC[2] * pow(clamp(dot(N, auxHalfVector2), 0, 1), gamma);

	return result;
}

//	Function to create the Phong specular vector
vec3 create_Phong_specular(vec3 N, vec3 eyePos, mat3 MSC0, mat3 MSC1, vec3 SC, mat3 ASC, float gamma) {
	vec3 result;

	float lightAttenuation = 0.025; // parameter used to lower the intensity of the reflex coming from the directional light

	//	Reflex direction for both direct and point lights
	vec3 refD = 2 * dot(gubo.dlightDirection, N) * N - gubo.dlightDirection;
	vec3 ref0 = 2 * dot(normalize(gubo.lightPosition0 - fragPos), N) * N - normalize(gubo.lightPosition0 - fragPos);
	vec3 ref1 = 2 * dot(normalize(gubo.lightPosition1 - fragPos), N) * N - normalize(gubo.lightPosition1 - fragPos);
	vec3 ref2 = 2 * dot(normalize(gubo.lightPosition2 - fragPos), N) * N - normalize(gubo.lightPosition2 - fragPos);
	vec3 ref3 = 2 * dot(normalize(gubo.lightPosition3 - fragPos), N) * N - normalize(gubo.lightPosition3 - fragPos);
	vec3 ref4 = 2 * dot(normalize(gubo.lightPosition4 - fragPos), N) * N - normalize(gubo.lightPosition4 - fragPos);
	vec3 ref5 = 2 * dot(normalize(gubo.lightPosition5 - fragPos), N) * N - normalize(gubo.lightPosition5 - fragPos);
	vec3 aref0 = 2 * dot(normalize(gubo.auxLightPos0 - fragPos), N) * N - normalize(gubo.auxLightPos0 - fragPos);
	vec3 aref1 = 2 * dot(normalize(gubo.auxLightPos1 - fragPos), N) * N - normalize(gubo.auxLightPos1 - fragPos);
	vec3 aref2 = 2 * dot(normalize(gubo.auxLightPos2 - fragPos), N) * N - normalize(gubo.auxLightPos2 - fragPos);

	result = SC * lightAttenuation * pow(clamp(dot(eyePos, refD), 0.0f, 1.0f), gamma);
	result += MSC0[0] * pow(clamp(dot(eyePos, ref0), 0.0f, 1.0f), gamma);
	result += MSC0[1] * pow(clamp(dot(eyePos, ref1), 0.0f, 1.0f), gamma);
	result += MSC0[2] * pow(clamp(dot(eyePos, ref2), 0.0f, 1.0f), gamma);
	result += MSC1[0] * pow(clamp(dot(eyePos, ref3), 0.0f, 1.0f), gamma);
	result += MSC1[1] * pow(clamp(dot(eyePos, ref4), 0.0f, 1.0f), gamma);
	result += MSC1[2] * pow(clamp(dot(eyePos, ref5), 0.0f, 1.0f), gamma);
	result += ASC[0] * pow(clamp(dot(eyePos, aref0), 0.0f, 1.0f), gamma);
	result += ASC[1] * pow(clamp(dot(eyePos, aref1), 0.0f, 1.0f), gamma);
	result += ASC[2] * pow(clamp(dot(eyePos, aref2), 0.0f, 1.0f), gamma);

	return result;
}

//	Function to create the Toon specular vector, it uses a constant parameter for the number of color levels, for now it is set to 5
vec3 create_Toon_specular (vec3 N, vec3 eyePos, vec3 SC, mat3 MSC0, mat3 MSC1, mat3 ASC, float thr) {
	vec3 result = vec3(0,0,0);
	float dlightAttenuation = 0.05; //attenuation for the specular color of the direct light
	float plightAttenuation = 0.3; //attenuation for the specular color of the point lights

	vec3 refD = 2 * N * dot(gubo.dlightDirection, N) - gubo.dlightDirection;
	float toConfront = dot(eyePos, refD);
	if (toConfront > thr)
		result += SC * toConfront * (floor(toConfront * toon_color_lvls) * toon_scale_factor) * dlightAttenuation;

	vec3 ref0 = 2 * N * dot(normalize(gubo.lightPosition0 - fragPos), N) - normalize(gubo.lightPosition0 - fragPos);
	toConfront = dot(eyePos, ref0);
	if (toConfront > thr)
		result += MSC0[0] * toConfront * (floor(toConfront * toon_color_lvls) * toon_scale_factor) * plightAttenuation;

	vec3 ref1 = 2 * N * dot(normalize(gubo.lightPosition1 - fragPos), N) - normalize(gubo.lightPosition1 - fragPos);
	toConfront = dot(eyePos, ref1);
	if (toConfront > thr)
		result += MSC0[1] * toConfront * (floor(toConfront * toon_color_lvls) * toon_scale_factor) * plightAttenuation;

	vec3 ref2 = 2 * N * dot(normalize(gubo.lightPosition2 - fragPos), N) - normalize(gubo.lightPosition2 - fragPos);
	toConfront = dot(eyePos, ref2);
	if (toConfront > thr)
		result += MSC0[2] * toConfront * (floor(toConfront * toon_color_lvls) * toon_scale_factor) * plightAttenuation;

	vec3 ref3 = 2 * N * dot(normalize(gubo.lightPosition3 - fragPos), N) - normalize(gubo.lightPosition3 - fragPos);
	toConfront = dot(eyePos, ref3);
	if (toConfront > thr)
		result += MSC1[0] * toConfront * (floor(toConfront * toon_color_lvls) * toon_scale_factor) * plightAttenuation;

	vec3 ref4 = 2 * N * dot(normalize(gubo.lightPosition4 - fragPos), N) - normalize(gubo.lightPosition4 - fragPos);
	toConfront = dot(eyePos, ref4);
	if (toConfront > thr)
		result += MSC1[1] * toConfront * (floor(toConfront * toon_color_lvls) * toon_scale_factor) * plightAttenuation;

	vec3 ref5 = 2 * N * dot(normalize(gubo.lightPosition5 - fragPos), N) - normalize(gubo.lightPosition5 - fragPos);
	toConfront = dot(eyePos, ref5);
	if (toConfront > thr)
		result += MSC1[2] * toConfront * (floor(toConfront * toon_color_lvls) * toon_scale_factor) * plightAttenuation;

	vec3 aref0 = 2 * N * dot(normalize(gubo.auxLightPos0 - fragPos), N) - normalize(gubo.auxLightPos0 - fragPos);
	toConfront = dot(eyePos, aref0);
	if (toConfront > thr)
		result += ASC[0] * toConfront * (floor(toConfront * toon_color_lvls) * toon_scale_factor) * plightAttenuation;

	vec3 aref1 = 2 * N * dot(normalize(gubo.auxLightPos1 - fragPos), N) - normalize(gubo.auxLightPos1 - fragPos);
	toConfront = dot(eyePos, aref1);
	if (toConfront > thr)
		result += ASC[1] * toConfront * (floor(toConfront * toon_color_lvls) * toon_scale_factor) * plightAttenuation;

	vec3 aref2 = 2 * N * dot(normalize(gubo.auxLightPos2 - fragPos), N) - normalize(gubo.auxLightPos2 - fragPos);
	toConfront = dot(eyePos, aref2);
	if (toConfront > thr)
		result += ASC[2] * toConfront * (floor(toConfront * toon_color_lvls) * toon_scale_factor) * plightAttenuation;

	return result;
}

//	Function to create the vectors that contain the color of the light after applying the decay
mat3 create_lights_decay_colors(float num) {
	mat3 mat;

	if (num == 0) {
		mat[0] = gubo.lightColor0 * pow((gubo.pointLightDecayFactors0to4.x/length(gubo.lightPosition0 - fragPos)), gubo.pointLightDecayFactors0to4.y);
		mat[1] = gubo.lightColor1 * pow((gubo.pointLightDecayFactors0to4.x/length(gubo.lightPosition1 - fragPos)), gubo.pointLightDecayFactors0to4.y);
		mat[2] = gubo.lightColor2 * pow((gubo.pointLightDecayFactors0to4.x/length(gubo.lightPosition2 - fragPos)), gubo.pointLightDecayFactors0to4.y);
	}
	else if (num == 1) {
		mat[0] = gubo.lightColor3 * pow((gubo.pointLightDecayFactors0to4.x/length(gubo.lightPosition3 - fragPos)), gubo.pointLightDecayFactors0to4.y);
		mat[1] = gubo.lightColor4 * pow((gubo.pointLightDecayFactors0to4.x/length(gubo.lightPosition4 - fragPos)), gubo.pointLightDecayFactors0to4.y);
		mat[2] = gubo.lightColor5 * pow((gubo.pointLightDecayFactors5to8.x/length(gubo.lightPosition5 - fragPos)), gubo.pointLightDecayFactors5to8.y);
	}
	else {
		mat[0] = gubo.auxLightColor0 * pow((gubo.pointLightDecayFactors5to8.x/length(gubo.auxLightPos0 - fragPos)), gubo.pointLightDecayFactors5to8.y);
		mat[1] = gubo.auxLightColor1 * pow((gubo.pointLightDecayFactors5to8.x/length(gubo.auxLightPos1 - fragPos)), gubo.pointLightDecayFactors5to8.y);
		mat[2] = gubo.auxLightColor2 * pow((gubo.pointLightDecayFactors5to8.x/length(gubo.auxLightPos2 - fragPos)), gubo.pointLightDecayFactors5to8.y);
	}

	return mat;
}

// Function to create the vectors that contain the specular color for creating the reflection of each light
mat3 create_specular_colors(float num) {
	mat3 mat;

	if (num == 0) {
		mat[0] = vec3 (0.0, 0.0, 0.0);
		mat[1] = vec3 (0.0, 0.0, 0.0);
		mat[2] = vec3 (0.0, 0.0, 0.0);
		if ((gubo.lightColor0.x + gubo.lightColor0.y + gubo.lightColor0.z) > 0.0 && 
			(gubo.lightColor0.x + gubo.lightColor0.y + gubo.lightColor0.z) < 3.0) {
			mat[0] = gubo.lightColor0;
		} else if ((gubo.lightColor0.x + gubo.lightColor0.y + gubo.lightColor0.z) >= 3.0){
			mat[0] = vec3(1.0, 1.0, 1.0);
		}
		if ((gubo.lightColor1.x + gubo.lightColor1.y + gubo.lightColor1.z) > 0.0 && 
			(gubo.lightColor1.x + gubo.lightColor1.y + gubo.lightColor1.z) < 3.0) {
			mat[1] = gubo.lightColor1;
		} else if ((gubo.lightColor1.x + gubo.lightColor1.y + gubo.lightColor1.z) >= 3.0){
			mat[1] = vec3(1.0, 1.0, 1.0);
		}
		if ((gubo.lightColor2.x + gubo.lightColor2.y + gubo.lightColor2.z) > 0.0 && 
			(gubo.lightColor2.x + gubo.lightColor2.y + gubo.lightColor2.z) < 3.0) {
			mat[2] = gubo.lightColor2;
		} else if ((gubo.lightColor2.x + gubo.lightColor2.y + gubo.lightColor2.z) >= 3.0){
			mat[2] = vec3(1.0, 1.0, 1.0);
		}
	}
	else if (num == 1) {
		mat[0] = vec3 (0.0, 0.0, 0.0);
		mat[1] = vec3 (0.0, 0.0, 0.0);
		mat[2] = vec3 (0.0, 0.0, 0.0);
		if ((gubo.lightColor3.x + gubo.lightColor3.y + gubo.lightColor3.z) > 0.0 && 
			(gubo.lightColor3.x + gubo.lightColor3.y + gubo.lightColor3.z) < 3.0) {
			mat[0] = gubo.lightColor3;
		} else if ((gubo.lightColor3.x + gubo.lightColor3.y + gubo.lightColor3.z) >= 3.0){
			mat[0] = vec3(1.0, 1.0, 1.0);
		}
		if ((gubo.lightColor4.x + gubo.lightColor4.y + gubo.lightColor4.z) > 0.0 && 
			(gubo.lightColor4.x + gubo.lightColor4.y + gubo.lightColor4.z) < 3.0) {
			mat[1] = gubo.lightColor4;
		} else if ((gubo.lightColor4.x + gubo.lightColor4.y + gubo.lightColor4.z) >= 3.0){
			mat[1] = vec3(1.0, 1.0, 1.0);
		}
		if ((gubo.lightColor5.x + gubo.lightColor5.y + gubo.lightColor5.z) > 0.0 && 
			(gubo.lightColor5.x + gubo.lightColor5.y + gubo.lightColor5.z) < 3.0) {
			mat[2] = gubo.lightColor5;
		} else if ((gubo.lightColor5.x + gubo.lightColor5.y + gubo.lightColor5.z) >= 3.0){
			mat[2] = vec3(1.0, 1.0, 1.0);
		}
	} 
	else {
		mat[0] = vec3 (0.0, 0.0, 0.0);
		mat[1] = vec3 (0.0, 0.0, 0.0);
		mat[2] = vec3 (0.0, 0.0, 0.0);
		if ((gubo.auxLightColor0.x + gubo.auxLightColor0.y + gubo.auxLightColor0.z) > 0.0 && 
			(gubo.auxLightColor0.x + gubo.auxLightColor0.y + gubo.auxLightColor0.z) < 3.0) {
			mat[0] = gubo.auxLightColor0;
		} else if ((gubo.auxLightColor0.x + gubo.auxLightColor0.y + gubo.auxLightColor0.z) >= 3.0) {
			mat[0] = vec3(1.0, 1.0, 1.0);
		}
		if ((gubo.auxLightColor1.x + gubo.auxLightColor1.y + gubo.auxLightColor1.z) > 0.0 && 
			(gubo.auxLightColor2.x + gubo.auxLightColor2.y + gubo.auxLightColor2.z) < 3.0) {
			mat[1] = gubo.auxLightColor1;
		} else if ((gubo.auxLightColor1.x + gubo.auxLightColor1.y + gubo.auxLightColor1.z) >= 3.0){
			mat[0] = vec3(1.0, 1.0, 1.0);
		}
		if ((gubo.auxLightColor2.x + gubo.auxLightColor2.y + gubo.auxLightColor2.z) > 0.0 && 
			(gubo.auxLightColor2.x + gubo.auxLightColor2.y + gubo.auxLightColor2.z) < 3.0) {
			mat[2] = gubo.auxLightColor2;
		} else if ((gubo.auxLightColor2.x + gubo.auxLightColor2.y + gubo.auxLightColor2.z) >= 3.0){
			mat[2] = vec3(1.0, 1.0, 1.0);
		}
	}

	return mat;
}

void main() {

	vec3 normal = normalize(fragNormal); // Normalizes the normals that comes from the vertex shader which have been affected by a transformation
	vec3 viewDirection = normalize(gubo.eyePosition - fragPos); // Calculates a unitary vector that points from the fragment in xyz coordinates to the point from which we are looking it										
    vec3 diffuseColor = texture(texSampler, fragUVText).rgb;

	vec3 diffuseBRDF, specularBRDF, ambientColor;
	specularBRDF = vec3(0.0);

	/* 
	*	Here I put the computations for the rendering equation. I will put different models and then we can either select the one that suites
	*	best or implement some sort of function to decide which to choose while the app is running. 
	*	In the following I'll put the code for both computing the BRDF function including:
	*		- DIFFUSE part, for which I will discard Oren_Nayar a priori due to the nature of the material that compose the pinball;
	*		- SPECULAR part, for which I will discard the Ward anisotropic model;
	*		- AMBIENT part, for which I used the hemispheric lighting model over the y-axis.
	*/

	//--------- DIFFUSE FUNCTIONS ---------// 

	// calcuation of the color of the pointlight after the decay comes into play, I put it into 3 3x3 matrices for convenience
	mat3 lightDecay0to2 = create_lights_decay_colors(0);
	mat3 lightDecay3to5 = create_lights_decay_colors(1);
	mat3 auxLightDecay = create_lights_decay_colors(2);

	if (gubo.functionDecider.x == 0.0) {
		//	Lambert diffuse
		diffuseBRDF = create_Lambert_diffuse(normal, diffuseColor, lightDecay0to2, lightDecay3to5, auxLightDecay);
	} else if (gubo.functionDecider.x == 1.0) {
		//  Toon diffuse (thr to define!)
		diffuseBRDF = create_Toon_diffuse(normal, diffuseColor, 0.5f, lightDecay0to2, lightDecay3to5, auxLightDecay);
	}
	
	//--------- SPECULAR FUNCTIONS ---------//
	
	//Note: for specular color I'm not sure about it but for now I'll go with white (the color I thought for the lights).

	// Specular color is the color the reflections of the lights will have on shiny obects, thus it is actually a problem to hardcode it as
	// white because not all the lights have a white reflection, plus if we turn off the lights the hardcoded color remains!
	//vec3 specularColor = vec3(1.0, 1.0, 1.0);

	vec3 directSpecularColor = vec3 (0.0, 0.0, 0.0);
	if ((gubo.dlightColor.x + gubo.dlightColor.y + gubo.dlightColor.z) > 0.0 &&
		(gubo.dlightColor.x + gubo.dlightColor.y + gubo.dlightColor.z) < 3.0) {
		directSpecularColor = gubo.dlightColor;
	}
	else if ((gubo.dlightColor.x + gubo.dlightColor.y + gubo.dlightColor.z) >= 3.0){
		directSpecularColor = vec3(1.0, 1.0, 1.0);
	}

	mat3 specularColors0to2 = create_specular_colors(0);
	mat3 specularColors3to5 = create_specular_colors(1);
	mat3 auxSpecularColors = create_specular_colors(2);
	
	if (gubo.functionDecider.y == 0.0) {
		//	Blinn specular
		float blinnExponent = 100.0f; // exponent decided
		specularBRDF = create_Blinn_specular(viewDirection, normal, directSpecularColor, specularColors0to2, specularColors3to5, auxSpecularColors, blinnExponent);
	} else if (gubo.functionDecider.y == 1.0) {
		//	Toon specular 
		specularBRDF = create_Toon_specular(normal, viewDirection, directSpecularColor, specularColors0to2, specularColors3to5, auxSpecularColors, 0.0f);
	} else if (gubo.functionDecider.y == 2.0) {
		//	Phong specular
		float phongExponent = 50.0f;
		specularBRDF = create_Phong_specular(normal, viewDirection, specularColors0to2, specularColors3to5, directSpecularColor, auxSpecularColors, phongExponent);
	}

	//--------- AMBIENT COLOR FUNCTION ---------// 
	//	For ambient lighting we used Hemispheric lighting over the y-axis
	float ambientRefCoefficient = 0.025; 
	ambientColor = gubo.basicAmbient * ((normal * gubo.definingDirection + 1)/2 * gubo.skyColor + 
		(1 - normal * gubo.definingDirection)/2 * gubo.floorColor) * ambientRefCoefficient;

	float xColor = clamp((diffuseBRDF.x + specularBRDF.x + ambientColor.x), 0.0f, 1.0f);
	float yColor = clamp((diffuseBRDF.y + specularBRDF.y + ambientColor.y), 0.0f, 1.0f);
	float zColor = clamp((diffuseBRDF.z + specularBRDF.z + ambientColor.z), 0.0f, 1.0f);
	outColor = vec4(xColor, yColor, zColor, 1.0f);

}

