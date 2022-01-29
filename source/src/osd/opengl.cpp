/** @file opengl.cpp

	Skelton for retropc emulator

	@author Sasaji
	@date   2020.03.08

	@brief [ OpenGL ]
*/


#include "../common.h"

#if defined(USE_OPENGL)

#include "opengl.h"
#include "../csurface.h"
#include "../logging.h"

//

COpenGL::COpenGL()
{
}
COpenGL::~COpenGL()
{
}
COpenGL *COpenGL::Create(int force)
{
	const GLubyte *glsl_version  = NULL;

	const GLubyte *vender   = glGetString(GL_VENDOR);
	const GLubyte *renderer = glGetString(GL_RENDERER);
	const GLubyte *version  = glGetString(GL_VERSION);

	logging->out_logf(LOG_DEBUG, "OPENGL: VENDER:%s RENDERER:%s VERSION:%s", vender, renderer, version);

#ifdef GL_SHADING_LANGUAGE_VERSION
	glsl_version  = glGetString(GL_SHADING_LANGUAGE_VERSION);
#endif
#ifdef USE_OPENGL33
	if (glsl_version && ((force == 0 && glsl_version[0] >= '3' && glsl_version[0] <= '9') || force == 3)) {
		logging->out_logf(LOG_DEBUG, "GLSL:%s  Use COpenGL3", (const char *)glsl_version);
		return new COpenGL3();
	}
#endif
#ifdef USE_OPENGL21
	if (glsl_version && ((force == 0 && glsl_version[0] == '1') || force == 2)) {
		logging->out_logf(LOG_DEBUG, "GLSL:%s  Use COpenGL2", (const char *)glsl_version);
		return new COpenGL2();
	}
#endif
	logging->out_logf(LOG_DEBUG, "GLSL:%s  Use COpenGL1", glsl_version != NULL ? (const char *)glsl_version : "(null)");
	return new COpenGL1();
}

int COpenGL::Version() const
{
	return 0;
}

bool COpenGL::Initialize()
{
	return false;
}
void COpenGL::Terminate()
{
}
bool COpenGL::InitViewport(int, int)
{
	return false;
}
GLuint COpenGL::CreateTexture(int)
{
	return 0;
}
void COpenGL::SetTextureFilter(int)
{
}
GLuint COpenGL::ReleaseTexture()
{
	return 0;
}
bool COpenGL::CreateBuffer(GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat)
{
	return false;
}
void COpenGL::ReleaseBuffer()
{
}
bool COpenGL::SetTexturePos(GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat)
{
	return false;
}
unsigned char COpenGL::SetInterval(unsigned char use)
{
	/// Sync VSYNC set
#if defined(_WIN32)
	const char *ext = reinterpret_cast<const char *>(glGetString(GL_EXTENSIONS));
	if (ext && strstr(ext, "WGL_EXT_swap_control")) {
		void (WINAPI *wglSwapIntervalEXT)(int);
		int (WINAPI *wglGetSwapIntervalEXT)();

		wglSwapIntervalEXT = reinterpret_cast<void (WINAPI *)(int)>(wglGetProcAddress("wglSwapIntervalEXT"));
		if (wglSwapIntervalEXT != nullptr) {
			if ( use == 1 ) {
				wglSwapIntervalEXT(1);
			} else {
				wglSwapIntervalEXT(0);
			}
		}
		wglGetSwapIntervalEXT = reinterpret_cast<int (WINAPI *)()>(wglGetProcAddress("wglGetSwapIntervalEXT"));
		if (wglGetSwapIntervalEXT != nullptr) {
			int interval = wglGetSwapIntervalEXT();
			use = (interval == 1 ? 1 : 2);
		}
	}
#endif
	return use;
}
void COpenGL::ClearScreen()
{
}

//

COpenGL1::COpenGL1()
{
	textureId = 0;
	tex_l = tex_t = 0.0f;
	tex_r = tex_b = 1.0f;
	pyl_l = pyl_t = -1.0f;
	pyl_r = pyl_b =  1.0f;
	disp_w = disp_h = 1;
}

COpenGL1::~COpenGL1()
{
	ReleaseTexture();
}

int COpenGL1::Version() const
{
	return 1;
}

bool COpenGL1::Initialize()
{
	return COpenGL::Initialize();
}

void COpenGL1::Terminate()
{
	ReleaseTexture();
}

bool COpenGL1::InitViewport(int w, int h)
{
	// shading
	glShadeModel( GL_SMOOTH );
	// culling
	glFrontFace( GL_CCW );
	glEnable( GL_CULL_FACE );
	glCullFace( GL_BACK );
	// bg color
	glClearColor( 0.0, 0.0, 0.0, 1.0 );

	// Setup our viewport : same as window size
	glViewport( 0, 0, w, h );

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();

	// change axis unit
#ifdef USE_OPENGL_WH_ORTHO
	glOrtho(0.0, (GLdouble)w, (GLdouble)h, 0.0, -1.0, 1.0);
#else
	glOrtho(-1.0, 1.0, 1.0, -1.0, -1.0, 1.0);
#endif

	disp_w = w;
	disp_h = h;

	return true;
}

