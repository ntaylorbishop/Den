#include "OpenGLExtensions.hpp"

PFNGLGENBUFFERSPROC					glGenBuffers				= nullptr;
PFNGLBINDBUFFERPROC					glBindBuffer				= nullptr;
PFNGLBUFFERDATAPROC					glBufferData				= nullptr;
PFNGLGENERATEMIPMAPPROC				glGenerateMipmap			= nullptr;
PFNGLDELETEBUFFERSPROC				glDeleteBuffers				= nullptr;
PFNGLACTIVETEXTUREPROC				glActiveTexture				= nullptr;
PFNGLCREATESHADERPROC				glCreateShader				= nullptr;
PFNGLSHADERSOURCEPROC				glShaderSource				= nullptr;
PFNGLCOMPILESHADERPROC				glCompileShader				= nullptr;
PFNGLGETSHADERIVPROC				glGetShaderiv				= nullptr;
PFNGLDELETESHADERPROC				glDeleteShader				= nullptr;
PFNGLCREATEPROGRAMPROC				glCreateProgram				= nullptr;
PFNGLATTACHSHADERPROC				glAttachShader				= nullptr;
PFNGLLINKPROGRAMPROC				glLinkProgram				= nullptr;
PFNGLGETPROGRAMIVPROC				glGetProgramiv				= nullptr;
PFNGLDETACHSHADERPROC				glDetachShader				= nullptr;
PFNGLDELETEPROGRAMPROC				glDeleteProgram				= nullptr;
PFNGLGETSHADERINFOLOGPROC			glGetShaderInfoLog			= nullptr;
PFNGLGETPROGRAMINFOLOGPROC			glGetProgramInfoLog			= nullptr;
PFNGLGENVERTEXARRAYSPROC			glGenVertexArrays			= nullptr;
PFNGLDELETEVERTEXARRAYSPROC			glDeleteVertexArrays		= nullptr;
PFNGLBINDVERTEXARRAYPROC			glBindVertexArray			= nullptr;
PFNGLGETATTRIBLOCATIONPROC			glGetAttribLocation			= nullptr;
PFNGLENABLEVERTEXATTRIBARRAYPROC	glEnableVertexAttribArray	= nullptr;
PFNGLVERTEXATTRIBPOINTERPROC		glVertexAttribPointer		= nullptr;
PFNGLVERTEXATTRIBIPOINTERPROC		glVertexAttribIPointer		= nullptr;
PFNGLUSEPROGRAMPROC					glUseProgram				= nullptr;
PFNGLGETUNIFORMLOCATIONPROC			glGetUniformLocation		= nullptr;
PFNGLUNIFORM1FVPROC					glUniform1fv				= nullptr;
PFNGLUNIFORM2FVPROC					glUniform2fv				= nullptr;
PFNGLUNIFORM3FVPROC					glUniform3fv				= nullptr;
PFNGLUNIFORM4FVPROC					glUniform4fv				= nullptr;
PFNGLUNIFORM1IPROC					glUniform1i					= nullptr;
PFNGLUNIFORM1IVPROC					glUniform1iv				= nullptr;
PFNGLUNIFORM2IVPROC					glUniform2iv				= nullptr;
PFNGLUNIFORM3IVPROC					glUniform3iv				= nullptr;
PFNGLUNIFORM4IVPROC					glUniform4iv				= nullptr;
PFNGLUNIFORM1UIPROC					glUniform1ui				= nullptr;
PFNGLUNIFORM1UIVPROC				glUniform1uiv				= nullptr;
PFNGLUNIFORM2UIVPROC				glUniform2uiv				= nullptr;
PFNGLUNIFORM3UIVPROC				glUniform3uiv				= nullptr;
PFNGLUNIFORM4UIVPROC				glUniform4uiv				= nullptr;
PFNGLBINDSAMPLERPROC				glBindSampler				= nullptr;
PFNGLGENSAMPLERSPROC				glGenSamplers				= nullptr;
PFNGLSAMPLERPARAMETERIPROC			glSamplerParameteri			= nullptr;
PFNGLDELETESAMPLERSPROC				glDeleteSamplers			= nullptr;
PFNGLUNIFORMMATRIX4FVPROC			glUniformMatrix4fv			= nullptr;
PFNGLGENFRAMEBUFFERSPROC			glGenFramebuffers			= nullptr;
PFNGLBINDFRAMEBUFFERPROC			glBindFramebuffer			= nullptr;
PFNGLFRAMEBUFFERTEXTUREPROC			glFramebufferTexture		= nullptr;
PFNGLCHECKFRAMEBUFFERSTATUSPROC		glCheckFramebufferStatus	= nullptr;
PFNGLDRAWBUFFERSPROC				glDrawBuffers				= nullptr;
PFNGLDELETEFRAMEBUFFERSPROC			glDeleteFramebuffers		= nullptr;
PFNGLBLITFRAMEBUFFERPROC			glBlitFramebuffer			= nullptr;
PFNGLGETPROGRAMRESOURCEIVPROC		glGetProgramResourceiv		= nullptr;
PFNGLGETPROGRAMRESOURCENAMEPROC		glGetProgramResourceName	= nullptr;
PFNGLGETACTIVEUNIFORMPROC			glGetActiveUniform			= nullptr;
PFNGLTEXSTORAGE2DPROC				glTexStorage2D				= nullptr;
PFNGLTEXIMAGE2DMULTISAMPLEPROC		glTexImage2DMultisample		= nullptr;
PFNGLFRAMEBUFFERTEXTURE2DPROC		glFramebufferTexture2D		= nullptr;
PFNGLSAMPLERPARAMETERFPROC			glSamplerParameterf			= nullptr;
PFNGLDISPATCHCOMPUTEPROC			glDispatchCompute			= nullptr;
PFNGLDISPATCHCOMPUTEINDIRECTPROC	glDispatchComputeIndirect	= nullptr;
PFNGLBINDIMAGETEXTUREPROC			glBindImageTexture			= nullptr;
PFNGLMEMORYBARRIERPROC				glMemoryBarrier				= nullptr;
PFNGLMAPBUFFERPROC					glMapBuffer					= nullptr;
PFNGLUNMAPBUFFERPROC				glUnmapBuffer				= nullptr;
PFNGLBINDBUFFERBASEPROC				glBindBufferBase			= nullptr;
PFNGLGETINTEGER64VPROC				glGetInteger64v				= nullptr;
PFNGLGENQUERIESPROC					glGenQueries				= nullptr;
PFNGLQUERYCOUNTERPROC				glQueryCounter				= nullptr;
PFNGLGETQUERYOBJECTIVPROC		    glGetQueryObjectiv			= nullptr;
PFNGLGETQUERYOBJECTUI64VPROC		glGetQueryObjectui64v		= nullptr;
PFNGLBUFFERSUBDATAPROC				glBufferSubData				= nullptr;
PFNGLGETDEBUGMESSAGELOGPROC			glGetDebugMessageLog		= nullptr;
PFNGLDEBUGMESSAGECONTROLPROC		glDebugMessageControl		= nullptr;
PFNGLDEBUGMESSAGECALLBACKPROC		glDebugMessageCallback		= nullptr;
PFNGLPUSHDEBUGGROUPPROC				glPushDebugGroup			= nullptr;
PFNGLENABLEIPROC					glEnablei					= nullptr;
PFNGLDISABLEIPROC					glDisablei					= nullptr;
PFNGLBLENDFUNCIPROC					glBlendFunci				= nullptr;
PFNGLCLEARBUFFERFVPROC				glClearBufferfv				= nullptr;
PFNGLBLENDEQUATIONPROC				glBlendEquation				= nullptr;
PFNGLBLENDEQUATIONSEPARATEPROC		glBlendEquationSeparate		= nullptr;