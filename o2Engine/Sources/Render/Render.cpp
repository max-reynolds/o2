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
        InitializeProperties();
        
        mLog = mnew LogStream("Render");
        o2Debug.GetLog()->BindStream(mLog);
        
        mVertexBufferSize = USHRT_MAX;
        mIndexBufferSize = USHRT_MAX;
        
        mResolution = o2Application.GetContentSize();
        
        mVertexData = new UInt8[mVertexBufferSize*sizeof(Vertex2)];
        mVertexIndexData = new UInt16[mIndexBufferSize];
        
        mLastDrawVertex = 0;
        mTrianglesCount = 0;
        mCurrentPrimitiveType = GL_TRIANGLES;
        
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        glLineWidth(1.0f);
        
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        
        glGenVertexArrays(1, &mVertexArrayObject);
        glBindVertexArray(mVertexArrayObject);
        
        glGenBuffers(1, &mVertexBufferObject);
        glBindBuffer(GL_ARRAY_BUFFER, mVertexBufferObject);
        glBufferData(GL_ARRAY_BUFFER, mVertexBufferSize * sizeof(Vertex2), &mVertexData, GL_STREAM_DRAW);
        
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex2), &((Vertex2*)0)->x);
        
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 1, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex2), &((Vertex2*)0)->color);
        
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2), &((Vertex2*)0)->tu);
        
        glGenBuffers(1, &mIndexBufferObject);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBufferObject);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndexBufferSize * sizeof(unsigned short), &mVertexIndexData, GL_STREAM_DRAW);
        
        InitializeStdShader();
        
        GL_CHECK_ERROR(mLog);
        
        mLog->Out("GL_VENDOR: " + (String)(char*)glGetString(GL_VENDOR));
        mLog->Out("GL_RENDERER: " + (String)(char*)glGetString(GL_RENDERER));
        mLog->Out("GL_VERSION: " + (String)(char*)glGetString(GL_VERSION));
        
        InitializeFreeType();
        
        mCurrentRenderTarget = TextureRef();
        
        if (IS_DEV_MODE)
            o2Assets.onAssetsRebuilded += Function<void(const Vector<UID>&)>(this, &Render::OnAssetsRebuilded);
        
        GL_CHECK_ERROR(mLog);
        
        mReady = true;
    }
    
    void Render::InitializeStdShader()
    {
        const char* fragShader = "#version 330 core                \n \
        out vec4 color;                                            \n \
                                                                   \n \
        in vec4 fragmentColor;                                     \n \
        in vec2 UV;                                                \n \
                                                                   \n \
        uniform sampler2D textureSampler;                          \n \
                                                                   \n \
        void main()                                                \n \
        {                                                          \n \
            color = texture( textureSampler, UV );   \n \
        }";
        
        const char* vtxShader = "#version 330 core                 \n \
        uniform mat4 modelViewProjectionMatrix;                    \n \
                                                                   \n \
        layout(location = 0) in vec3 vertexPosition_modelspace;    \n \
        layout(location = 1) in vec4 vertexColor;                  \n \
        layout(location = 2) in vec2 vertexUV;                     \n \
                                                                   \n \
        out vec4 fragmentColor;                                    \n \
        out vec2 UV;                                               \n \
                                                                   \n \
        uniform mat4 MVP;                                          \n \
                                                                   \n \
        void main(){                                               \n \
            gl_Position = MVP * vec4(vertexPosition_modelspace,1); \n \
            fragmentColor = vertexColor;                           \n \
            UV = vertexUV;                                         \n \
        }";
        
        mStdShader = BuildShaderProgram(vtxShader, fragShader);
        mStdShaderMvpUniform = glGetUniformLocation(mStdShader, "MVP");
        mStdShaderTextureSample  = glGetUniformLocation(mStdShader, "textureSampler");
        
        glUseProgram(mStdShader);
    }

	void Render::OnFrameResized()
	{
		mResolution = o2Application.GetContentSize();
        mCurrentResolution = mResolution;
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
		//get max texture size
		glGetIntegerv(GL_MAX_TEXTURE_SIZE, &mMaxTextureSize.x);
		mMaxTextureSize.y = mMaxTextureSize.x;
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

		SetupViewMatrix(mResolution);
		UpdateCameraTransforms();
        
        GL_CHECK_ERROR(mLog);
        
        Clear(Color4::Red());
	}

	void Render::DrawPrimitives()
	{
		if (mLastDrawVertex < 1)
            return;
        
        GLfloat* mapVertexBuffer = (GLfloat*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE);
        
        if (mapVertexBuffer)
        {
            memcpy(mapVertexBuffer, mVertexData, sizeof(Vertex2)*mLastDrawVertex);
            glUnmapBuffer(GL_ARRAY_BUFFER);
        }
        else GL_CHECK_ERROR(mLog);
        
        GLfloat* mapIndexBuffer = (GLfloat*)glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_READ_WRITE);
        
        if (mapIndexBuffer)
        {
            memcpy(mapIndexBuffer, mVertexIndexData, sizeof(UInt16)*mLastDrawIdx);
            glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
        }
        else GL_CHECK_ERROR(mLog);
        
        glDrawElements(mCurrentPrimitiveType, mLastDrawIdx, GL_UNSIGNED_SHORT, (void*)0);

		GL_CHECK_ERROR(mLog);

		mFrameTrianglesCount += mTrianglesCount;
		mLastDrawVertex = mTrianglesCount = mLastDrawIdx = 0;

		mDIPCount++;
	}

	void Render::SetupViewMatrix(const Vec2I& viewSize)
	{
		mCurrentResolution = viewSize;
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

	void Render::UpdateCameraTransforms()
	{
		DrawPrimitives();
        
        float projMat[16];
        Math::OrthoProjMatrix(projMat, 0.0f, (float)mCurrentResolution.x, (float)mCurrentResolution.y, 0.0f, 0.0f, 10.0f);
        glViewport(0, 0, mCurrentResolution.x, mCurrentResolution.y);

		float modelMatrix[16] =
		{
			1,           0,            0, 0,
			0,          -1,            0, 0,
			0,           0,            1, 0,
			Math::Round(mCurrentResolution.x*0.5f), Math::Round(mCurrentResolution.y*0.5f), -1, 1
		};

		Basis defaultCameraBasis((Vec2F)mCurrentResolution*-0.5f, Vec2F::Right()*mCurrentResolution.x, Vec2F().Up()*mCurrentResolution.y);
		Basis camTransf = mCamera.GetBasis().Inverted()*defaultCameraBasis;

		float camTransfMatr[16] =
		{
			camTransf.xv.x,   camTransf.xv.y,   0, 0,
			camTransf.yv.x,   camTransf.yv.y,   0, 0,
			0,                0,                0, 0,
			camTransf.offs.x, camTransf.offs.y, 0, 1
		};

        float mvp[16];
        float finalCamMtx[16];
        mtxMultiply(finalCamMtx, modelMatrix, camTransfMatr);
        mtxMultiply(mvp, projMat, finalCamMtx);
        
        glUniformMatrix4fv(mStdShaderMvpUniform, 1, GL_FALSE, mvp);
        
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
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, mLastDrawTexture->mHandle);
                glUniform1i(mStdShaderTextureSample, 0);

				GL_CHECK_ERROR(mLog);
			}
			else
            {
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, 0);
                glUniform1i(mStdShaderTextureSample, 0);
                
                GL_CHECK_ERROR(mLog);
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
        GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
        GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
        
        GLint Result = GL_FALSE;
        int InfoLogLength;
        
        glShaderSource(VertexShaderID, 1, &vertexSource , NULL);
        glCompileShader(VertexShaderID);
        
        // Выполняем проверку Вершинного шейдера
        glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
        glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
        if ( InfoLogLength > 0 ){
            std::vector<char> VertexShaderErrorMessage(InfoLogLength+1);
            glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
            fprintf(stdout, "%s \n", &VertexShaderErrorMessage[0]);
        }
        
        glShaderSource(FragmentShaderID, 1, &fragmentSource , NULL);
        glCompileShader(FragmentShaderID);
        
        glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
        glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
        if ( InfoLogLength > 0 ){
            std::vector<char> FragmentShaderErrorMessage(InfoLogLength+1);
            glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
            fprintf(stdout, "%s \n", &FragmentShaderErrorMessage[0]);
        }
        
        GLuint ProgramID = glCreateProgram();
        glAttachShader(ProgramID, VertexShaderID);
        glAttachShader(ProgramID, FragmentShaderID);
        glLinkProgram(ProgramID);
        
        glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
        glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
        if ( InfoLogLength > 0 ){
            std::vector<char> ProgramErrorMessage(InfoLogLength+1);
            glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
            fprintf(stdout, "%s \n", &ProgramErrorMessage[0]);
        }
        
        glDeleteShader(VertexShaderID);
        glDeleteShader(FragmentShaderID);
        
        return ProgramID;
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
