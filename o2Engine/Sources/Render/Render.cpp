#include "Render/Render.h"

#include "Application/Application.h"
#include "Assets/Assets.h"
#include "Render/Font.h"
#include "Render/Mesh.h"
#include "Render/Sprite.h"
#include "Render/Texture.h"
#include "Utils/Debug.h"
#include "Utils/Log/LogStream.h"
#include "Utils/Math/Interpolation.h"

namespace o2
{
	DECLARE_SINGLETON(Render);
    
    void Render::Initialize()
    {
        mVertexBufferSize = USHRT_MAX;
        mIndexBufferSize = USHRT_MAX;
        
        InitializeProperties();
        
        // Create log stream
        mLog = mnew LogStream("Render");
        o2Debug.GetLog()->BindStream(mLog);
        
        // Initialize OpenGL
        mLog->Out("Initializing OpenGL render..");
        
        mResolution = o2Application.GetContentSize();
        
        // Check compatibles
        CheckCompatibles();
        
        // Initialize buffers
        mVertexData = new UInt8[mVertexBufferSize*sizeof(Vertex2)];
        
        mVertexIndexData = new UInt16[mIndexBufferSize];
        mLastDrawVertex = 0;
        mTrianglesCount = 0;
        mCurrentPrimitiveType = GL_TRIANGLES;
        
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                
        glLineWidth(1.0f);
        
        GL_CHECK_ERROR(mLog);
        
        mLog->Out("GL_VENDOR: " + (String)(char*)glGetString(GL_VENDOR));
        mLog->Out("GL_RENDERER: " + (String)(char*)glGetString(GL_RENDERER));
        mLog->Out("GL_VERSION: " + (String)(char*)glGetString(GL_VERSION));
        
        InitializeFreeType();
        
        mCurrentRenderTarget = TextureRef();
        
        if (IS_DEV_MODE)
            o2Assets.onAssetsRebuilded += Function<void(const Vector<UID>&)>(this, &Render::OnAssetsRebuilded);
        
        GL_CHECK_ERROR(mLog);
        
        InitializeStdShader();
        
        mReady = true;
    }
    
    void Render::InitializeStdShader()
    {
        const char* fragShader = "                                              \n \
        #if __VERSION__ >= 140                                             \n \
        in vec2      varTexcoord;                                          \n \
        out vec4     fragColor;                                            \n \
        #else                                                              \n \
        varying vec2 varTexcoord;                                          \n \
        #endif                                                             \n \
                                                                           \n \
        uniform sampler2D diffuseTexture;                                  \n \
                                                                           \n \
                                                                           \n \
        void main (void)                                                   \n \
        {                                                                  \n \
        #if __VERSION__ >= 140                                             \n \
            fragColor = texture(diffuseTexture, varTexcoord.st, 0.0);      \n \
        #else                                                              \n \
            gl_FragColor = texture2D(diffuseTexture, varTexcoord.st, 0.0); \n \
        #endif                                                             \n \
        }";
        
        const char* vtxShader = "                                      \n \
        uniform mat4 modelViewProjectionMatrix;                   \n \
                                                                  \n \
        #if __VERSION__ >= 140                                    \n \
        in vec4  inPosition;                                      \n \
        in vec2  inTexcoord;                                      \n \
        out vec2 varTexcoord;                                     \n \
        #else                                                     \n \
        attribute vec4 inPosition;                                \n \
        attribute vec2 inTexcoord;                                \n \
        varying vec2 varTexcoord;                                 \n \
        #endif                                                    \n \
                                                                  \n \
        void main (void)                                          \n \
        {                                                         \n \
            gl_Position	= modelViewProjectionMatrix * inPosition; \n \
            varTexcoord = inTexcoord;                             \n \
        }                                                         \n \
        ";
        
        mStdShader = BuildShaderProgram(vtxShader, fragShader);
        mStdShaderMvpUniformIdx = glGetUniformLocation(mStdShader, "modelViewProjectionMatrix");
    }

	void Render::OnFrameResized()
	{
		mResolution = o2Application.GetContentSize();
	}

	void Render::InitializeFreeType()
	{
		FT_Error error = FT_Init_FreeType(&mFreeTypeLib);
		if (error)
			mLog->Out("Failed to initialize FreeType: %i", error);
	}

	void Render::DeinitializeFreeType()
	{
		FT_Done_FreeType(mFreeTypeLib);
	}
    
    void Render::Deinitialize()
    {
        if (!mReady)
            return;
        
        if (IS_DEV_MODE)
            o2Assets.onAssetsRebuilded -= Function<void(const Vector<UID>&)>(this, &Render::OnAssetsRebuilded);
        
        auto fonts = mFonts;
        for (auto font : fonts)
            delete font;
        
        auto textures = mTextures;
        for (auto texture : textures)
            delete texture;
        
        DeinitializeFreeType();
        
        mReady = false;
    }

