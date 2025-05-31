// Common resources
Texture2D tex : register(t0);
SamplerState samp : register(s0);

// Vertex shader input
struct VSInput
{
    float2 position : POSITION;
    float2 uv       : TEXCOORD0; // Explicit register index
};

// Vertex shader output / Pixel shader input
struct VSOutput
{
    float4 position : SV_POSITION;
    float2 uv       : TEXCOORD0; // Must match the pixel shader input
};

// Vertex Shader
VSOutput VSMain(VSInput input)
{
    VSOutput output;
    output.position = float4(input.position, 0.0f, 1.0f);
    output.uv = input.uv;
    return output;
}

// Pixel Shader
float4 PSMain(VSOutput input) : SV_TARGET
{
    return tex.Sample(samp, input.uv);
}
