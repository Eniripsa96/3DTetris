//float4 main( float4 pos : POSITION ) : SV_POSITION
//{
//	return pos;
//}

struct ParticleInput
{
	float3 initialPos	: POSITION;
	float3 initialVel	: VELOCITY;
	float2 size			: SIZE;
	float age			: AGE;
	unsigned int type	: TYPE;
};

struct ParticleOutput
{
	float4 initialPos	: SV_POSITION;
	float3 initialVel	: VELOCITY;
	float2 size			: SIZE;
	float age : AGE;
	unsigned int type : TYPE;
};

ParticleOutput main(ParticleInput input)
{
	ParticleOutput output;
	output.initialPos = float4(input.initialPos, 1.0f);
	output.initialVel = input.initialPos;
	output.size = input.size;
	output.age = input.age;
	output.type = input.type;

	return output;
}