GLuint COpenGL1::CreateTexture(int filter)
{
	GLenum err;

#if defined(_RGB555) || defined(_RGB565)
	glPixelStorei(GL_UNPACK_ALIGNMENT, 2);
#elif defined(_RGB888)
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
#else
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
#endif

	glGenTextures(1, &textureId);
	while ((err = glGetError()) != GL_NO_ERROR) {
		logging->out_logf(LOG_ERROR, _T("COpenGL1::CreateTexture::glGenTextures: 0x%x"), err);
	}
	logging->out_debugf(_T("COpenGL1::CreateTexture::textureId: %d"), textureId);

	SetTextureFilter(filter);

	// fill black
	glBegin(GL_QUADS);
	glColor3ub(0, 0, 0);
#ifdef USE_OPENGL_WH_ORTHO
	glVertex3i(0, 0, 0);
	glVertex3i(0, disp_h, 0);
	glVertex3i(disp_w, disp_h, 0);
	glVertex3i(disp_w, 0, 0);
#else
	glVertex3i(-1, -1, 0);
	glVertex3i(-1,  1, 0);
	glVertex3i( 1,  1, 0);
	glVertex3i( 1, -1, 0);
#endif
	glEnd();

	return textureId;
}

void COpenGL1::SetTextureFilter(int filter)
{
	GLenum err;

	if (!textureId) return;

	glBindTexture(GL_TEXTURE_2D, textureId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST + filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST + filter);
	while ((err = glGetError()) != GL_NO_ERROR) {
		logging->out_logf(LOG_ERROR, _T("COpenGL1::SetTextureFilter::glTexParameteri FILTER 0x%x"), err);
	}
	glTexParameteri(GL_TEXTURE_2D , GL_TEXTURE_WRAP_S, GL_CLAMP);	// no wrap texture
	glTexParameteri(GL_TEXTURE_2D , GL_TEXTURE_WRAP_T, GL_CLAMP);
	while ((err = glGetError()) != GL_NO_ERROR) {
		logging->out_logf(LOG_ERROR, _T("COpenGL1::SetTextureFilter::glTexParameteri WRAP 0x%x"), err);
	}
	glBindTexture(GL_TEXTURE_2D, 0);	// unbound
}

GLuint COpenGL1::ReleaseTexture()
{
	if (textureId) {
		// release texture
		glDeleteTextures(1, &textureId);
		logging->out_debug(_T("COpenGL1::ReleaseTexture"));
		textureId = 0;
	}
	return textureId;
}

/// @param[in] pyl_left    left side of polygon
/// @param[in] pyl_top     top side of polygon
/// @param[in] pyl_right   right side of polygon
/// @param[in] pyl_bottom  bottom side of polygon
/// @param[in] tex_left    left side of texture
/// @param[in] tex_top     top side of texture
/// @param[in] tex_right   right side of texture
/// @param[in] tex_bottom  bottom side of texture
bool COpenGL1::CreateBuffer(GLfloat pyl_left, GLfloat pyl_top, GLfloat pyl_right, GLfloat pyl_bottom, GLfloat tex_left, GLfloat tex_top, GLfloat tex_right, GLfloat tex_bottom)
{
	pyl_l = pyl_left;
	pyl_r = pyl_right;
	pyl_t = pyl_top;
	pyl_b = pyl_bottom;
	tex_l = tex_left;
	tex_t = tex_top;
	tex_r = tex_right;
	tex_b = tex_bottom;
	return true;
}
bool COpenGL1::SetTexturePos(GLfloat pyl_left, GLfloat pyl_top, GLfloat pyl_right, GLfloat pyl_bottom, GLfloat tex_left, GLfloat tex_top, GLfloat tex_right, GLfloat tex_bottom)
{
	pyl_l = pyl_left;
	pyl_r = pyl_right;
	pyl_t = pyl_top;
	pyl_b = pyl_bottom;
	tex_l = tex_left;
	tex_t = tex_top;
	tex_r = tex_right;
	tex_b = tex_bottom;
	return true;
}

void COpenGL1::Draw(CSurface *surface)
{
	Draw(surface->Width(), surface->Height(), surface->GetBuffer());
}
void COpenGL1::Draw(int width, int height, void *buffer)
{
	GLenum err;

	glClear(GL_COLOR_BUFFER_BIT);

	// draw texture using screen pixel buffer
	glBindTexture(GL_TEXTURE_2D, textureId);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
			GL_UNSIGNED_BYTE, static_cast<GLvoid *>(buffer));
	while ((err = glGetError()) != GL_NO_ERROR) {
		logging->out_logf(LOG_ERROR, _T("COpenGL1::Draw::glTexImage2D: 0x%x"), err);
	}

	// fill black
	glBegin(GL_QUADS);
		glColor3ub(0, 0, 0);
#ifdef USE_OPENGL_WH_ORTHO
		glVertex3i(0, 0, 0);
		glVertex3i(0, disp_h, 0);
		glVertex3i(disp_w, disp_h, 0);
		glVertex3i(disp_w, 0, 0);
#else
		glVertex3i(-1, -1, 0);
		glVertex3i(-1,  1, 0);
		glVertex3i( 1,  1, 0);
		glVertex3i( 1, -1, 0);
