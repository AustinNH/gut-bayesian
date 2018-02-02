GLuint computeProgramID;
GLuint displayProgramID;
GLuint writeFramebufferID;
GLuint readFramebufferID;
GLuint writeTextureID;
GLuint readTextureID;

void checkForErrors(char* where){
	GLenum err = glGetError();
	while (err != GL_NO_ERROR){
		printf ("at %s, GL error code %d\n", where, err);
		err = glGetError();
	}
}

void loadShader (GLuint* shaderID, char** fileNames, int numberOfFiles, GLenum shaderType){
	FILE* file;
	char* shaderSource;
	GLint shaderSuccess;
	GLint shaderLogLength;
	GLchar* shaderLog;

	shaderSource = malloc (1024*1024*sizeof (char));//1 million chars should be enough
	int index = 0;
	for (int i = 0; i < numberOfFiles; i++){
		file = fopen (fileNames[i], "r");
		char character = fgetc (file);
		while (character != EOF){
			*(shaderSource + index*sizeof (char)) = character;
			character = fgetc (file);
			index++;
		}
		fclose (file);
	}
	*(shaderSource + index*sizeof (char)) = '\0';//null terminated string
	*shaderID = glCreateShader (shaderType);
	glShaderSource (*shaderID, 1, (const char**) &shaderSource, NULL);
	glCompileShader (*shaderID);
	free (shaderSource);
	glGetShaderiv (*shaderID, GL_COMPILE_STATUS, &shaderSuccess);
	if (shaderSuccess == GL_FALSE){
		glGetShaderiv (*shaderID, GL_INFO_LOG_LENGTH, &shaderLogLength);
		shaderLog = malloc (shaderLogLength*sizeof (GLchar));
		glGetShaderInfoLog (*shaderID, shaderLogLength, NULL, shaderLog);
		printf ("%s\n", shaderLog);
		free (shaderLog);
	}
}

void loadProgram (GLuint* programID, char* geometryFile, char* vertexFile, char** fragmentFiles, int numberOfFragmentFiles){
	GLint programSuccess;
	GLint programLogLength;
	GLchar* programLog;

	GLuint geometryShaderID;
	loadShader (&geometryShaderID, &geometryFile, 1, GL_GEOMETRY_SHADER);
	GLuint vertexShaderID;
	loadShader (&vertexShaderID, &vertexFile, 1, GL_VERTEX_SHADER);
	GLuint fragmentShaderID;
	loadShader (&fragmentShaderID, fragmentFiles, numberOfFragmentFiles, GL_FRAGMENT_SHADER);

	*programID = glCreateProgram();
	glAttachShader (*programID, geometryShaderID);
	glAttachShader (*programID, vertexShaderID);
	glAttachShader (*programID, fragmentShaderID);
	glLinkProgram (*programID);

	glGetProgramiv (*programID, GL_LINK_STATUS, &programSuccess);
	if (programSuccess == GL_FALSE){
		glGetProgramiv (*programID, GL_INFO_LOG_LENGTH, &programLogLength);
		programLog = malloc (programLogLength*sizeof (GLchar));
		glGetProgramInfoLog (*programID, programLogLength, NULL, programLog);
		printf ("%s\n", programLog);
		free (programLog);
	}
}

