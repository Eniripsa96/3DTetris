
// The constant buffer that holds our "per model" data
// - Each object you draw with this shader will probably have
//   slightly different data (at least for the world matrix)
cbuffer perModel : register(b0)
{
	matrix world;
	matrix view;
	matrix projection;
	matrix lightView;
	matrix lightProjection;
	float4 lightDirection;
	float4 color;
};

// Defines what kind of data to expect as input
// - This should match our input layout!
struct VertexShaderInput
{
	float3 position		: POSITION;
};

// Defines the output data of our vertex shader
// - At a minimum, you'll need an SV_POSITION
struct Output
{
	float4 position		: SV_POSITION;
	float4 lightPos     : TEXCOORD0;
};

// The entry point for our vertex shader
Output main(VertexShaderInput input)
{
	Output output;

	// Caclulate lighting position
	matrix lightWorldViewProj = mul(mul(world, lightView), lightProjection);
	output.position = mul(float4(input.position, 1.0f), lightWorldViewProj);
	output.lightPos = output.position;

	return output;
}