#endif
	glEnd();

	glEnable(GL_TEXTURE_2D);
	glColor3ub(255, 255, 255);
	glBegin(GL_QUADS);
		// anticlockwise
		glTexCoord2f(tex_l, tex_t);
		glVertex3f(pyl_l, pyl_t, 0.0f);
		glTexCoord2f(tex_l, tex_b);
		glVertex3f(pyl_l, pyl_b, 0.0f);
		glTexCoord2f(tex_r, tex_b);
		glVertex3f(pyl_r, pyl_b, 0.0f);
		glTexCoord2f(tex_r, tex_t);
		glVertex3f(pyl_r, pyl_t, 0.0f);
	glEnd();
	glDisable(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);
}
void COpenGL1::ClearScreen()
{
	// fill black
	glBegin(GL_QUADS);
	glColor3ub(0, 0, 0);
#ifdef USE_OPENGL_WH_ORTHO
	glVertex3i(0, 0, 0);
	glVertex3i(0, disp_h, 0);
	glVertex3i(disp_w, disp_h, 0);
	glVertex3i(disp_w, 0, 0);
#else
	glVertex3i(-1, -1, 0);
	glVertex3i(-1,  1, 0);
	glVertex3i( 1,  1, 0);
	glVertex3i( 1, -1, 0);
#endif
	glEnd();
}

//
#ifdef USE_OPENGL21

COpenGL2::COpenGL2()
	: COpenGL()
{
	posBufferId = 0;
	texPosBufferId = 0;

	textureLoc = -1;
	positionLoc = -1;
	texPositionLoc = -1;
	textureId = 0;

	vertexId = 0;
	fragmentId = 0;
	programId = 0;
}

COpenGL2::~COpenGL2()
{
	ReleaseProgram();
	ReleaseTexture();
	ReleaseBuffer();
}

int COpenGL2::Version() const
{
	return 2;
}

bool COpenGL2::Initialize()
{
#ifdef USE_OPENGL_GLEW
	glewInit();
#endif

	COpenGL::Initialize();

	if (!BuildProgram()) {
		return false;
	}
	return true;
}

void COpenGL2::Terminate()
{
	ReleaseProgram();
}

bool COpenGL2::InitViewport(int w, int h)
{
	// bg color
	glClearColor( 0.0, 0.0, 0.0, 1.0 );

	// Setup our viewport : same as window size
	glViewport( 0, 0, w, h );

	return true;
}

/// create texture
/// @param[in] filter  GL_NEAREST/GL_LINEAR
GLuint COpenGL2::CreateTexture(int filter)
{
	GLenum err;

	glActiveTexture(GL_TEXTURE0);

	// create
#if defined(_RGB555) || defined(_RGB565)
	glPixelStorei(GL_UNPACK_ALIGNMENT, 2);
#elif defined(_RGB888)
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
#else
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
#endif

	glGenTextures(1, &textureId);
	while ((err = glGetError()) != GL_NO_ERROR) {
		logging->out_logf(LOG_ERROR, _T("COpenGL2::CreateTexture::glGenTextures: 0x%x"), err);
	}
	logging->out_debugf(_T("COpenGL2::CreateTexture::textureId: %d"), textureId);

	SetTextureFilter(filter);

	textureLoc = glGetUniformLocation(programId, "myTextureSample");
	while((err = glGetError()) != GL_NO_ERROR) {
		logging->out_logf(LOG_ERROR, _T("COpenGL2::CreateTexture::glGetUniformLocation: err:0x%x"), err);
	}
	logging->out_debugf(_T("COpenGL2::CreateTexture::textureLoc: %d"), textureLoc);

	return textureId;
}

