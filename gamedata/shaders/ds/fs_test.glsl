// ws - world space
// ms - model space
// cs - camera space

interface OutputVs {
    vec2 UV;
    vec3 position_ws;
    vec3 normal_cs;
    vec3 tangent_ws;
    vec3 bitangent_ws;
};
struct OutputVs1 {
    vec2 UV;
    vec3 position_ws;
    vec3 normal_cs;
};

//layout (location = 5)  in mat4 _MVP;
//layout (location = 9)  in mat4 _M;

uniform mat4 _MVP;
uniform mat4 _M;

shader main_vs(
        in vec3 position_ms,
        in vec2 vertexUV,
        in vec3 vertexNormal_ms,
        in vec3 tangent_ms,
        in vec3 bitangent_ms,
        out OutputVs o)
{
    gl_Position    = _MVP * vec4(position_ms, 1.f);
    o.UV           = vertexUV;
    o.position_ws  = (_M * vec4(position_ms, 1.f)).xyz;
    o.normal_cs    = (_M * vec4(vertexNormal_ms, 0.0)).xyz;
    o.tangent_ws   = (_M * vec4(tangent_ms, 0.f)).xyz;
    o.bitangent_ws = (_M * vec4(bitangent_ms, 0.f)).xyz;
};

struct BaseLight {
    vec3 color;
    float ambient_intensity;
    float diffuse_intensity;
};

struct DirectionalLight {
    BaseLight base;
    vec3      direction;
};

struct Attenuation {
    float constant;
    float linear;
    float quadratic;
};

struct PointLight {
    BaseLight   base;
    vec3        position;
    Attenuation attenuation;
};

struct SpotLight {
    PointLight base;
    vec3       direction;
    float      cutoff;
};

const int MAX_POINT_LIGHTS = 16;
const int MAX_SPOT_LIGHTS  = 16;

uniform vec3  _eye_pos_ws;
uniform float _specular_power;
uniform float _mat_specular_intensity;
uniform int   _num_point_lights;
uniform int   _num_spot_lights;

uniform DirectionalLight _directional_light;
uniform PointLight       _point_lights[MAX_POINT_LIGHTS];
uniform SpotLight        _spot_lights [MAX_SPOT_LIGHTS];

vec4 calcLightInternal(BaseLight light, vec3 light_direction, OutputVs1 i2)
{
    vec4 AmbientColor = vec4(light.color, 1.0f) * light.ambient_intensity;
    float DiffuseFactor = dot(i2.normal_cs, -light_direction);

    vec4 DiffuseColor  = vec4(0, 0, 0, 0);
    vec4 SpecularColor = vec4(0, 0, 0, 0);

    if (DiffuseFactor > 0) {
        DiffuseColor = vec4(light.color, 1.0f) * light.diffuse_intensity * DiffuseFactor;

        vec3 VertexToEye = normalize(_eye_pos_ws - i2.position_ws);
        vec3 LightReflect = normalize(reflect(light_direction, i2.normal_cs));
        float SpecularFactor = dot(VertexToEye, LightReflect);
        SpecularFactor = pow(SpecularFactor, _specular_power);
        if (SpecularFactor > 0) {
            SpecularColor = vec4(light.color, 1.0f) * _mat_specular_intensity * SpecularFactor;
        }
    }

    return (AmbientColor + DiffuseColor + SpecularColor);
}

vec4 calcDirectionalLight(OutputVs1 In)
{
    return calcLightInternal(_directional_light.base, _directional_light.direction, In);
}

vec4 calcPointLight(PointLight l, OutputVs1 i2)
{
    vec3 LightDirection = i2.position_ws - l.position;
    float Distance = length(LightDirection);
    LightDirection = normalize(LightDirection);

    vec4 Color = calcLightInternal(l.base, LightDirection, i2);
    float attenuation =  l.attenuation.constant + l.attenuation.linear * Distance +
                                                  l.attenuation.quadratic * Distance * Distance;

    return Color / attenuation;
}

vec4 calcSpotLight(SpotLight l, OutputVs1 i)
{
    vec3 LightToPixel = normalize(i.position_ws - l.base.position);
    float SpotFactor  = dot(LightToPixel, l.direction);

    if (SpotFactor > l.cutoff) {
        vec4 Color = calcPointLight(l.base, i);
        return Color * (1.0 - (1.0 - SpotFactor) * 1.0/(1.0 - l.cutoff));
    }
    else {
        return vec4(0,0,0,0);
    }
}

uniform sampler2D _textureSampler;
uniform sampler2D _normal_map;

vec3 calcBumpedNormal(vec2 uv, vec3 normal_cs, vec3 tangent_ws, vec3 bitangent_ws)
{
    vec3 normal    = normalize(normal_cs);
    vec3 tangent   = normalize(tangent_ws);
    vec3 bitangent = normalize(bitangent_ws);

    vec3 bump_map_normal = 2.0 * texture(_normal_map, uv).xyz - vec3(1.0, 1.0, 1.0);

    mat3 TBN        = mat3(tangent, bitangent, normal);
    vec3 new_normal = TBN * bump_map_normal;

    return normalize(new_normal);
}

shader main_fs(in OutputVs i, out vec4 color)
{
    OutputVs1 inp;
    inp.UV          = i.UV;
    inp.normal_cs   = calcBumpedNormal(i.UV, i.normal_cs, i.tangent_ws, i.bitangent_ws);
    inp.position_ws = i.position_ws;

    vec4 total_light = calcDirectionalLight(inp);

    for (int i = 0; i < _num_point_lights; ++i)
        total_light += calcPointLight(_point_lights[i], inp);

    for (int i = 0; i < _num_spot_lights; ++i)
        total_light += calcSpotLight(_spot_lights[i], inp);

    color = texture(_textureSampler, i.UV) * total_light;
};

program fs_test {
    vs(410) = main_vs();
    fs(410) = main_fs();
};