sampler2D TS0 : register(s0);
float4 tint_color : register(c0);
float4 background_color : register(c1);
float4 flash_color : register(c2);
float4 mask_color : register(c3);
float flipped_channels : register(c4);

float4 main(float2 texCoord : TEXCOORD0) : COLOR0 {
    float4 tex_point = tex2D(TS0, texCoord);
    float alpha_factor = 0.25;
    float meter_state = tint_color.a;
    float drained_fraction = flash_color.a;

    if(flipped_channels > 0.5) {
        float4 inv_tex_point = tex_point;
        tex_point = float4(inv_tex_point.aaa, inv_tex_point.r);
    }

    clip(tex_point.r - alpha_factor);

    float4 color;
    if(tex_point.a <= meter_state) {
        if(tex_point.a <= drained_fraction) {
            if(drained_fraction != meter_state) {
                color.rgb = lerp(flash_color.rgb, tint_color.rgb, saturate((drained_fraction - tex_point.a) / 0.1));
            }
            else {
                color.rgb = tint_color.rgb;
            }
        }   
        else {
            color.rgb = flash_color.rgb;
        }
        color.a = mask_color.a;
    }
    else {
        color.rgb = background_color.rgb;
        color.a = mask_color.a * background_color.a / 2;
    }

    float4 res;
    res.rgb = tex_point.rgb * color.rgb * mask_color.rgb;
    res.a = color.a;

    return res;
};
