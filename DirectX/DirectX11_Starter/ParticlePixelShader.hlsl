
Texture2D myTexture : register(t0);
SamplerState mySampler: register(s0);

// Defines the input to this pixel shader
// - Should match the output of our corresponding vertex shader
struct PSInput
{
	float4 position        : SV_POSITION;
	float4 color        : COLOR;
	float2 uv            : TEXCOORD0;
};

// Entry point for this pixel shader
float4 main(PSInput input) : SV_TARGET
{
	// Return color sampled from texture
	return myTexture.Sample(mySampler, input.uv);
}