/// set filter type
/// @param[in] filter  GL_NEAREST/GL_LINEAR
void COpenGL2::SetTextureFilter(int filter)
{
	GLenum err;

	if (!textureId) return;

	glBindTexture(GL_TEXTURE_2D, textureId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST + filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST + filter);
	while ((err = glGetError()) != GL_NO_ERROR) {
		logging->out_logf(LOG_ERROR, _T("COpenGL2::SetTextureFilter::glTexParameteri FILTER 0x%x"), err);
	}
	glTexParameteri(GL_TEXTURE_2D , GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	// no wrap texture
	glTexParameteri(GL_TEXTURE_2D , GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	while ((err = glGetError()) != GL_NO_ERROR) {
		logging->out_logf(LOG_ERROR, _T("COpenGL2::SetTextureFilter::glTexParameteri WRAP 0x%x"), err);
	}
}

GLuint COpenGL2::ReleaseTexture()
{
	if (textureId) {
		// release texture
		glDeleteTextures(1, &textureId);
		textureId = 0;
	}
	return textureId;
}

/// create buffer
/// @param[in] pyl_left    left side of polygon (-1.0-1.0)
/// @param[in] pyl_top     top side of polygon (-1.0-1.0)
/// @param[in] pyl_right   right side of polygon (-1.0-1.0)
/// @param[in] pyl_bottom  bottom side of polygon (-1.0-1.0)
/// @param[in] tex_left    left side of texture (0.0-1.0)
/// @param[in] tex_top     top side of texture (0.0-1.0)
/// @param[in] tex_right   right side of texture (0.0-1.0)
/// @param[in] tex_bottom  bottom side of texture (0.0-1.0)
/// @return true/false
bool COpenGL2::CreateBuffer(GLfloat pyl_left, GLfloat pyl_top, GLfloat pyl_right, GLfloat pyl_bottom, GLfloat tex_left, GLfloat tex_top, GLfloat tex_right, GLfloat tex_bottom)
{
	if (!SetTexturePos(pyl_left, pyl_top, pyl_right, pyl_bottom, tex_left, tex_top, tex_right, tex_bottom)) {
		return false;
	}

	return true;
}

bool COpenGL2::SetTexturePos(GLfloat pyl_left, GLfloat pyl_top, GLfloat pyl_right, GLfloat pyl_bottom, GLfloat tex_left, GLfloat tex_top, GLfloat tex_right, GLfloat tex_bottom)
{
	GLenum err;


	if (posBufferId == 0) {
		glGenBuffers(1, &posBufferId);
		while((err = glGetError()) != GL_NO_ERROR) {
			logging->out_logf(LOG_ERROR, _T("COpenGL2::SetTexturePos::glGenBuffers: V err:0x%x"), err);
		}
		logging->out_debugf(_T("COpenGL2::SetTexturePos::glGenBuffers: vId:%d"), posBufferId);

		glBindBuffer(GL_ARRAY_BUFFER, posBufferId);
		positionLoc = glGetAttribLocation(programId, "vPosition");
		while((err = glGetError()) != GL_NO_ERROR) {
			logging->out_logf(LOG_ERROR, _T("COpenGL2::SetTexturePos::glGetAttribLocation: vPosition err:0x%x"), err);
		}
		logging->out_debugf(_T("COpenGL2::SetTexturePos::glGetAttribLocation: positionLoc:%d"), positionLoc);
	}

	// vartex array for polygon
	GLfloat varr[12];
	varr[ 0] = pyl_left; varr[ 1] = pyl_bottom; varr[ 2] = 0.0f;
	varr[ 3] = pyl_right; varr[ 4] = pyl_bottom; varr[ 5] = 0.0f;
	varr[ 6] = pyl_left; varr[ 7] = pyl_top; varr[ 8] = 0.0f;
	varr[ 9] = pyl_right; varr[10] = pyl_top; varr[11] = 0.0f;

	glBindBuffer(GL_ARRAY_BUFFER, posBufferId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(varr), varr, GL_STATIC_DRAW);
	while((err = glGetError()) != GL_NO_ERROR) {
		logging->out_logf(LOG_ERROR, _T("COpenGL2::SetTexturePos::glBufferData: V err:0x%x"), err);
	}

	if (texPosBufferId == 0) {
		glGenBuffers(1, &texPosBufferId);
		while((err = glGetError()) != GL_NO_ERROR) {
			logging->out_logf(LOG_ERROR, _T("COpenGL2::SetTexturePos::glGenBuffers: UV err:0x%x"), err);
		}
		logging->out_debugf(_T("COpenGL2::SetTexturePos::glGenBuffers: uvId:%d"), texPosBufferId);

		glBindBuffer(GL_ARRAY_BUFFER, texPosBufferId);
		texPositionLoc = glGetAttribLocation(programId, "vUV");
		while((err = glGetError()) != GL_NO_ERROR) {
			logging->out_logf(LOG_ERROR, _T("COpenGL2::SetTexturePos::glGetAttribLocation: vUV err:0x%x"), err);
		}
		logging->out_debugf(_T("COpenGL2::SetTexturePos::glGetAttribLocation: texPositionLoc:%d"), texPositionLoc);
	}

	// vertex array for texture
	GLfloat uvarr[8];
	uvarr[ 0] = tex_left;  uvarr[ 1] = tex_top;
	uvarr[ 2] = tex_right; uvarr[ 3] = tex_top;
	uvarr[ 4] = tex_left;  uvarr[ 5] = tex_bottom;
	uvarr[ 6] = tex_right; uvarr[ 7] = tex_bottom;

	glBindBuffer(GL_ARRAY_BUFFER, texPosBufferId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(uvarr), uvarr, GL_STATIC_DRAW);
	while((err = glGetError()) != GL_NO_ERROR) {
		logging->out_logf(LOG_ERROR, _T("COpenGL2::SetTexturePos::glBufferData: UV err:0x%x"), err);
	}


	// vertex
	glBindBuffer(GL_ARRAY_BUFFER, posBufferId);
	glEnableVertexAttribArray(positionLoc);
	glVertexAttribPointer(positionLoc, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);
//	glDisableVertexAttribArray(positionLoc);
	while((err = glGetError()) != GL_NO_ERROR) {
		logging->out_logf(LOG_ERROR, _T("COpenGL2::SetTexturePos:glVertexAttribPointer positionLoc err:0x%x"), err);
	}

	// texture
	glBindBuffer(GL_ARRAY_BUFFER, texPosBufferId);
	glEnableVertexAttribArray(texPositionLoc);
	glVertexAttribPointer(texPositionLoc, 2, GL_FLOAT, GL_FALSE, 0, (void *)0);
//	glDisableVertexAttribArray(texPositionLoc);
	while((err = glGetError()) != GL_NO_ERROR) {
		logging->out_logf(LOG_ERROR, _T("COpenGL2::SetTexturePos:glVertexAttribPointer texPositionLoc err:0x%x"), err);
	}

	return true;
}

/// release buffer
void COpenGL2::ReleaseBuffer()
{
	if (texPosBufferId) {
		glDeleteBuffers(1, &texPosBufferId);
		texPosBufferId = 0;
	}
	if (posBufferId) {
		glDeleteBuffers(1, &posBufferId);
		posBufferId = 0;
	}
}

/// build (compile and link) shader programs
bool COpenGL2::BuildProgram()
{
	if (!CompileVertex()) {
		return false;
	}
	if (!CompileFragment()) {
		return false;
	}
	if (!LinkProgram()) {
		return false;
	}
	return true;
}

/// release program
void COpenGL2::ReleaseProgram()
{
	if (programId) {
		glDeleteProgram(programId);
		programId = 0;
	}
	if (vertexId) {
		glDeleteShader(vertexId);
		vertexId = 0;
	}
	if (fragmentId) {
		glDeleteShader(fragmentId);
		fragmentId = 0;
	}
}

/// compile a shader program
/// @param[in]  type  GL_VERTEX_SHADER/GL_FRAGMENT_SHADER/GL_GEOMETRY_SHADER
/// @param[out] id    shader id
/// @param[in]  prog  program list
/// @return true/false
bool COpenGL2::CompileShader(GLenum type, GLuint &id, const GLchar *prog)
{
	GLenum err;

	id = glCreateShader(type);
	while((err = glGetError()) != GL_NO_ERROR) {
		logging->out_logf(LOG_ERROR, _T("COpenGL2::Compile::glCreateShader: type:0x%x err:0x%x"), type, err);
	}
	logging->out_debugf(_T("COpenGL2::Compile::glCreateShader: type:0x%x id:%d"), type, id);

	glShaderSource(id, 1, &prog, NULL);
	while((err = glGetError()) != GL_NO_ERROR) {
		logging->out_logf(LOG_ERROR, _T("COpenGL2::Compile::glShaderSource: type:0x%x err:0x%x"), type, err);
	}
	glCompileShader(id);
	while((err = glGetError()) != GL_NO_ERROR) {
		logging->out_logf(LOG_ERROR, _T("COpenGL2::Compile::glCompileShader: type:0x%x err:0x%x"), type, err);
	}
	// check compile error
	GLint result = GL_FALSE;
	int len = 0;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	glGetShaderiv(id, GL_INFO_LOG_LENGTH, &len);
	if (len > 0) {
		GLchar *msg = new GLchar[len + 1];
		if (msg) {
			glGetShaderInfoLog(id, len, NULL, msg);
			logging->out_logf(LOG_ERROR, _T("COpenGL2::CompileError: type:0x%x %s"), type, msg);
			delete [] msg;
		}
	}

	return (result == GL_TRUE);
}

/// compile the vertex shader
bool COpenGL2::CompileVertex()
{
	const GLchar *vertexProg =
"#version 120\n"
"attribute vec3 vPosition;\n"
"attribute vec2 vUV;\n"
"varying vec2 UV;\n"
"void main() {\n"
"  gl_Position = vec4(vPosition, 1.0);\n"
"  UV = vUV;\n"
"}\n";
	return CompileShader(GL_VERTEX_SHADER, vertexId, vertexProg);
}

/// compile the fragment shader
bool COpenGL2::CompileFragment()
{
	const GLchar *fragmentProg =
"#version 120\n"
"varying vec2 UV;\n"
"uniform sampler2D myTextureSample;\n"
"void main() {\n"
"  gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0) * texture2D(myTextureSample, UV);\n"
"}\n";
	return CompileShader(GL_FRAGMENT_SHADER, fragmentId, fragmentProg);
}

/// link vertex and fragment shader program
bool COpenGL2::LinkProgram()
{
	GLenum err;

	programId = glCreateProgram();
	while((err = glGetError()) != GL_NO_ERROR) {
		logging->out_logf(LOG_ERROR, _T("COpenGL2::Link::glCreateProgram: err:0x%x"), err);
	}
	logging->out_debugf(_T("COpenGL2::Link::glCreateProgram: id:%d"), programId);

	glAttachShader(programId, vertexId);
	glAttachShader(programId, fragmentId);
	while((err = glGetError()) != GL_NO_ERROR) {
		logging->out_logf(LOG_ERROR, _T("COpenGL2::Link::glAttachShader: err:0x%x"), err);
	}

	glLinkProgram(programId);
	while((err = glGetError()) != GL_NO_ERROR) {
		logging->out_logf(LOG_ERROR, _T("COpenGL2::Link::glLinkProgram: err:0x%x"), err);
	}
	// check compile error
	GLint result = GL_FALSE;
	int len = 0;
	glGetProgramiv(programId, GL_LINK_STATUS, &result);
	glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &len);
	if (len > 0) {
		GLchar *msg = new GLchar[len + 1];
		if (msg) {
			glGetProgramInfoLog(programId, len, NULL, msg);
			logging->out_logf(LOG_ERROR, _T("COpenGL2::LinkError: %s"), msg);
			delete [] msg;
		}
	}
	glDetachShader(programId, vertexId);
	glDetachShader(programId, fragmentId);

	glDeleteShader(vertexId);
	vertexId = 0;
	glDeleteShader(fragmentId);
	fragmentId = 0;

	while((err = glGetError()) != GL_NO_ERROR) {
		logging->out_logf(LOG_ERROR, _T("COpenGL2::Link: err:0x%x"), err);
	}

	return (result == GL_TRUE);
}

void COpenGL2::UseProgram()
{
	glUseProgram(programId);
}

/// draw texture on the window
void COpenGL2::Draw(CSurface *surface)
{
	Draw(surface->Width(), surface->Height(), surface->GetBuffer());
}
void COpenGL2::Draw(int width, int height, void *buffer)
{
	GLenum err;

	UseProgram();

	glClear(GL_COLOR_BUFFER_BIT);

	glActiveTexture(GL_TEXTURE0);

	// draw texture using screen pixel buffer
	glBindTexture(GL_TEXTURE_2D, textureId);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
			GL_UNSIGNED_BYTE, (GLvoid *)buffer);
	while ((err = glGetError()) != GL_NO_ERROR) {
		logging->out_logf(LOG_ERROR, _T("COpenGL2::Draw::glTexImage2D: 0x%x"), err);
	}

	// bind texture0 to texture in program
	glUniform1i(textureLoc, 0);

	// draw vertex
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	while ((err = glGetError()) != GL_NO_ERROR) {
		logging->out_logf(LOG_ERROR, _T("COpenGL2::Draw: 0x%x"), err);
	}
}
void COpenGL2::ClearScreen()
{
	// fill black
	glBegin(GL_QUADS);
	glColor3ub(0, 0, 0);
	glVertex3i(-1, -1, 0);
	glVertex3i(-1,  1, 0);
	glVertex3i( 1,  1, 0);
	glVertex3i( 1, -1, 0);
	glEnd();
}
#endif /* USE_OPENGL21 */