	void Render::CheckCompatibles()
	{
		//check render targets available
		const char* extensions[] = { "GL_ARB_framebuffer_object", "GL_EXT_framebuffer_object", "GL_EXT_framebuffer_blit",
			"GL_EXT_packed_depth_stencil" };

		mRenderTargetsAvailable = true;
		for (int i = 0; i < 4; i++)
		{
			if (!IsGLExtensionSupported(extensions[i]))
				mRenderTargetsAvailable = false;
		}

		//get max texture size
		glGetIntegerv(GL_MAX_TEXTURE_SIZE, &mMaxTextureSize.x);
		mMaxTextureSize.y = mMaxTextureSize.x;
	}

	void Render::Begin()
	{
		if (!mReady)
			return;

		// Reset batching params
		mLastDrawTexture = NULL;
		mLastDrawVertex = 0;
		mLastDrawIdx = 0;
		mTrianglesCount = 0;
		mFrameTrianglesCount = 0;
		mDIPCount = 0;
		mCurrentPrimitiveType = GL_TRIANGLES;

		mDrawingDepth = 0.0f;

		mScissorInfos.Clear();
		mStackScissors.Clear();

		mClippingEverything = false;
        
        glUseProgram(mStdShader);

		// Reset view matrices
		SetupViewMatrix(mResolution);

		UpdateCameraTransforms();
        
        GL_CHECK_ERROR(mLog);
	}

	void Render::DrawPrimitives()
	{
		if (mLastDrawVertex < 1)
			return;

		glDrawElements(mCurrentPrimitiveType, mLastDrawIdx, GL_UNSIGNED_SHORT, mVertexIndexData);

		GL_CHECK_ERROR(mLog);

		mFrameTrianglesCount += mTrianglesCount;
		mLastDrawVertex = mTrianglesCount = mLastDrawIdx = 0;

		mDIPCount++;
	}

	void Render::SetupViewMatrix(const Vec2I& viewSize)
	{
//		mCurrentResolution = viewSize;
//		float projMat[16];
//		Math::OrthoProjMatrix(projMat, 0.0f, (float)viewSize.x, (float)viewSize.y, 0.0f, 0.0f, 10.0f);
//		//glMatrixMode(GL_PROJECTION);
//		//glLoadIdentity();
//		glViewport(0, 0, viewSize.x, viewSize.y);
//		//glLoadMatrixf(projMat);
        
		UpdateCameraTransforms();
	}

	void Render::End()
	{
		if (!mReady)
			return;

		DrawPrimitives();
        
#ifdef WINDOWS
		SwapBuffers(mHDC);
#endif

		GL_CHECK_ERROR(mLog);

		CheckTexturesUnloading();
		CheckFontsUnloading();
	}

	void Render::Clear(const Color4& color /*= Color4::Blur()*/)
	{
		glClearColor(color.RF(), color.GF(), color.BF(), color.AF());
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		GL_CHECK_ERROR(mLog);
	}

	Vec2I Render::GetResolution() const
	{
		return mResolution;
	}

	Vec2I Render::GetCurrentResolution() const
	{
		return mCurrentResolution;
	}

	Vec2I Render::GetDPI() const
	{
		return mDPI;
	}

	int Render::GetDrawCallsCount()
	{
		return mDIPCount;
	}

	void Render::SetCamera(const Camera& camera)
	{
		mCamera = camera;
		UpdateCameraTransforms();
	}

	Camera Render::GetCamera() const
	{
		return mCamera;
	}
    
