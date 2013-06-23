varying vec3 textureCoords;
varying vec3 normal;
uniform vec3 lightDir;
uniform sampler3D data;
uniform sampler1D transerfunction;
uniform sampler3D TF2D_data;
uniform sampler2D TF_color;
uniform vec3 viewVec;
uniform vec3 NofViewPlan;
uniform vec3 scale;

//reference http://www.siggraph.org/education/materials/HyperGraph/raytrace/rtinter3.htm
bool intersectBox( vec3 eye_pos , vec3 eye_dir , vec3 boxmin , vec3 boxmax , out float tnear , out float tfar ){
	vec3 invR = 1.0 / eye_dir;
    vec3 tbot = invR * (boxmin - eye_pos);
    vec3 ttop = invR * (boxmax - eye_pos);

    // re-order intersections to find smallest and largest on each axis
    float3 tmin = min(ttop, tbot);
    float3 tmax = max(ttop, tbot);

    // find the largest tmin and the smallest tmax
    float largest_tmin = max(max(tmin.x, tmin.y), max(tmin.x, tmin.z));
    float smallest_tmax = min(min(tmax.x, tmax.y), min(tmax.x, tmax.z));
	tnear = largest_tmin;
	tfar = smallest_tmax;
    return smallest_tmax > largest_tmin;
}

vec3 getNormal( vec3 texPosition ){
	vec3 pos = (0.5*texPosition+0.5*scale)/scale; //0.5*texPosition + 0.5*vec3(1);
	vec3 up = vec3( 0 , 0.005 , 0);
	vec3 down = vec3( 0 , -0.005 , 0);
	vec3 left = vec3( -0.005 , 0 , 0);
	vec3 right = vec3( 0.005 , 0 , 0);
	vec3 front = vec3( 0 , 0 , -0.005);
	vec3 end = vec3( 0 , 0 , 0.005 );
    vec3 scalar = vec3( texture3D( data , pos + left ).s - texture3D( data , pos + right ).s ,
	 texture3D( data , pos + down ).s - texture3D( data , pos + up ).s ,
	  texture3D( data , pos + front ).s - texture3D( data , pos + end ).s );
	  if( length( scalar ) > 0 )
		scalar = normalize( scalar);
	return scalar;
}
vec4 getColor( vec3 texPosition ){
	vec3 pos = (0.5*texPosition+0.5*scale)/scale; //0.5*texPosition + 0.5*vec3(1);
	vec4 scalar = texture3D( data , pos.xyz );
	vec4 tempColor = texture1D( transerfunction , scalar.s );
	tempColor.rgb = tempColor.rgb * tempColor.a;
	return tempColor;
}
void main(){
	float sampleSpacing = 0.005;
	vec4 accumulatedColor = vec4(0.0);
	vec3 samplePos = vec3(0.0);
	vec3 rayStart = textureCoords;
	vec3 tex = textureCoords;
	/**
	*  drawing on the back-plane
	*/
	vec3 viewV = normalize(textureCoords - viewVec); // pos to eye
	
	/**
	*  drawing on the intersection plan
	*/
	float tnear,tfar;
	//view /= scaleDim;
	vec3 boxMin = vec3(  -scale.x ,  -scale.y ,  -scale.z );
	vec3 boxMax = vec3( scale.x , scale.y , scale.z );
	if( intersectBox( viewVec , viewV , boxMin , boxMax , tnear , tfar ) ){
	if( tnear < 0.0 ) tnear = 0.0;
	float sampleLen = sampleSpacing ;
	vec4 sampleColor = vec4(0.0);
	vec4 diffuse = vec4( 1.0 , 1.0 , 1.0 , 0.0);
	vec4 specular = vec4( 1.0 , 1.0 , 1.0  , 0.0 );
	vec3 normalv;
	lightDir = vec3(0,-1,0);
	vec3 ldir = normalize(lightDir);
	vec3 h = normalize(-viewV );
	rayStart = viewVec + tnear*viewV;
	while(true){
		samplePos = rayStart + sampleLen * viewV;
		sampleLen += sampleSpacing;
		// caculate normal
		normalv = getNormal( samplePos );
		float f = dot( h , normalv );
		float sp = dot( h , normalv );
		sampleColor = getColor(samplePos);
		if( f > 0.0 ) sampleColor.rgb *= f*diffuse.rgb;
		if( sp > 0.0 ) sampleColor += pow( sp , 250.0 )*specular;
		
		if( sampleColor.a > 0.001 ){
			accumulatedColor.rgb = (1.0 - accumulatedColor.a)*sampleColor.rgb + accumulatedColor.rgb;
			accumulatedColor.a = (1.0 - accumulatedColor.a)*sampleColor.a + accumulatedColor.a;
		}
		if( samplePos.x > scale.x || samplePos.y > scale.y || samplePos.x < -scale.x || samplePos.y < -scale.y
			|| samplePos.z > scale.z || samplePos.z < -scale.z || accumulatedColor.a > 1.0){
				break;
			
		}
	}
	}
	gl_FragColor = accumulatedColor;
}