void initializeCompute(int windowWidth, int windowHeight, GLfloat* data, char* modelFileName){
	glClearColor (0.0, 0.0, 0.0, 1.0);
	GLuint vertexArrayID;//just needed for render call
	glGenVertexArrays (1, &vertexArrayID);
	glBindVertexArray (vertexArrayID);
	checkForErrors("vertex array creation");

	char* geometryFile = "geometry.glsl";
	char* vertexFile = "vertex.glsl";
	char* fragmentFiles[] = {NULL, NULL, NULL};
	fragmentFiles[0] = "fragment.glsl";
	fragmentFiles[1] = modelFileName;
	fragmentFiles[2] = "fragmentMain.glsl";
	loadProgram (&computeProgramID, geometryFile, vertexFile, fragmentFiles, 3);
	checkForErrors("at compute program creation");

	fragmentFiles[0] = "displayFragment.glsl";
	loadProgram (&displayProgramID, geometryFile, vertexFile, fragmentFiles, 1);
	checkForErrors("at display program creation");
	
	glGenTextures (1, &writeTextureID);
	checkForErrors("texture generation");
	glBindTexture (GL_TEXTURE_2D, writeTextureID);
	glTexStorage2D (GL_TEXTURE_2D, 1, GL_RGBA32F, windowWidth, windowHeight);
	/*glTexImage2D (
		GL_TEXTURE_2D,
		0,
		GL_RGBA32F,
		windowWidth,
		windowHeight,
		0,
		GL_RGBA,
		GL_FLOAT,
		(GLfloat*)0);*/
		//data);//when I call this it writes into the texture, but then i can't read the texture in shader with texelFetch? weird. instead use glClear to clear it a few lines down
	glGenTextures (1, &readTextureID);
	glBindTexture (GL_TEXTURE_2D, readTextureID);
	glTexStorage2D (GL_TEXTURE_2D, 1, GL_RGBA32F, windowWidth, windowHeight);
	/*glTexImage2D (
		GL_TEXTURE_2D,
		0,
		GL_RGBA32F,
		windowWidth,
		windowHeight,
		0,
		GL_RGBA,
		GL_FLOAT,
		(GLfloat*)0);
		//data);*/
	glBindTexture (GL_TEXTURE_2D, 0);
	checkForErrors("at texture creation");


	glGenFramebuffers (1, &writeFramebufferID);
	glBindFramebuffer (GL_FRAMEBUFFER, writeFramebufferID);
	glFramebufferTexture (GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, writeTextureID, 0);
	glClear (GL_COLOR_BUFFER_BIT);
	glGenFramebuffers (1, &readFramebufferID);
	glBindFramebuffer (GL_FRAMEBUFFER, readFramebufferID);
	glFramebufferTexture (GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, readTextureID, 0);
	glClear (GL_COLOR_BUFFER_BIT);
	glBindFramebuffer (GL_FRAMEBUFFER, 0);
	checkForErrors("at framebuffer creation");
}

void pingPong (){
	GLuint textureIDSwap = readTextureID;
	readTextureID = writeTextureID;
	writeTextureID = textureIDSwap;
	GLuint framebufferIDSwap = readFramebufferID;
	readFramebufferID = writeFramebufferID;
	writeFramebufferID = framebufferIDSwap;
	checkForErrors("ping pong");
}

void compute (int iteration, int numberOfSamples, float zeroToOneScan){
	pingPong();
	glBindFramebuffer (GL_FRAMEBUFFER, writeFramebufferID);
	glBindTexture (GL_TEXTURE_2D, readTextureID);
	glClear (GL_COLOR_BUFFER_BIT);
	glUseProgram (computeProgramID);
	glUniform1i (glGetUniformLocation (computeProgramID, "iteration"), iteration);
	glUniform1i (glGetUniformLocation (computeProgramID, "numberOfSamples"), numberOfSamples);
	glUniform1f (glGetUniformLocation (computeProgramID, "zeroToOneScan"), zeroToOneScan);
	glDrawArrays (GL_POINTS, 0, 1);//this command seems to eat cpu on nvidia, but not on intel integrated graphics
	glBindTexture (GL_TEXTURE_2D, 0);
	glBindFramebuffer (GL_FRAMEBUFFER, 0);
	checkForErrors("compute");
}

void wait(){
	glFinish();
}

void readData (float* data){
	glBindTexture (GL_TEXTURE_2D, writeTextureID);
	glGetTexImage (
		GL_TEXTURE_2D,
		0,
		GL_RGBA,
		GL_FLOAT,
		data);
	glBindTexture (GL_TEXTURE_2D, 0);
}

void display(){
	glBindFramebuffer (GL_FRAMEBUFFER, 0);
	glBindTexture (GL_TEXTURE_2D, writeTextureID);
	glClear (GL_COLOR_BUFFER_BIT);
	glUseProgram (displayProgramID);
	glDrawArrays (GL_POINTS, 0, 1);
	glBindTexture (GL_TEXTURE_2D, 0);
	swapBuffers();
	checkForErrors("display");
}