    void mtxMultiply(float* ret, const float* lhs, const float* rhs)
    {
        // [ 0 4  8 12 ]   [ 0 4  8 12 ]
        // [ 1 5  9 13 ] x [ 1 5  9 13 ]
        // [ 2 6 10 14 ]   [ 2 6 10 14 ]
        // [ 3 7 11 15 ]   [ 3 7 11 15 ]
        ret[ 0] = lhs[ 0]*rhs[ 0] + lhs[ 4]*rhs[ 1] + lhs[ 8]*rhs[ 2] + lhs[12]*rhs[ 3];
        ret[ 1] = lhs[ 1]*rhs[ 0] + lhs[ 5]*rhs[ 1] + lhs[ 9]*rhs[ 2] + lhs[13]*rhs[ 3];
        ret[ 2] = lhs[ 2]*rhs[ 0] + lhs[ 6]*rhs[ 1] + lhs[10]*rhs[ 2] + lhs[14]*rhs[ 3];
        ret[ 3] = lhs[ 3]*rhs[ 0] + lhs[ 7]*rhs[ 1] + lhs[11]*rhs[ 2] + lhs[15]*rhs[ 3];
        
        ret[ 4] = lhs[ 0]*rhs[ 4] + lhs[ 4]*rhs[ 5] + lhs[ 8]*rhs[ 6] + lhs[12]*rhs[ 7];
        ret[ 5] = lhs[ 1]*rhs[ 4] + lhs[ 5]*rhs[ 5] + lhs[ 9]*rhs[ 6] + lhs[13]*rhs[ 7];
        ret[ 6] = lhs[ 2]*rhs[ 4] + lhs[ 6]*rhs[ 5] + lhs[10]*rhs[ 6] + lhs[14]*rhs[ 7];
        ret[ 7] = lhs[ 3]*rhs[ 4] + lhs[ 7]*rhs[ 5] + lhs[11]*rhs[ 6] + lhs[15]*rhs[ 7];
        
        ret[ 8] = lhs[ 0]*rhs[ 8] + lhs[ 4]*rhs[ 9] + lhs[ 8]*rhs[10] + lhs[12]*rhs[11];
        ret[ 9] = lhs[ 1]*rhs[ 8] + lhs[ 5]*rhs[ 9] + lhs[ 9]*rhs[10] + lhs[13]*rhs[11];
        ret[10] = lhs[ 2]*rhs[ 8] + lhs[ 6]*rhs[ 9] + lhs[10]*rhs[10] + lhs[14]*rhs[11];
        ret[11] = lhs[ 3]*rhs[ 8] + lhs[ 7]*rhs[ 9] + lhs[11]*rhs[10] + lhs[15]*rhs[11];
        
        ret[12] = lhs[ 0]*rhs[12] + lhs[ 4]*rhs[13] + lhs[ 8]*rhs[14] + lhs[12]*rhs[15];
        ret[13] = lhs[ 1]*rhs[12] + lhs[ 5]*rhs[13] + lhs[ 9]*rhs[14] + lhs[13]*rhs[15];
        ret[14] = lhs[ 2]*rhs[12] + lhs[ 6]*rhs[13] + lhs[10]*rhs[14] + lhs[14]*rhs[15];
        ret[15] = lhs[ 3]*rhs[12] + lhs[ 7]*rhs[13] + lhs[11]*rhs[14] + lhs[15]*rhs[15];
    }

	void Render::UpdateCameraTransforms()
	{
		DrawPrimitives();

		Vec2F resf = (Vec2F)mCurrentResolution;
        
        mCurrentResolution = resf;
        float projMat[16];
        Math::OrthoProjMatrix(projMat, 0.0f, (float)resf.x, (float)resf.y, 0.0f, 0.0f, 10.0f);
        glViewport(0, 0, resf.x, resf.y);

		float modelMatrix[16] =
		{
			1,           0,            0, 0,
			0,          -1,            0, 0,
			0,           0,            1, 0,
			Math::Round(resf.x*0.5f), Math::Round(resf.y*0.5f), -1, 1
		};

		Basis defaultCameraBasis((Vec2F)mCurrentResolution*-0.5f, Vec2F::Right()*resf.x, Vec2F().Up()*resf.y);
		Basis camTransf = mCamera.GetBasis().Inverted()*defaultCameraBasis;

		float camTransfMatr[16] =
		{
			camTransf.xv.x,   camTransf.xv.y,   0, 0,
			camTransf.yv.x,   camTransf.yv.y,   0, 0,
			0,                0,                0, 0,
			camTransf.offs.x, camTransf.offs.y, 0, 1
		};

        float mvp[16];
        float pp[16];
        mtxMultiply(pp, modelMatrix, camTransfMatr);
        mtxMultiply(mvp, projMat, pp);
        
        glUniformMatrix4fv(mStdShaderMvpUniformIdx, 1, GL_FALSE, mvp);
        
        GL_CHECK_ERROR(mLog);
	}

	void Render::CheckTexturesUnloading()
	{
		TexturesVec unloadTextures;
		for (auto texture : mTextures)
			if (texture->mRefs.Count() == 0)
				unloadTextures.Add(texture);

		unloadTextures.ForEach([](auto texture) { delete texture; });
	}

	void Render::CheckFontsUnloading()
	{
		FontsVec unloadFonts;
		for (auto font : mFonts)
			if (font->mRefs.Count() == 0)
				unloadFonts.Add(font);

		unloadFonts.ForEach([](auto fnt) { delete fnt; });
	}

	void Render::OnAssetsRebuilded(const Vector<UID>& changedAssets)
	{
		for (auto tex : mTextures)
		{
			if (changedAssets.Contains(tex->GetAtlasAssetId()))
				tex->Reload();
		}

		for (auto spr : mSprites)
		{
			if (changedAssets.Contains(spr->GetAtlasAssetId()))
				spr->ReloadImage();
		}
	}

	void Render::DrawLine(const Vec2F& a, const Vec2F& b, const Color4& color /*= Color4::White()*/)
	{
		ULong dcolor = color.ABGR();
		Vertex2 v[] = { Vertex2(a.x, a.y, dcolor, 0, 0), Vertex2(b.x, b.y, dcolor, 0, 0) };
		DrawLines(v, 1);
	}