//
#ifdef USE_OPENGL33

COpenGL3::COpenGL3()
	: COpenGL()
{
	vaoId = 0;

	posBufferId = 0;
	texPosBufferId = 0;

	textureLoc = -1;
	textureId = 0;

	vertexId = 0;
	fragmentId = 0;
	programId = 0;
}

COpenGL3::~COpenGL3()
{
	ReleaseProgram();
	ReleaseTexture();
	ReleaseBuffer();
	ReleaseVao();
}

int COpenGL3::Version() const
{
	return 3;
}

bool COpenGL3::CreateVao()
{
	GLenum err;

#if defined(__APPLE__) && defined(__MACH__)
	glGenVertexArraysAPPLE(1, &vaoId);
#else
	glGenVertexArrays(1, &vaoId);
#endif
	while((err = glGetError()) != GL_NO_ERROR) {
		logging->out_logf(LOG_ERROR, _T("COpenGL3::CreateVao::glGenVertexArrays: err:0x%x"), err);
	}
	logging->out_debugf(_T("COpenGL3::CreateVao::glGenVertexArrays: id:%d"), vaoId);

#if defined(__APPLE__) && defined(__MACH__)
	glBindVertexArrayAPPLE(vaoId);
#else
	glBindVertexArray(vaoId);
#endif

	return true;
}

