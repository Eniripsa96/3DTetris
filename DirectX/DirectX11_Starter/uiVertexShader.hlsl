// Defines what kind of data to expect as input
// - This should match our input layout!
struct VertexShaderInput
{
	float3 position		: POSITION;
	float2 uv			: TEXCOORD0;
};

// Defines the output data of our vertex shader
// - At a minimum, you'll need an SV_POSITION
// - Should match the pixel shader's input
struct VertexToPixel
{
	float4 position		: SV_POSITION;
	float2 uv			: TEXCOORD0;
};

// The entry point for our vertex shader
float4 main( VertexShaderInput input )
{
	// Set up output
	VertexToPixel output;

	// Pass the position through
	output.position = float4(input.position, 1.0f);

	// Pass the UV coordinates
	output.uv = input.uv;

	return output;
}