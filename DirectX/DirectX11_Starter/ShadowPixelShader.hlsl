// Defines the output data of our vertex shader
// - At a minimum, you'll need an SV_POSITION
struct Input
{
	float4 position		: SV_POSITION;
	float4 lightPos     : TEXCOORD0;
};

// Entry point for this pixel shader
float4 main(Input input) : SV_TARGET
{
	input.lightPos.xyz /= input.lightPos.w;

	// Return color sampled from texture
	return float4(input.lightPos.z, input.lightPos.z, input.lightPos.z, 1.0f);
}