void COpenGL3::ReleaseVao()
{
	if (vaoId) {
#if defined(__APPLE__) && defined(__MACH__)
		glDeleteVertexArraysAPPLE(1, &vaoId);
#else
		glDeleteVertexArrays(1, &vaoId);
#endif
		vaoId = 0;
	}
}

bool COpenGL3::Initialize()
{
#ifdef USE_OPENGL_GLEW
	glewInit();
#endif

	COpenGL::Initialize();

	if (!CreateVao()) {
		return false;
	}
	if (!BuildProgram()) {
		return false;
	}
	return true;
}

void COpenGL3::Terminate()
{
	ReleaseProgram();
	ReleaseVao();
}

bool COpenGL3::InitViewport(int w, int h)
{
	// bg color
	glClearColor( 0.0, 0.0, 0.0, 1.0 );

	// Setup our viewport : same as window size
	glViewport( 0, 0, w, h );

	return true;
}

/// create texture
/// @param[in] filter  GL_NEAREST/GL_LINEAR
GLuint COpenGL3::CreateTexture(int filter)
{
	GLenum err;

	// create
#if defined(_RGB555) || defined(_RGB565)
	glPixelStorei(GL_UNPACK_ALIGNMENT, 2);
#elif defined(_RGB888)
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
#else
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
#endif

	glGenTextures(1, &textureId);
	while ((err = glGetError()) != GL_NO_ERROR) {
		logging->out_logf(LOG_ERROR, _T("COpenGL3::CreateTexture::glGenTextures: 0x%x"), err);
	}
	logging->out_debugf(_T("COpenGL3::CreateTexture::textureId: %d"), textureId);

	SetTextureFilter(filter);

	textureLoc = glGetUniformLocation(programId, "myTextureSample");
	while((err = glGetError()) != GL_NO_ERROR) {
		logging->out_logf(LOG_ERROR, _T("COpenGL3::CreateTexture::glGetUniformLocation: err:0x%x"), err);
	}
	logging->out_debugf(_T("COpenGL3::CreateTexture::textureLoc: %d"), textureLoc);

	return textureId;
}

