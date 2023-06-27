#version 330 core

struct PositionalLight {
	vec3 Position;
	vec3 Ka;
	vec3 Kd;
	vec3 Ks;
	float Kc;
	float Kl;
	float Kq;
};

struct DirectionalLight {
	vec3 Position;
	vec3 Direction;
	vec3 Ka;
	vec3 Kd;
	vec3 Ks;
	float InnerCutOff;
	float OuterCutOff;
	float Kc;
	float Kl;
	float Kq;
};

struct Material {
	sampler2D Kd;
	sampler2D Ks;
	float Shininess;
};

out vec4 FragColor;

in vec2 UV;
in vec3 vWorldSpaceFragment;
in vec3 vWorldSpaceNormal;

uniform vec3 uCol;
uniform sampler2D ourTexture;
uniform vec3 lightColor;

uniform PositionalLight uPointLight1;
uniform PositionalLight uPointLight2;
uniform PositionalLight uPointLight3;
uniform PositionalLight uPointLight4;
uniform DirectionalLight uSpotlight;
uniform DirectionalLight uDirLight;
uniform Material uMaterial;
uniform vec3 uViewPos;


void main() {
	vec3 ViewDirection = normalize(uViewPos - vWorldSpaceFragment);
	vec3 DirLightVector = normalize(-uDirLight.Direction);
	float DirDiffuse = max(dot(vWorldSpaceNormal, DirLightVector), 0.0f);
	vec3 DirReflectDirection = reflect(-DirLightVector, vWorldSpaceNormal);
	float DirSpecular = pow(max(dot(ViewDirection, DirReflectDirection), 0.0f), uMaterial.Shininess);

	vec3 DirAmbientColor = uDirLight.Ka * vec3(texture(uMaterial.Kd, UV));
	vec3 DirDiffuseColor = uDirLight.Kd * DirDiffuse * vec3(texture(uMaterial.Kd, UV));
	vec3 DirSpecularColor = uDirLight.Ks * DirSpecular * vec3(texture(uMaterial.Ks, UV));
	vec3 DirColor = DirAmbientColor + DirDiffuseColor + DirSpecularColor;

	//point 1
	vec3 PtLightVector = normalize(uPointLight1.Position - vWorldSpaceFragment);
	float PtDiffuse = max(dot(vWorldSpaceNormal, PtLightVector), 0.0f);
	vec3 PtReflectDirection = reflect(-PtLightVector, vWorldSpaceNormal);
	float PtSpecular = pow(max(dot(ViewDirection, PtReflectDirection), 0.0f), uMaterial.Shininess);

	vec3 PtAmbientColor = uPointLight1.Ka * vec3(texture(uMaterial.Kd, UV));
	vec3 PtDiffuseColor = PtDiffuse * uPointLight1.Kd * vec3(texture(uMaterial.Kd, UV));
	vec3 PtSpecularColor = PtSpecular * uPointLight1.Ks * vec3(texture(uMaterial.Ks, UV));

	float PtLightDistance = pow(length(uPointLight1.Position - vWorldSpaceFragment)/10, 3);
	float PtAttenuation = 1.0f / (uPointLight1.Kc + uPointLight1.Kl * PtLightDistance + uPointLight1.Kq * (PtLightDistance * PtLightDistance));
	vec3 PtColor1 = PtAttenuation * (PtAmbientColor + PtDiffuseColor + PtSpecularColor);

	//point 2
	PtLightVector = normalize(uPointLight2.Position - vWorldSpaceFragment);
	PtDiffuse = max(dot(vWorldSpaceNormal, PtLightVector), 0.0f);
	PtReflectDirection = reflect(-PtLightVector, vWorldSpaceNormal);
	PtSpecular = pow(max(dot(ViewDirection, PtReflectDirection), 0.0f), uMaterial.Shininess);

	PtAmbientColor = uPointLight2.Ka * vec3(texture(uMaterial.Kd, UV));
	PtDiffuseColor = PtDiffuse * uPointLight2.Kd * vec3(texture(uMaterial.Kd, UV));
	PtSpecularColor = PtSpecular * uPointLight2.Ks * vec3(texture(uMaterial.Ks, UV));

	PtLightDistance = pow(length(uPointLight2.Position - vWorldSpaceFragment)/10, 3);
	PtAttenuation = 1.0f / (uPointLight2.Kc + uPointLight2.Kl * PtLightDistance + uPointLight2.Kq * (PtLightDistance * PtLightDistance));
	vec3 PtColor2 = PtAttenuation * (PtAmbientColor + PtDiffuseColor + PtSpecularColor);

	//point 3
	PtLightVector = normalize(uPointLight3.Position - vWorldSpaceFragment);
	PtDiffuse = max(dot(vWorldSpaceNormal, PtLightVector), 0.0f);
	PtReflectDirection = reflect(-PtLightVector, vWorldSpaceNormal);
	PtSpecular = pow(max(dot(ViewDirection, PtReflectDirection), 0.0f), uMaterial.Shininess);

	PtAmbientColor = uPointLight3.Ka * vec3(texture(uMaterial.Kd, UV));
	PtDiffuseColor = PtDiffuse * uPointLight3.Kd * vec3(texture(uMaterial.Kd, UV));
	PtSpecularColor = PtSpecular * uPointLight3.Ks * vec3(texture(uMaterial.Ks, UV));

	PtLightDistance = pow(length(uPointLight3.Position - vWorldSpaceFragment)/10, 3);
	PtAttenuation = 1.0f / (uPointLight3.Kc + uPointLight3.Kl * PtLightDistance + uPointLight3.Kq * (PtLightDistance * PtLightDistance));
	vec3 PtColor3 = PtAttenuation * (PtAmbientColor + PtDiffuseColor + PtSpecularColor);

	//point 4
	PtLightVector = normalize(uPointLight4.Position - vWorldSpaceFragment);
	PtDiffuse = max(dot(vWorldSpaceNormal, PtLightVector), 0.0f);
	PtReflectDirection = reflect(-PtLightVector, vWorldSpaceNormal);
	PtSpecular = pow(max(dot(ViewDirection, PtReflectDirection), 0.0f), uMaterial.Shininess);

	PtAmbientColor = uPointLight4.Ka * vec3(texture(uMaterial.Kd, UV));
	PtDiffuseColor = PtDiffuse * uPointLight4.Kd * vec3(texture(uMaterial.Kd, UV));
	PtSpecularColor = PtSpecular * uPointLight4.Ks * vec3(texture(uMaterial.Ks, UV));

	PtLightDistance = pow(length(uPointLight4.Position - vWorldSpaceFragment)/10, 3);
	PtAttenuation = 1.0f / (uPointLight4.Kc + uPointLight4.Kl * PtLightDistance + uPointLight4.Kq * (PtLightDistance * PtLightDistance));
	vec3 PtColor4 = PtAttenuation * (PtAmbientColor + PtDiffuseColor + PtSpecularColor);

	vec3 SpotlightVector = normalize(uSpotlight.Position - vWorldSpaceFragment);

	float SpotDiffuse = max(dot(vWorldSpaceNormal, SpotlightVector), 0.0f);
	vec3 SpotReflectDirection = reflect(-SpotlightVector, vWorldSpaceNormal);
	float SpotSpecular = pow(max(dot(ViewDirection, SpotReflectDirection), 0.0f), uMaterial.Shininess);

	vec3 SpotAmbientColor = uSpotlight.Ka * vec3(texture(uMaterial.Kd, UV));
	vec3 SpotDiffuseColor = SpotDiffuse * uSpotlight.Kd * vec3(texture(uMaterial.Kd, UV));
	vec3 SpotSpecularColor = SpotSpecular * uSpotlight.Ks * vec3(texture(uMaterial.Ks, UV));

	float SpotlightDistance = length(uSpotlight.Position - vWorldSpaceFragment);
	float SpotAttenuation = 1.0f / (uSpotlight.Kc + uSpotlight.Kl * SpotlightDistance + uSpotlight.Kq * (SpotlightDistance * SpotlightDistance));

	float Theta = dot(SpotlightVector, normalize(-uSpotlight.Direction));
	float Epsilon = uSpotlight.InnerCutOff - uSpotlight.OuterCutOff;
	float SpotIntensity = clamp((Theta - uSpotlight.OuterCutOff) / Epsilon, 0.0f, 1.0f);
	vec3 SpotColor = SpotIntensity * SpotAttenuation * (SpotAmbientColor + SpotDiffuseColor + SpotSpecularColor);

	vec3 FinalColor = DirColor + PtColor1 + PtColor2 + PtColor3 + PtColor4 + SpotColor;

	FragColor = vec4((FinalColor  * uCol), 1.0f);
}