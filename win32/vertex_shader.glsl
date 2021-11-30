	#version 150 core
	in vec3 pos;
	in vec3 material_ambient;
	in vec3 material_diffuse;
	in vec3 material_specular;
	in float material_shininess;
	in vec2 uv;
	in float tid;
	in float which;
	in vec3 norm;
	
	out vec3 fPos;      			// Output to geometry (or fragment) shader
	out vec3 fMaterial_ambient;		// Output to geometry (or fragment) shader
	out vec3 fMaterial_diffuse;		// Output to geometry (or fragment) shader
	out vec3 fMaterial_specular;	// Output to geometry (or fragment) shader
	out float fMaterial_shininess;	// Output to geometry (or fragment) shader
	out vec2 fUv;       // Output to geometry (or fragment) shader
	out float fTid;     // Output to geometry (or fragment) shader
	out float fWhich;   // Output to geometry (or fragment) shader
	out vec3 fNorm;     // Output to geometry (or fragment) shader

	uniform mat4 model_matrix;
    uniform mat4 view_matrix;
    uniform mat4 projection_matrix;

	void main()
	{
        gl_Position = projection_matrix * view_matrix * model_matrix * vec4(pos, 1.0);
        fPos = pos;
        fMaterial_ambient = material_ambient;
		fMaterial_diffuse = material_diffuse;
		fMaterial_specular = material_specular;
		fMaterial_shininess = material_shininess;
        fUv = uv;
        fTid = tid;
        fWhich = which;
        fNorm = norm;
	};