	void Render::DrawLine(const Vector<Vec2F>& points, const Color4& color /*= Color4::White()*/)
	{
		ULong dcolor = color.ABGR();
		int segCount = points.Count() - 1;
		Vertex2* v = new Vertex2[segCount * 2];
		for (int i = 0; i < segCount; i++)
		{
			v[i * 2] = Vertex2(points[i], dcolor, 0, 0);
			v[i * 2 + 1] = Vertex2(points[i + 1], dcolor, 0, 0);
		}
		DrawLines(v, segCount);
		delete[] v;
	}

	void Render::DrawArrow(const Vec2F& a, const Vec2F& b, const Color4& color /*= Color4::White()*/,
						   const Vec2F& arrowSize /*= Vec2F(10, 10)*/)
	{
		ULong dcolor = color.ABGR();
		Vec2F dir = (b - a).Normalized();
		Vec2F ndir = dir.Perpendicular();

		Vertex2 v[] = {
			Vertex2(a, dcolor, 0, 0), Vertex2(b, dcolor, 0, 0),
			Vertex2(b - dir*arrowSize.x + ndir*arrowSize.y, dcolor, 0, 0), Vertex2(b, dcolor, 0, 0),
			Vertex2(b - dir*arrowSize.x - ndir*arrowSize.y, dcolor, 0, 0), Vertex2(b, dcolor, 0, 0) };
		DrawLines(v, 3);
	}

	void Render::DrawRectFrame(const Vec2F& minp, const Vec2F& maxp, const Color4& color /*= Color4::White()*/)
	{
		ULong dcolor = color.ABGR();
		Vertex2 v[] = {
			Vertex2(minp.x, minp.y, dcolor, 0, 0), Vertex2(maxp.x, minp.y, dcolor, 0, 0),
			Vertex2(maxp.x, minp.y, dcolor, 0, 0), Vertex2(maxp.x, maxp.y, dcolor, 0, 0),
			Vertex2(maxp.x, maxp.y, dcolor, 0, 0), Vertex2(minp.x, maxp.y, dcolor, 0, 0),
			Vertex2(minp.x, maxp.y, dcolor, 0, 0), Vertex2(minp.x, minp.y, dcolor, 0, 0)
		};
		DrawLines(v, 4);
	}

	void Render::DrawRectFrame(const RectF& rect, const Color4& color /*= Color4::White()*/)
	{
		DrawRectFrame(rect.LeftBottom(), rect.RightTop(), color);
	}

	void Render::DrawBasis(const Basis& basis, const Color4& xcolor /*= Color4::Red()*/,
						   const Color4& ycolor /*= Color4::Blue()*/, const Color4& color /*= Color4::White()*/)
	{
		Vertex2 v[] =
		{
			Vertex2(basis.offs, xcolor.ABGR(), 0, 0), Vertex2(basis.offs + basis.xv, xcolor.ABGR(), 0, 0),
			Vertex2(basis.offs, ycolor.ABGR(), 0, 0), Vertex2(basis.offs + basis.yv, ycolor.ABGR(), 0, 0),
			Vertex2(basis.offs + basis.xv, color.ABGR(), 0, 0), Vertex2(basis.offs + basis.yv + basis.xv, color.ABGR(), 0, 0),
			Vertex2(basis.offs + basis.yv, color.ABGR(), 0, 0), Vertex2(basis.offs + basis.yv + basis.xv, color.ABGR(), 0, 0)
		};
		DrawLines(v, 4);
	}

	void Render::DrawCross(const Vec2F& pos, float size /*= 5*/, const Color4& color /*= Color4::White()*/)
	{
		ULong dcolor = color.ABGR();
		Vertex2 v[] = {
			Vertex2(pos.x - size, pos.y, dcolor, 0, 0), Vertex2(pos.x + size, pos.y, dcolor, 0, 0),
			Vertex2(pos.x, pos.y - size, dcolor, 0, 0), Vertex2(pos.x, pos.y + size, dcolor, 0, 0) };
		DrawLines(v, 2);
	}

	void Render::DrawCircle(const Vec2F& pos, float radius /*= 5*/, const Color4& color /*= Color4::White()*/,
							int segCount /*= 20*/)
	{
		Vertex2* v = new Vertex2[segCount * 2];
		ULong dcolor = color.ABGR();

		float angleSeg = 2.0f*Math::PI() / (float)(segCount - 1);
		for (int i = 0; i < segCount; i++)
		{
			float a = (float)i*angleSeg;
			v[i * 2] = Vertex2(Vec2F::Rotated(a)*radius + pos, dcolor, 0, 0);
			v[i * 2 + 1] = Vertex2(Vec2F::Rotated(a + angleSeg)*radius + pos, dcolor, 0, 0);
		}

		DrawLines(v, segCount);
		delete[] v;
	}

