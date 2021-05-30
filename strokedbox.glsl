precision mediump float;

uniform float time;
uniform vec2 mouse; 
uniform vec2 resolution;

void main( void ) {

    vec2 position = ( gl_FragCoord.xy / resolution.xy ) + mouse / 4.0;

                   // x1  y1   x2   y2
    vec4 rect = vec4(0.2, 0.1, 0.4, 0.5);
    vec4 bigRect = vec4(0.15, 0.05, 0.45, 0.55);
    vec2 hv = step(rect.xy, position) * step(position, rect.zw);
    vec2 hv1 = step(bigRect.xy, position) * step(position, bigRect.zw);
    float onOff = hv.x * hv.y;
    float onOff1 = hv1.x * hv1.y;

    gl_FragColor = mix(vec4(0,0,0,0), vec4(0,1,0,0), onOff1 - onOff) + mix(vec4(0,0,0,0), vec4(1,0,0,0), onOff);
}