/// set filter type
/// @param[in] filter  GL_NEAREST/GL_LINEAR
void COpenGL3::SetTextureFilter(int filter)
{
	GLenum err;

	if (!textureId) return;

	glBindTexture(GL_TEXTURE_2D, textureId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST + filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST + filter);
	while ((err = glGetError()) != GL_NO_ERROR) {
		logging->out_logf(LOG_ERROR, _T("COpenGL3::SetTextureFilter::glTexParameteri FILTER 0x%x"), err);
	}
	glTexParameteri(GL_TEXTURE_2D , GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	// no wrap texture
	glTexParameteri(GL_TEXTURE_2D , GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	while ((err = glGetError()) != GL_NO_ERROR) {
		logging->out_logf(LOG_ERROR, _T("COpenGL3::SetTextureFilter::glTexParameteri WRAP 0x%x"), err);
	}
}

GLuint COpenGL3::ReleaseTexture()
{
	if (textureId) {
		// release texture
		glDeleteTextures(1, &textureId);
		textureId = 0;
	}
	return textureId;
}

/// create buffer
/// @param[in] pyl_left    left side of polygon (-1.0-1.0)
/// @param[in] pyl_top     top side of polygon (-1.0-1.0)
/// @param[in] pyl_right   right side of polygon (-1.0-1.0)
/// @param[in] pyl_bottom  bottom side of polygon (-1.0-1.0)
/// @param[in] tex_left    left side of texture (0.0-1.0)
/// @param[in] tex_top     top side of texture (0.0-1.0)
/// @param[in] tex_right   right side of texture (0.0-1.0)
/// @param[in] tex_bottom  bottom side of texture (0.0-1.0)
/// @return true/false
bool COpenGL3::CreateBuffer(GLfloat pyl_left, GLfloat pyl_top, GLfloat pyl_right, GLfloat pyl_bottom, GLfloat tex_left, GLfloat tex_top, GLfloat tex_right, GLfloat tex_bottom)
{
	if (!SetTexturePos(pyl_left, pyl_top, pyl_right, pyl_bottom, tex_left, tex_top, tex_right, tex_bottom)) {
		return false;
	}

	return true;
}

bool COpenGL3::SetTexturePos(GLfloat pyl_left, GLfloat pyl_top, GLfloat pyl_right, GLfloat pyl_bottom, GLfloat tex_left, GLfloat tex_top, GLfloat tex_right, GLfloat tex_bottom)
{
	GLenum err;


	if (posBufferId == 0) {
		glGenBuffers(1, &posBufferId);
		while((err = glGetError()) != GL_NO_ERROR) {
			logging->out_logf(LOG_ERROR, _T("COpenGL3::SetTexturePos::glGenBuffers: V err:0x%x"), err);
		}
		logging->out_debugf(_T("COpenGL3::SetTexturePos::glGenBuffers: vId:%d"), posBufferId);
	}

	// vartex array for polygon
	GLfloat varr[12];
	varr[ 0] = pyl_left; varr[ 1] = pyl_bottom; varr[ 2] = 0.0f;
	varr[ 3] = pyl_right; varr[ 4] = pyl_bottom; varr[ 5] = 0.0f;
	varr[ 6] = pyl_left; varr[ 7] = pyl_top; varr[ 8] = 0.0f;
	varr[ 9] = pyl_right; varr[10] = pyl_top; varr[11] = 0.0f;

	glBindBuffer(GL_ARRAY_BUFFER, posBufferId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(varr), varr, GL_STATIC_DRAW);
	while((err = glGetError()) != GL_NO_ERROR) {
		logging->out_logf(LOG_ERROR, _T("COpenGL3::SetTexturePos::glBufferData: V err:0x%x"), err);
	}


	if (texPosBufferId == 0) {
		glGenBuffers(1, &texPosBufferId);
		while((err = glGetError()) != GL_NO_ERROR) {
			logging->out_logf(LOG_ERROR, _T("COpenGL3::SetTexturePos::glGenBuffers: UV err:0x%x"), err);
		}
		logging->out_debugf(_T("COpenGL3::SetTexturePos::glGenBuffers: uvId:%d"), texPosBufferId);
	}

	// vartex array for texture
	GLfloat uvarr[8];
	uvarr[ 0] = tex_left;  uvarr[ 1] = tex_top;
	uvarr[ 2] = tex_right; uvarr[ 3] = tex_top;
	uvarr[ 4] = tex_left;  uvarr[ 5] = tex_bottom;
	uvarr[ 6] = tex_right; uvarr[ 7] = tex_bottom;

	glBindBuffer(GL_ARRAY_BUFFER, texPosBufferId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(uvarr), uvarr, GL_STATIC_DRAW);
	while((err = glGetError()) != GL_NO_ERROR) {
		logging->out_logf(LOG_ERROR, _T("COpenGL3::SetTexturePos::glBufferData: UV err:0x%x"), err);
	}

	return true;
}

/// release buffer
void COpenGL3::ReleaseBuffer()
{
	if (texPosBufferId) {
		glDeleteBuffers(1, &texPosBufferId);
		texPosBufferId = 0;
	}
	if (posBufferId) {
		glDeleteBuffers(1, &posBufferId);
		posBufferId = 0;
	}
}

/// build (compile and link) shader programs
bool COpenGL3::BuildProgram()
{
	if (!CompileVertex()) {
		return false;
	}
	if (!CompileFragment()) {
		return false;
	}
	if (!LinkProgram()) {
		return false;
	}
	return true;
}

/// release program
void COpenGL3::ReleaseProgram()
{
	if (programId) {
		glDeleteProgram(programId);
		programId = 0;
	}
	if (vertexId) {
		glDeleteShader(vertexId);
		vertexId = 0;
	}
	if (fragmentId) {
		glDeleteShader(fragmentId);
		fragmentId = 0;
	}
}

/// compile a shader program
/// @param[in]  type  GL_VERTEX_SHADER/GL_FRAGMENT_SHADER/GL_GEOMETRY_SHADER
/// @param[out] id    shader id
/// @param[in]  prog  program list
/// @return true/false
bool COpenGL3::CompileShader(GLenum type, GLuint &id, const GLchar *prog)
{
	GLenum err;

	id = glCreateShader(type);
	while((err = glGetError()) != GL_NO_ERROR) {
		logging->out_logf(LOG_ERROR, _T("COpenGL3::Compile::glCreateShader: type:0x%x err:0x%x"), type, err);
	}
	logging->out_debugf(_T("COpenGL3::Compile::glCreateShader: type:0x%x id:%d"), type, id);

	glShaderSource(id, 1, &prog, NULL);
	while((err = glGetError()) != GL_NO_ERROR) {
		logging->out_logf(LOG_ERROR, _T("COpenGL3::Compile::glShaderSource: type:0x%x err:0x%x"), type, err);
	}
	glCompileShader(id);
	while((err = glGetError()) != GL_NO_ERROR) {
		logging->out_logf(LOG_ERROR, _T("COpenGL3::Compile::glCompileShader: type:0x%x err:0x%x"), type, err);
	}
	// check compile error
	GLint result = GL_FALSE;
	int len = 0;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	glGetShaderiv(id, GL_INFO_LOG_LENGTH, &len);
	if (len > 0) {
		GLchar *msg = new GLchar[len + 1];
		if (msg) {
			glGetShaderInfoLog(id, len, NULL, msg);
			logging->out_logf(LOG_ERROR, _T("COpenGL3::CompileError: type:0x%x %s"), type, msg);
			delete [] msg;
		}
	}

	return (result == GL_TRUE);
}

/// compile the vertex shader
bool COpenGL3::CompileVertex()
{
	const GLchar *vertexProg =
"#version 330 core\n"
"layout(location = 0) in vec3 vPos;\n"
"layout(location = 1) in vec2 vUV;\n"
"out vec2 UV;\n"
"void main() {\n"
"  gl_Position = vec4(vPos, 1);\n"
"  UV = vUV;\n"
"}\n";
	return CompileShader(GL_VERTEX_SHADER, vertexId, vertexProg);
}

/// compile the fragment shader
bool COpenGL3::CompileFragment()
{
	const GLchar *fragmentProg =
"#version 330 core\n"
"in vec2 UV;\n"
"out vec4 color;\n"
"uniform sampler2D myTextureSample;\n"
"void main() {\n"
"  color = texture(myTextureSample, UV);\n"
"}\n";
	return CompileShader(GL_FRAGMENT_SHADER, fragmentId, fragmentProg);
}

/// link vertex and fragment shader program
bool COpenGL3::LinkProgram()
{
	GLenum err;

	programId = glCreateProgram();
	while((err = glGetError()) != GL_NO_ERROR) {
		logging->out_logf(LOG_ERROR, _T("COpenGL3::Link::glCreateProgram: err:0x%x"), err);
	}
	logging->out_debugf(_T("COpenGL3::Link::glCreateProgram: id:%d"), programId);

	glAttachShader(programId, vertexId);
	glAttachShader(programId, fragmentId);
	while((err = glGetError()) != GL_NO_ERROR) {
		logging->out_logf(LOG_ERROR, _T("COpenGL3::Link::glAttachShader: err:0x%x"), err);
	}

	glLinkProgram(programId);
	while((err = glGetError()) != GL_NO_ERROR) {
		logging->out_logf(LOG_ERROR, _T("COpenGL3::Link::glLinkProgram: err:0x%x"), err);
	}
	// check compile error
	GLint result = GL_FALSE;
	int len = 0;
	glGetProgramiv(programId, GL_LINK_STATUS, &result);
	glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &len);
	if (len > 0) {
		GLchar *msg = new GLchar[len + 1];
		if (msg) {
			glGetProgramInfoLog(programId, len, NULL, msg);
			logging->out_logf(LOG_ERROR, _T("COpenGL3::LinkError: %s"), msg);
			delete [] msg;
		}
	}
	glDetachShader(programId, vertexId);
	glDetachShader(programId, fragmentId);

	glDeleteShader(vertexId);
	vertexId = 0;
	glDeleteShader(fragmentId);
	fragmentId = 0;

	while((err = glGetError()) != GL_NO_ERROR) {
		logging->out_logf(LOG_ERROR, _T("COpenGL3::Link: err:0x%x"), err);
	}

	return (result == GL_TRUE);
}

void COpenGL3::UseProgram()
{
	glUseProgram(programId);
}

/// draw texture on the window
void COpenGL3::Draw(CSurface *surface)
{
	Draw(surface->Width(), surface->Height(), surface->GetBuffer());
}
void COpenGL3::Draw(int width, int height, void *buffer)
{
	GLenum err;

	UseProgram();

	glClear(GL_COLOR_BUFFER_BIT);

	glActiveTexture(GL_TEXTURE0);

	// draw texture using screen pixel buffer
	glBindTexture(GL_TEXTURE_2D, textureId);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
			GL_UNSIGNED_BYTE, (GLvoid *)buffer);
	while ((err = glGetError()) != GL_NO_ERROR) {
		logging->out_logf(LOG_ERROR, _T("COpenGL3::Draw::glTexImage2D: 0x%x"), err);
	}

	// bind texture0 to texture in program
	glUniform1i(textureLoc, 0);

	// vertex
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, posBufferId);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);

	// texture
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, texPosBufferId);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void *)0);

	// draw vertex
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);
}
void COpenGL3::ClearScreen()
{
	// fill black
	glBegin(GL_QUADS);
	glColor3ub(0, 0, 0);
	glVertex3i(-1, -1, 0);
	glVertex3i(-1,  1, 0);
	glVertex3i( 1,  1, 0);
	glVertex3i( 1, -1, 0);
	glEnd();
}
#endif /* USE_OPENGL33 */

#endif /* USE_OPENGL */