	void Render::DrawBezierCurve(const Vec2F& p1, const Vec2F& p2, const Vec2F& p3, const Vec2F& p4,
								 const Color4& color /*= Color4::White()*/)
	{
		const int segCount = 20;
		Vertex2 v[segCount * 2];
		ULong dcolor = color.ABGR();

		Vec2F lastp = p1;
		for (int i = 0; i < segCount; i++)
		{
			float coef = (float)(i + 1) / (float)segCount;
			Vec2F p = Bezier(p1, p2, p3, p4, coef);

			v[i * 2] = Vertex2(lastp, dcolor, 0, 0);
			v[i * 2 + 1] = Vertex2(p, dcolor, 0, 0);

			lastp = p;
		}

		DrawLines(v, segCount);
	}

	void Render::DrawBezierCurveArrow(const Vec2F& p1, const Vec2F& p2, const Vec2F& p3, const Vec2F& p4,
									  const Color4& color /*= Color4::White()*/, const Vec2F& arrowSize /*= Vec2F(10, 10)*/)
	{
		const int segCount = 20;
		Vertex2 v[segCount * 2 + 4];
		ULong dcolor = color.ABGR();

		Vec2F lastp = p1;
		Vec2F dir;
		for (int i = 0; i < segCount; i++)
		{
			float coef = (float)(i + 1) / (float)segCount;
			Vec2F p = Bezier(p1, p2, p3, p4, coef);

			v[i * 2] = Vertex2(lastp, dcolor, 0, 0);
			v[i * 2 + 1] = Vertex2(p, dcolor, 0, 0);

			dir = p - lastp;
			lastp = p;
		}

		dir.Normalize();
		Vec2F ndir = dir.Perpendicular();

		v[segCount * 2 + 0] = Vertex2(p4, dcolor, 0, 0);
		v[segCount * 2 + 1] = Vertex2(p4 - dir*arrowSize.x + ndir*arrowSize.y, dcolor, 0, 0);
		v[segCount * 2 + 2] = Vertex2(p4, dcolor, 0, 0);
		v[segCount * 2 + 3] = Vertex2(p4 - dir*arrowSize.x - ndir*arrowSize.y, dcolor, 0, 0);

		DrawLines(v, segCount + 2);
	}

	void Render::BeginRenderToStencilBuffer()
	{
		if (mStencilDrawing || mStencilTest)
			return;

		DrawPrimitives();

		glEnable(GL_STENCIL_TEST);
		glStencilFunc(GL_ALWAYS, 0x1, 0xffffffff);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

		GL_CHECK_ERROR(mLog);

		mStencilDrawing = true;
	}

	void Render::EndRenderToStencilBuffer()
	{
		if (!mStencilDrawing)
			return;

		DrawPrimitives();

		glDisable(GL_STENCIL_TEST);
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

		GL_CHECK_ERROR(mLog);

		mStencilDrawing = false;
	}

	void Render::EnableStencilTest()
	{
		if (mStencilTest || mStencilDrawing)
			return;

		DrawPrimitives();

		glEnable(GL_STENCIL_TEST);
		glStencilFunc(GL_EQUAL, 0x1, 0xffffffff);

		GL_CHECK_ERROR(mLog);

		mStencilTest = true;
	}

	void Render::DisableStencilTest()
	{
		if (!mStencilTest)
			return;

		DrawPrimitives();

		glDisable(GL_STENCIL_TEST);

		mStencilTest = false;
	}

	bool Render::IsStencilTestEnabled() const
	{
		return mStencilTest;
	}

	void Render::ClearStencil()
	{
		glClearStencil(0);
		glClear(GL_STENCIL_BUFFER_BIT);

		GL_CHECK_ERROR(mLog);
	}

	RectI Render::GetScissorRect() const
	{
		if (mStackScissors.IsEmpty())
			return RectI(-(int)(mCurrentResolution.x*0.5f), -(int)(mCurrentResolution.y*0.5f),
						 (int)(mCurrentResolution.x*0.5f), (int)(mCurrentResolution.y*0.5f));

		return (RectI)(mStackScissors.Last().mScrissorRect);
	}

	RectI Render::GetResScissorRect() const
	{
		if (mStackScissors.IsEmpty())
			return RectI(-(int)(mCurrentResolution.x*0.5f), -(int)(mCurrentResolution.y*0.5f),
						 (int)(mCurrentResolution.x*0.5f), (int)(mCurrentResolution.y*0.5f));

		return (RectI)(mStackScissors.Last().mSummaryScissorRect);
	}

	const Render::StackScissorVec& Render::GetScissorsStack() const
	{
		return mStackScissors;
	}

