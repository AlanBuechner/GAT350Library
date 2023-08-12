#section config

#section common
struct VS_Input
{
	float4 position : POSITION;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float2 uv : UV;
};

struct VS_Output
{
	float4 position : SV_POSITION;
	float4 worldPosition : WORLD_POSITION;
	float depth : DEPTH; // the depth into the scene
	float2 uv : TEXTCOORD;
	float3 normal : NORMAL; // world space normal
	float3 tangent : TANGENT; // world space tangent
	float3 bitangent : BITANGENT; // world space bigangent
	float3 CameraToPoint : CTOP; // direction from the camera to the point
};

typedef VS_Output PS_Input;

#section vertex

cbuffer Camera
{
	float4x4 View;
	float4x4 ViewProjection;
	float3 CameraPosition;
};

cbuffer Model
{
	float4x4 Transform;
};

VS_Output main(VS_Input input)
{
	VS_Output output;

	float4x4 mvp = mul(ViewProjection, Transform);
	float4x4 mv = mul(View, Transform);

	output.position = mul(mvp, input.position);
	output.worldPosition = mul(Transform, input.position);
	output.uv = input.uv;

	// get the depth in world space by tarnsforming the vertex relitive to the camera without the projection and geting the z component
	float4 cameraSpacePosition = mul(mv, input.position);
	output.depth = -cameraSpacePosition.z; // the z needs to be fliped because directx 11 is left handed while glm is right handed

	output.normal = normalize(mul((float3x3)Transform, input.normal));
	output.tangent = normalize(mul((float3x3)Transform, input.tangent));
	output.bitangent = normalize(cross(output.normal, output.tangent));

	output.CameraToPoint = CameraPosition - output.worldPosition.xyz;

	return output;
}

#section pixel

struct PS_Output
{
	float4 color : SV_TARGET0;
};

Texture2D<float4> texDef : register(t0);

StaticSampler textureSampler = StaticSampler(repeat, repeat, anisotropic, anisotropic);

PS_Output main(PS_Input input)
{
	PS_Output output;

	output.color = texDef.Sample(textureSampler, input.uv);
	if (output.color.a < 0.5)
		discard;

	return output;
}