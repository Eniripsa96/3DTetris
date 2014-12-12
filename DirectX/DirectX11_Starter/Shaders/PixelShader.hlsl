
Texture2D myTexture : register(t0);
Texture2D shadowMap : register(t1);
SamplerState mySampler : register(s0);
SamplerState shadowSampler : register(s1);

// Defines the input to this pixel shader
// - Should match the output of our corresponding vertex shader
struct VertexToPixel
{
	float4 position		: SV_POSITION;
	float3 normal		: NORMAL;
	float2 uv			: TEXCOORD0;
	float4 lightPos     : TEXCOORD1;
	float4 lightDir     : LIGHT;
	float4 color        : COLOR;
};

// Entry point for this pixel shader
float4 main(VertexToPixel input) : SV_TARGET
{
	input.lightPos.xyz /= input.lightPos.w;
	float shade = 1.0f;

	input.lightPos.y = -input.lightPos.y;
	input.lightPos.xy = input.lightPos.xy / 2 + 0.5f;

	float depth = shadowMap.Sample(shadowSampler, input.lightPos.xy).r;
	if (depth < input.lightPos.z - 0.0001f) {
		shade = 0.0f;
	}

	// Return color sampled from texture
	float nDotL = dot(normalize(input.normal), -normalize((float3)input.lightDir));
	return float4(myTexture.Sample(mySampler, input.uv).xyz * (1 - input.lightDir.w + shade * input.lightDir.w * nDotL) * input.color.xyz, input.color.w);
}