	void Render::EnableScissorTest(const RectI& rect)
	{
		DrawPrimitives();

		RectI summaryScissorRect = rect;
		if (!mStackScissors.IsEmpty())
		{
			RectI lastSummaryClipRect = mStackScissors.Last().mSummaryScissorRect;
			mClippingEverything = !summaryScissorRect.IsIntersects(lastSummaryClipRect);
			summaryScissorRect = summaryScissorRect.GetIntersection(lastSummaryClipRect);
			mScissorInfos.Last().mEndDepth = mDrawingDepth;
		}
		else
		{
			glEnable(GL_SCISSOR_TEST);
			GL_CHECK_ERROR(mLog);
		}

		mScissorInfos.Add(ScissorInfo(summaryScissorRect, mDrawingDepth));
		mStackScissors.Add(ScissorStackItem(rect, summaryScissorRect));

		glScissor((int)(summaryScissorRect.left + mCurrentResolution.x*0.5f), 
				  (int)(summaryScissorRect.bottom + mCurrentResolution.y*0.5f),
				  (int)summaryScissorRect.Width(), 
				  (int)summaryScissorRect.Height());
	}

	void Render::DisableScissorTest(bool forcible /*= false*/)
	{
		if (mStackScissors.IsEmpty())
		{
			mLog->WarningStr("Can't disable scissor test - no scissor were enabled!");
			return;
		}

		DrawPrimitives();

		if (forcible)
		{
			glDisable(GL_SCISSOR_TEST);
			GL_CHECK_ERROR(mLog);

			while (!mStackScissors.IsEmpty() && !mStackScissors.Last().mRenderTarget)
				mStackScissors.PopBack();

			mScissorInfos.Last().mEndDepth = mDrawingDepth;
		}
		else
		{
			if (mStackScissors.Count() == 1)
			{
				glDisable(GL_SCISSOR_TEST);
				GL_CHECK_ERROR(mLog);
				mStackScissors.PopBack();

				mScissorInfos.Last().mEndDepth = mDrawingDepth;
				mClippingEverything = false;
			}
			else
			{
				mStackScissors.PopBack();
				RectI lastClipRect = mStackScissors.Last().mSummaryScissorRect;
				glScissor((int)(lastClipRect.left + mCurrentResolution.x*0.5f),
						  (int)(lastClipRect.bottom + mCurrentResolution.y*0.5f),
						  (int)lastClipRect.Width(), 
						  (int)lastClipRect.Height());

				mScissorInfos.Last().mEndDepth = mDrawingDepth;
				mScissorInfos.Add(ScissorInfo(lastClipRect, mDrawingDepth));

				mClippingEverything = lastClipRect == RectI();
			}
		}
	}

	bool Render::IsScissorTestEnabled() const
	{
		return !mStackScissors.IsEmpty();
	}

	bool Render::DrawMesh(Mesh* mesh)
	{
		if (!mReady)
			return false;

		mDrawingDepth += 1.0f;

		if (mClippingEverything)
			return true;

		// Check difference
		if (mLastDrawTexture != mesh->mTexture.mTexture ||
			mLastDrawVertex + mesh->vertexCount >= mVertexBufferSize ||
			mLastDrawIdx + mesh->polyCount * 3 >= mIndexBufferSize ||
			mCurrentPrimitiveType == GL_LINES)
		{
			DrawPrimitives();

			mLastDrawTexture = mesh->mTexture.mTexture;
			mCurrentPrimitiveType = GL_TRIANGLES;

			if (mLastDrawTexture)
			{
				glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, mLastDrawTexture->mHandle);

				GL_CHECK_ERROR(mLog);
			}
			else
			{
				glDisable(GL_TEXTURE_2D);
			}
		}

		// Copy data
		memcpy(&mVertexData[mLastDrawVertex*sizeof(Vertex2)], mesh->vertices, sizeof(Vertex2)*mesh->vertexCount);

		for (UInt i = mLastDrawIdx, j = 0; j < mesh->polyCount * 3; i++, j++)
		{
			mVertexIndexData[i] = mLastDrawVertex + mesh->indexes[j];
		}

		mTrianglesCount += mesh->polyCount;
		mLastDrawVertex += mesh->vertexCount;
		mLastDrawIdx += mesh->polyCount * 3;

