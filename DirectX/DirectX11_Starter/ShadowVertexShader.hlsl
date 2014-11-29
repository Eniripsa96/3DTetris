
// The constant buffer that holds our "per model" data
// - Each object you draw with this shader will probably have
//   slightly different data (at least for the world matrix)
cbuffer perModel : register(b0)
{
	matrix world;
	matrix view;
	matrix projection;
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
};

// The entry point for our vertex shader
Output main(VertexShaderInput input)
{
	Output output;
	matrix worldViewProj = mul(mul(world, view), projection);
	output.position = mul(float4(input.position, 1.0f), worldViewProj);
	return output;
}