#define MAX_LIGHTS 10

struct PS_INPUT
{
	float4 color : COLOR;
	float2 uv : TEXTCOORD;
	float3 worldPos : WORLDPOSITION; // position in the world
	float3 wn : NORMAL; // wrold normal 
	float3 cameraPos : CAMERAPOS; // position relitave to the camera
	float3 cn : CAMERANORMAL; // normal relitave to the camera
};

float4 main(PS_INPUT input) : SV_TARGET
{
	float4 diffuse = input.color;
	float ao = 1;

	float3 lo = float3(0, 0, 0);
	
	float3 ambiant = float3(0.5, 0.5, 0.5) * diffuse.rgb;

	float4 color = float4((ambiant + lo) * ao, diffuse.a);

	return color;
}