		return true;
	}

	bool Render::DrawMeshWire(Mesh* mesh, const Color4& color /*= Color4::White()*/)
	{
		Vertex2* vertices = new Vertex2[mesh->polyCount * 6];
		auto dcolor = color.ABGR();

		for (UInt i = 0; i < mesh->polyCount; i++)
		{
			vertices[i * 6] = mesh->vertices[mesh->indexes[i * 3]];
			vertices[i * 6 + 1] = mesh->vertices[mesh->indexes[i * 3 + 1]];
			vertices[i * 6 + 2] = mesh->vertices[mesh->indexes[i * 3 + 1]];
			vertices[i * 6 + 3] = mesh->vertices[mesh->indexes[i * 3 + 2]];
			vertices[i * 6 + 4] = mesh->vertices[mesh->indexes[i * 3 + 2]];
			vertices[i * 6 + 5] = mesh->vertices[mesh->indexes[i * 3]];
		}

		for (UInt i = 0; i < mesh->polyCount * 6; i++)
			vertices[i].color = dcolor;

		bool res = DrawLines(vertices, mesh->polyCount * 3);
		delete[] vertices;

		return res;
	}

	bool Render::DrawLines(Vertex2* verticies, int count)
	{
		if (!mReady)
			return false;

		// Check difference
		if (mCurrentPrimitiveType == GL_TRIANGLES ||
			mLastDrawVertex + count * 2 >= mVertexBufferSize ||
			mLastDrawIdx + count * 2 >= mIndexBufferSize)
		{
			DrawPrimitives();

			mLastDrawTexture = NULL;
			mCurrentPrimitiveType = GL_LINES;
			glDisable(GL_TEXTURE_2D);
		}

		// Copy data
		memcpy(&mVertexData[mLastDrawVertex*sizeof(Vertex2)], verticies, sizeof(Vertex2)*count * 2);

		for (UInt i = mLastDrawIdx, j = 0; j < (UInt)count * 2; i++, j++)
		{
			mVertexIndexData[i] = mLastDrawVertex + j;
		}

		mTrianglesCount += count;
		mLastDrawVertex += count * 2;
		mLastDrawIdx += count * 2;

		return true;
	}

	void Render::SetRenderTexture(TextureRef renderTarget)
	{
		if (!renderTarget)
		{
			UnbindRenderTexture();
			return;
		}

		if (renderTarget->mUsage != Texture::Usage::RenderTarget)
		{
			mLog->Error("Can't set texture as render target: not render target texture");
			UnbindRenderTexture();
			return;
		}

		if (!renderTarget->IsReady())
		{
			mLog->Error("Can't set texture as render target: texture isn't ready");
			UnbindRenderTexture();
			return;
		}

		DrawPrimitives();

		if (!mStackScissors.IsEmpty())
		{
			mScissorInfos.Last().mEndDepth = mDrawingDepth;
			glDisable(GL_SCISSOR_TEST);
			GL_CHECK_ERROR(mLog);
		}

		mStackScissors.Add(ScissorStackItem(RectI(), RectI(), true));

		glBindFramebuffer(GL_FRAMEBUFFER, renderTarget->mFrameBuffer);
		GL_CHECK_ERROR(mLog);

		SetupViewMatrix(renderTarget->GetSize());

		mCurrentRenderTarget = renderTarget;
	}

	void Render::UnbindRenderTexture()
	{
		if (!mCurrentRenderTarget)
			return;

		DrawPrimitives();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		GL_CHECK_ERROR(mLog);

		SetupViewMatrix(mResolution);

		mCurrentRenderTarget = TextureRef();

		DisableScissorTest(true);
		mStackScissors.PopBack();
		if (!mStackScissors.IsEmpty())
		{
			glEnable(GL_SCISSOR_TEST);
			GL_CHECK_ERROR(mLog);

			auto clipRect = mStackScissors.Last().mSummaryScissorRect;

			glScissor((int)(clipRect.left + mCurrentResolution.x*0.5f), 
					  (int)(clipRect.bottom + mCurrentResolution.y*0.5f),
					  (int)clipRect.Width(), 
					  (int)clipRect.Height());
		}
	}
    

    GLuint Render::BuildShaderProgram(const char* vertexSource, const char* fragmentSource)
    {
        GLuint prgName;
        GLint logLength, status;
        GLchar* sourceString = NULL;
        
        float  glLanguageVersion;
        sscanf((char *)glGetString(GL_SHADING_LANGUAGE_VERSION), "%f", &glLanguageVersion);
        
        GLuint version = 100 * glLanguageVersion;
        const GLsizei versionStringSize = sizeof("#version 123\n");
        
        prgName = glCreateProgram();
        
        glBindAttribLocation(prgName, 0, "inPosition");
        glBindAttribLocation(prgName, 1, "inTexcoord");
        
        sourceString = (GLchar*)malloc(strlen(vertexSource) + versionStringSize);
        
        sprintf(sourceString, "#version %d\n%s", version, vertexSource);
        
        GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, (const GLchar **)&(sourceString), NULL);
        glCompileShader(vertexShader);
        glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &logLength);
        
        if (logLength > 0)
        {
            GLchar *log = (GLchar*) malloc(logLength);
            glGetShaderInfoLog(vertexShader, logLength, &logLength, log);
            mLog->Out((String)"Vtx Shader compile log:\n" + log);
            free(log);
        }
        
        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &status);
        if (status == 0)
        {
            mLog->Error((String)"Failed to compile vtx shader:\n" + sourceString);
            return 0;
        }
        
        free(sourceString);
        sourceString = NULL;
        
        glAttachShader(prgName, vertexShader);
        glDeleteShader(vertexShader);
        
        // fragment
        sourceString = (GLchar*)malloc(strlen(fragmentSource) + versionStringSize);
        sprintf(sourceString, "#version %d\n%s", version, fragmentSource);
        
        GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragShader, 1, (const GLchar **)&(sourceString), NULL);
        glCompileShader(fragShader);
        glGetShaderiv(fragShader, GL_INFO_LOG_LENGTH, &logLength);
        if (logLength > 0)
        {
            GLchar *log = (GLchar*)malloc(logLength);
            glGetShaderInfoLog(fragShader, logLength, &logLength, log);
            mLog->Out((String)"Frag Shader compile log:\n" + log);
            free(log);
        }
        
        glGetShaderiv(fragShader, GL_COMPILE_STATUS, &status);
        if (status == 0)
        {
            mLog->Error((String)"Failed to compile frag shader:\n" + sourceString);
            return 0;
        }
        
        free(sourceString);
        sourceString = NULL;
        
        glAttachShader(prgName, fragShader);
        glDeleteShader(fragShader);
        
        glLinkProgram(prgName);
        glGetProgramiv(prgName, GL_INFO_LOG_LENGTH, &logLength);
        if (logLength > 0)
        {
            GLchar *log = (GLchar*)malloc(logLength);
            glGetProgramInfoLog(prgName, logLength, &logLength, log);
            mLog->Out((String)"Program link log:\n" + log);
            free(log);
        }
        
        glGetProgramiv(prgName, GL_LINK_STATUS, &status);
        if (status == 0)
        {
            mLog->Error("Failed to link program");
            return 0;
        }
        
        glValidateProgram(prgName);
        
        glGetProgramiv(prgName, GL_VALIDATE_STATUS, &status);
        if (status == 0)
            mLog->Error("Program cannot run with current OpenGL State");
        
        glGetProgramiv(prgName, GL_INFO_LOG_LENGTH, &logLength);
        if (logLength > 0)
        {
            GLchar *log = (GLchar*)malloc(logLength);
            glGetProgramInfoLog(prgName, logLength, &logLength, log);
            mLog->Out((String)"Program validate log:\n" + log);
            free(log);
        }
        
        glUseProgram(prgName);
        
        GLint samplerLoc = glGetUniformLocation(prgName, "diffuseTexture");
        
        GLint unit = 0;
        glUniform1i(samplerLoc, unit);
        
        GL_CHECK_ERROR(mLog);
        
        return prgName;
    }

	TextureRef Render::GetRenderTexture() const
	{
		return mCurrentRenderTarget;
	}

	bool Render::IsRenderTextureAvailable() const
	{
		return mRenderTargetsAvailable;
	}

	Vec2I Render::GetMaxTextureSize() const
	{
		return mMaxTextureSize;
	}

	float Render::GetDrawingDepth()
	{
		mDrawingDepth += 1.0f;
		return mDrawingDepth;
	}

	const Render::ScissorInfosVec& Render::GetScissorInfos() const
	{
		return mScissorInfos;
	}

	void Render::InitializeProperties()
	{
		INITIALIZE_PROPERTY(Render, camera, SetCamera, GetCamera);
		INITIALIZE_PROPERTY(Render, scissorRect, EnableScissorTest, GetScissorRect);
		INITIALIZE_PROPERTY(Render, renderTexture, SetRenderTexture, GetRenderTexture);
		INITIALIZE_GETTER(Render, resolution, GetResolution);
		INITIALIZE_GETTER(Render, renderTextureAvailable, IsRenderTextureAvailable);
		INITIALIZE_GETTER(Render, maxTextureSize, GetMaxTextureSize);
	}

	Render& Render::operator=(const Render& other)
	{
		return *this;
	}

	Render::ScissorInfo::ScissorInfo():
		mBeginDepth(0), mEndDepth(0)
	{}

	Render::ScissorInfo::ScissorInfo(const RectI& rect, float beginDepth) :
		mScissorRect(rect), mBeginDepth(beginDepth), mEndDepth(beginDepth)
	{}

	bool Render::ScissorInfo::operator==(const ScissorInfo& other)
	{
		return Math::Equals(mBeginDepth, other.mBeginDepth) && Math::Equals(mEndDepth, other.mEndDepth) &&
			mScissorRect == other.mScissorRect;
	}

	Render::ScissorStackItem::ScissorStackItem()
	{}

	Render::ScissorStackItem::ScissorStackItem(const RectI& rect, const RectI& summaryRect, bool renderTarget /*= false*/):
		mScrissorRect(rect), mSummaryScissorRect(summaryRect), mRenderTarget(renderTarget)
	{}

	bool Render::ScissorStackItem::operator==(const ScissorStackItem& other)
	{
		return mScrissorRect == other.mScrissorRect;
	}

}
