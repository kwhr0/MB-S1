/** @file opengl.h

	Skelton for retropc emulator

	@author Sasaji
	@date   2020.03.08

	@brief [ OpenGL ]
*/

#ifndef OSD_OPENGL_H
#define OSD_OPENGL_H

#if defined(USE_OPENGL)

#if defined(_WIN32)
#include <windows.h>
//# define USE_OPENGL_GLEW
//# define USE_OPENGL21
//# define USE_OPENGL33
# if defined(_MSC_VER) && defined(USE_OPENGL_GLEW)
#  ifdef _DEBUG
#   pragma  comment(lib,"glew32sd.lib")
#  else
#   pragma  comment(lib,"glew32s.lib")
#  endif
# endif
#elif defined(__APPLE__) && defined(__MACH__)
# define USE_OPENGL_GLEXT
# define USE_OPENGL21
# define USE_OPENGL33
#elif defined(linux)
# define USE_OPENGL_GLEXT
# define USE_OPENGL21
# define USE_OPENGL33
#endif

#ifdef USE_OPENGL_GLEXT
# ifndef GL_GLEXT_PROTOTYPES
#  define GL_GLEXT_PROTOTYPES
# endif
#endif

#ifdef USE_OPENGL_GLEW

#define GLEW_STATIC 1
#include <gl/glew.h>

#else

#if defined(__APPLE__) && defined(__MACH__)
# include <OpenGL/gl.h>
#else
# include <GL/gl.h>
#endif

#ifdef USE_OPENGL_GLEXT
# if defined(__APPLE__) && defined(__MACH__)
#  include <OpenGL/glext.h>
# else
#  include <GL/glext.h>
# endif
#endif

#endif /* USE_OPENGL_GLEW */


class CSurface;

/// abstruct class
class COpenGL
{
public:
	COpenGL();
	virtual ~COpenGL();

	static COpenGL *Create(int force = 0);
	virtual int Version() const;

	virtual bool Initialize();
	virtual void Terminate();

	virtual bool InitViewport(int w, int h);

	virtual GLuint CreateTexture(int filter);
	virtual void SetTextureFilter(int filter);
	virtual GLuint ReleaseTexture();

	virtual bool CreateBuffer(GLfloat pyl_left, GLfloat pyl_top, GLfloat pyl_right, GLfloat pyl_bottom, GLfloat tex_left, GLfloat tex_top, GLfloat tex_right, GLfloat tex_bottom);
	virtual void ReleaseBuffer();

	virtual bool SetTexturePos(GLfloat pyl_left, GLfloat pyl_top, GLfloat pyl_right, GLfloat pyl_bottom, GLfloat tex_left, GLfloat tex_top, GLfloat tex_right, GLfloat tex_bottom);

	virtual unsigned char SetInterval(unsigned char use);

	virtual void Draw(CSurface *surface) = 0;
	virtual void Draw(int width, int height, void *buffer) = 0;
	virtual void ClearScreen();
};

///
/// Classic OpenGL 
///
class COpenGL1 : public COpenGL
{
private:
	// texture
	GLuint textureId;

	GLfloat tex_l, tex_r, tex_t, tex_b;
	GLfloat pyl_l, pyl_r, pyl_t, pyl_b;

	int disp_w, disp_h;

public:
	COpenGL1();
	~COpenGL1();

	int Version() const;

	bool Initialize();
	void Terminate();

	bool InitViewport(int w, int h);

	GLuint CreateTexture(int filter);
	void SetTextureFilter(int filter);
	GLuint ReleaseTexture();

	bool CreateBuffer(GLfloat pyl_left, GLfloat pyl_top, GLfloat pyl_right, GLfloat pyl_bottom, GLfloat tex_left, GLfloat tex_top, GLfloat tex_right, GLfloat tex_bottom);

	bool SetTexturePos(GLfloat pyl_left, GLfloat pyl_top, GLfloat pyl_right, GLfloat pyl_bottom, GLfloat tex_left, GLfloat tex_top, GLfloat tex_right, GLfloat tex_bottom);
	void Draw(CSurface *surface);
	void Draw(int width, int height, void *buffer);
	void ClearScreen();
};

#ifdef USE_OPENGL21
///
/// OpenGL 2.1
///
class COpenGL2 : public COpenGL
{
private:
	// vertex buffer
	GLint positionLoc;
	GLuint posBufferId;
	// UV buffer
	GLint texPositionLoc;
	GLuint texPosBufferId;

	// texture
	GLint textureLoc;
	GLuint textureId;

	// for shader program
	GLuint vertexId;
	GLuint fragmentId;
	GLuint programId;
	bool CompileShader(GLenum type, GLuint &id, const GLchar *prog);
	bool CompileVertex();
	bool CompileFragment();
	bool LinkProgram();
	bool BuildProgram();
	void UseProgram();
	void ReleaseProgram();
public:
	COpenGL2();
	~COpenGL2();

	int Version() const;

	bool Initialize();
	void Terminate();

	bool InitViewport(int w, int h);

	GLuint CreateTexture(int filter);
	void SetTextureFilter(int filter);
	GLuint ReleaseTexture();

	bool CreateBuffer(GLfloat pyl_left, GLfloat pyl_top, GLfloat pyl_right, GLfloat pyl_bottom, GLfloat tex_left, GLfloat tex_top, GLfloat tex_right, GLfloat tex_bottom);
	void ReleaseBuffer();

	bool SetTexturePos(GLfloat pyl_left, GLfloat pyl_top, GLfloat pyl_right, GLfloat pyl_bottom, GLfloat tex_left, GLfloat tex_top, GLfloat tex_right, GLfloat tex_bottom);
	void Draw(CSurface *surface);
	void Draw(int width, int height, void *buffer);
	void ClearScreen();
};
#endif /* USE_OPENGL21 */

#ifdef USE_OPENGL33
///
/// OpenGL 3.3 or later
///
class COpenGL3 : public COpenGL
{
private:
	// vertex array
	GLuint vaoId;
	bool CreateVao();
	void ReleaseVao();

	// vertex buffer
	GLuint posBufferId;
	// UV buffer
	GLuint texPosBufferId;

	// texture
	GLint textureLoc;
	GLuint textureId;

	// for shader program
	GLuint vertexId;
	GLuint fragmentId;
	GLuint programId;
	bool CompileShader(GLenum type, GLuint &id, const GLchar *prog);
	bool CompileVertex();
	bool CompileFragment();
	bool LinkProgram();
	bool BuildProgram();
	void UseProgram();
	void ReleaseProgram();
public:
	COpenGL3();
	~COpenGL3();

	int Version() const;

	bool Initialize();
	void Terminate();

	bool InitViewport(int w, int h);

	GLuint CreateTexture(int filter);
	void SetTextureFilter(int filter);
	GLuint ReleaseTexture();

	bool CreateBuffer(GLfloat pyl_left, GLfloat pyl_top, GLfloat pyl_right, GLfloat pyl_bottom, GLfloat tex_left, GLfloat tex_top, GLfloat tex_right, GLfloat tex_bottom);
	void ReleaseBuffer();

	bool SetTexturePos(GLfloat pyl_left, GLfloat pyl_top, GLfloat pyl_right, GLfloat pyl_bottom, GLfloat tex_left, GLfloat tex_top, GLfloat tex_right, GLfloat tex_bottom);
	void Draw(CSurface *surface);
	void Draw(int width, int height, void *buffer);
	void ClearScreen();
};
#endif /* USE_OPENGL33 */

#endif /* USE_OPENGL */

#endif /* OSD_OPENGL_H */
