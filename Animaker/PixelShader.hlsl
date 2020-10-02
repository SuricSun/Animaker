//

struct PS_INPUT
{
    float4 position : SV_POSITION;  // interpolated vertex position (system value)
    float4 color    : COLOR;       // interpolated color
};

float4 main(PS_INPUT inPix) : SV_TARGET
{
    return inPix.color;
}