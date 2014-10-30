
Texture2D myTexture : register(t0);
SamplerState mySampler: register(s0);

// Defines the input to this pixel shader
// - Should match the output of our corresponding vertex shader
struct VertexToPixel
{
	float4 position		: SV_POSITION;
	float3 normal		: NORMAL;
	float2 uv			: TEXCOORD0;
	float3 light        : float3;
};

// Entry point for this pixel shader
float4 main(VertexToPixel input) : SV_TARGET
{
	// Return color sampled from texture
	float nDotL = dot(normalize(input.normal), -normalize(input.light));
	return myTexture.Sample(mySampler, input.uv) * nDotL;
}