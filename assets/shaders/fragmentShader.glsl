#version 330 core
in vec3 vertexColor;
in vec2 vertexUV;
in float alpha;
in vec3 fragmentNormal;
in vec3 fragmentPosition;
in vec4 fragPosLightSpace;
in vec4 gl_FragCoord;

uniform sampler2D textureSampler;
uniform vec3 lightColor;
uniform vec3 lightPosition;
uniform vec3 lightDirection;
uniform float lightCutoffOuter;
uniform float lightCutoffInner;
uniform float lightNearPlane;
uniform float lightFarPlane;
uniform vec3 viewPosition;
uniform sampler2D shadowMap;
uniform vec3 objectColor;
uniform int isSpotLightOn = 0;
uniform int isShadowOn = 0;

out vec4 FragColor;

const float shadingAmbientK    = 0.4;
const float shadingDiffuseK    = 0.6;
const float shadingSpecularK   = 0.3;
const float PI = 3.1415926535897932384626433832795;

vec3 ambientColor(vec3 lightColorArg) {
    return shadingAmbientK * lightColorArg;
}

vec3 diffuseColor(vec3 lightColorArg, vec3 lightPositionArg) {
    vec3 lightDirection = normalize(lightPositionArg - fragmentPosition);
    return shadingDiffuseK * lightColorArg * max(dot(normalize(fragmentNormal), lightDirection), 0.0f);
}

vec3 specularColor(vec3 lightColorArg, vec3 lightPositionArg) {
    vec3 lightDirection = normalize(lightPositionArg - fragmentPosition);
    vec3 viewDirection = normalize(viewPosition - fragmentPosition);
    vec3 reflectLightDirection = reflect(-lightDirection, normalize(fragmentNormal));
    return shadingSpecularK * lightColorArg * pow(max(dot(reflectLightDirection, viewDirection), 0.0f), 32);
}

float shadowScalar() {
    // this function returns 1.0 when the surface receives light, and 0.0 when it is in a shadow
    // perform perspective divide
    vec3 normalizedDeviceCoordinates = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    normalizedDeviceCoordinates = normalizedDeviceCoordinates * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLightSpace as coords)
    float closestDepth = texture(shadowMap, normalizedDeviceCoordinates.xy).r;
    // get depth of current fragment from light's perspective
    float currentDepth = normalizedDeviceCoordinates.z;
    // check whether current frag pos is in shadow
    float bias = 0.05;// bias applied in depth map: see shadow_vertex.glsl
    return ((currentDepth - bias) < closestDepth) ? 1.0 : 0.0;
}

float spotLightScalar() {
    float theta = dot(normalize(fragmentPosition - lightPosition), lightDirection);
    if (theta > lightCutoffInner) {
        return 1.0;
    } else if (theta > lightCutoffOuter) {
        return (1.0 - cos(PI * (theta - lightCutoffOuter) / (lightCutoffInner - lightCutoffOuter))) / 2.0;
    } else {
        return 0.0;
    }
}

void main()
{
    vec4 textureColor = texture(textureSampler, vertexUV);
    int checkForShadow = isShadowOn;
    float scalar = checkForShadow == 0 ? spotLightScalar() : shadowScalar() * spotLightScalar();
    vec3 ambient = ambientColor(lightColor);
    vec3 diffuse = scalar * diffuseColor(lightColor, lightPosition);
    vec3 specular = scalar * specularColor(lightColor, lightPosition);
    int checkForSpotLight = isSpotLightOn;
    vec3 totalLight = checkForSpotLight == 0 ? 2 * ambient : specular + diffuse + ambient;
    FragColor = vec4(textureColor.rgb * totalLight * objectColor, textureColor.a * alpha);
}