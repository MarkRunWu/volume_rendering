varying vec3 textureCoords;
varying vec3 normal;
uniform vec3 lightDir;
uniform sampler3D data;
uniform sampler1D transerfunction;
uniform vec3 viewVec;
uniform vec3 NofViewPlan;

vec3 getNormal( vec3 texPosition ){
	vec3 pos = 0.5*texPosition + 0.5*vec3(1);
	vec3 up = vec3( 0 , 0.005 , 0);
	vec3 down = vec3( 0 , -0.005 , 0);
	vec3 left = vec3( -0.005 , 0 , 0);
	vec3 right = vec3( 0.005 , 0 , 0);
	vec3 front = vec3( 0 , 0 , -0.005);
	vec3 end = vec3( 0 , 0 , 0.005 );
    vec4 scalar = vec4( texture3D( data , pos + right ).s - texture3D( data , pos + left ).s ,
	 texture3D( data , pos + up ).s - texture3D( data , pos + down ).s ,
	  texture3D( data , pos + end ).s - texture3D( data , pos + front ).s , 0.0 ) ;
	  if( length( scalar ) > 0 )
		scalar.xyz = normalize( scalar.xyz );
	return scalar.xyz;
}
vec4 getColor( vec3 texPosition ){
	vec3 pos = 0.5*texPosition + 0.5*vec3(1);
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
	vec3 view = viewVec ; 
	
	vec3 scaleDim = vec3(1.0 , 1.0 , 0.9375); 

	tex *= scaleDim;
	view = tex - view;

	/**
	*  drawing on the back-plane
	*/
	vec3 n = normalize(normal);
	vec3 viewV = normalize(viewVec - textureCoords); // pos to eye
	if( dot( viewV , n ) < 0.0 ){ // view pos is within cube	  
	  vec3 np = normalize( NofViewPlan );
	  float t = dot( tex - viewVec  , np ) / dot(np , np);
	  if( t < 0.0) // exist a point in the viewplan 
		rayStart = tex - t*np - sampleSpacing;
	  
	}
	
	/**
	*  drawing on the intersection plan
	*/

	//view /= scaleDim;
	
	
	float sampleLen = sampleSpacing ;
	vec4 sampleColor = vec4(0.0);
	vec4 diffuse = vec4( 1.0 , 0.0 ,0.0 , 0.0);
	vec4 specular = vec4( 1.0 , 1.0 , 1.0  , 0.0 );
	vec3 normalv;
	lightDir = vec3(0,-1,0);
	vec3 ldir = normalize(lightDir);
	vec3 h = normalize(-viewV );
	while(true){
		samplePos = rayStart + sampleLen * view;
		sampleLen += sampleSpacing;
		// caculate normal
		normalv = getNormal( samplePos );
		float f = dot( h , normalv );
		float sp = dot( h , normalv );
		sampleColor = getColor(samplePos);
		if( f > 0.0 ) sampleColor.rgb *= f*diffuse.rgb;
		if( sp > 0.0 ) sampleColor += pow( sp , 10.0 )*specular;
		
		if( sampleColor.a > 0.001 ){
			accumulatedColor.rgb = (1.0 - accumulatedColor.a)*sampleColor.rgb + accumulatedColor.rgb;
			accumulatedColor.a = (1.0 - accumulatedColor.a)*sampleColor.a + accumulatedColor.a;
		}
		if( samplePos.x > 1.0 || samplePos.y > 1.0 || samplePos.x < -1.0 || samplePos.y < -1.0
		|| samplePos.z > 1.0 || samplePos.z < -1.0 || accumulatedColor.a > 1.0){
			break;
		}
	}
	gl_FragColor = accumulatedColor;
}