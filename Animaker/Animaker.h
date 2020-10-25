#pragma once

#ifdef ANIMAKER_EXPORTS
#define DLLIO __declspec(dllexport)
#else 
#define DLLIO __declspec(dllimport)
#endif

#include<stdio.h>
#include<Windows.h>
#include<d2d1_3.h>
#include<d3d11_4.h>
#include<mfapi.h>
#include<mfidl.h>
#include<Mfreadwrite.h>
#include<mferror.h>
#include<dwrite_3.h> 

#pragma comment(lib, "mfreadwrite")
#pragma comment(lib, "mfplat")
#pragma comment(lib, "mfuuid")
#pragma comment(lib, "d2d1")
#pragma comment(lib, "d3d11")
#pragma comment(lib, "dxgi")
#pragma comment(lib,"dwrite")

namespace Animaker {

	namespace Math {
		
		class DLLIO Float4 {
		public:
			float x, y, z, w;
			Float4(){}
			Float4(float x0,float x1,float x2,float x3) {
				this->x = x0;
				this->y = x1;
				this->z = x2;
				this->w = x3;
			}
			Float4 operator+(Float4 f4) {
				return Float4(this->x + f4.x, this->y + f4.y, this->z + f4.z, this->w + f4.w);
			}
			Float4 operator*(float f) {
				return Float4(this->x * f, this->y * f, this->z * f, this->w * f);
			}
		};

		class DLLIO Float4x4 {		
		public:
			Float4 xyzw0, xyzw1, xyzw2, xyzw3;//because this is how hlsl stores a matrix
			Float4x4(){}
			Float4x4(Float4 vx, Float4 vy, Float4 vz, Float4 vw) {
				this->xyzw0 = Float4(vx.x, vy.x, vz.x, vw.x);
				this->xyzw1 = Float4(vx.y, vy.y, vz.y, vw.y);
				this->xyzw2 = Float4(vx.z, vy.z, vz.z, vw.z);
				this->xyzw3 = Float4(vx.w, vy.w, vz.w, vw.w);
			}
		};

	}

	namespace Animation{	
		class Interpolator {
			virtual void Move(Math::Float4 from,Math::Float4 to,float t) = 0;
		};
	}

	namespace Core {

		enum RV {
			RV_OK,
			RV_ERR_CreateD3DDevice,
			RV_ERR_CreateD2DFactory,
			RV_ERR_CreateGPUTextureRT,
			RV_ERR_CreateCPUTextureRT,
			RV_ERR_CreateRTV,
			RV_ERR_CreateDepthStencil,
			RV_ERR_CreateDSV,
			RV_ERR_CreateVertexShader,
			RV_ERR_CreateInputLayout,
			RV_ERR_CreatePixelShader,
			RV_ERR_OpenVertexShader,
			RV_ERR_OpenPixelShader,
			RV_ERR_QueryIDXGISurface,
			RV_ERR_CreateD2DRenderTarget,
			RV_ERR_InvalidArg,
			RV_ERR_CreateVertexBuffer,
			RV_ERR_CreateVideoOutput,
			RV_ERR_WriteVideo,
			RV_ERR_CloseVideo,
			RV_ERR_CreateMatrixBuffer,
			RV_ERR_CreateD2DBrush,
			RV_ERR_CreateDWriteFactory,
			RV_ERR_CreateTextFormat
		};

		class GraphicsObject;//forward declaration
		class Surface;//forward declaration
		class TextObject;

		class DLLIO Renderer {
		private:
			bool isInitialized;
			ID3D11Device* pc_d3dDevice;
			ID3D11DeviceContext* pc_d3dDeviceCtx;
			ID2D1Factory* pc_d2dFactory;
			IDWriteFactory* pc_dwriteFactory;

			ID3D11Texture2D* pc_depthStencilTexture;
			ID3D11DepthStencilView* pc_dsv;
			ID3D11VertexShader* pc_vertexShader;
			ID3D11PixelShader* pc_pixelShader;
			ID3D11InputLayout* pc_inputLayout;
		public:
			class Vertex {
			public:
				Math::Float4 pos;
				Math::Float4 color;
			};
			Renderer();
			RV Init();
			void GORender(GraphicsObject* pc_go,Surface* pc_surface);
			void TORender(TextObject* pc_to, Surface* pc_surface);
			RV GOUploadVertexBuffer(GraphicsObject* pc_go);//must be 16-byte aligned,suggest use Structure [Vertex]
			void* GOGetGPUVertexData(GraphicsObject* pc_go);
			void GOReleaseGPUVertexData(GraphicsObject* pc_go);
			RV GOUploadWorldMatrixBuffer(GraphicsObject* pc_go);
			void GOUpdateWorldMatrixBuffer(GraphicsObject* pc_go);
			RV TOInitText(TextObject* pc_to);
			RV SurfaceInit(Surface* pc_surface);
			RV SurfaceUploadProjectionMatrixBuffer(Surface* pc_surface);
			void SurfaceUpdateProjectionMatrixBuffer(Surface* pc_surface);
			void SurfaceClear(Surface* pc_surface);
			void* SurfaceGetData(Surface* pc_surface);		
			~Renderer();
		};

		class DLLIO Surface {
		private:
			friend class Renderer;
			ID3D11Texture2D* pc_gpuTextureRT;
			ID3D11Texture2D* pc_cpuTextureRT;
			ID3D11Buffer* pc_gpuMatrixBuffer;//projection matrix
			ID3D11RenderTargetView* pc_rtv;
			D3D11_VIEWPORT viewPort;
			Math::Float4x4 projection;//my coord
			//dxwrite stuff
			ID2D1RenderTarget* pc_d2dRenderTarget;//this is DXGISurface specific,so we cant put it in Renderer
			ID2D1SolidColorBrush* pc_brush;
			ID2D1LinearGradientBrush* pc_linearGradientBrush;
			INT32 pixelXLength, pixelYLength;
		public:

			Surface(){
				pixelXLength = pixelYLength = 0;
			}

			void SetProjection(float x, float y) {
				this->projection.xyzw0 = { 1.0f / x,0.0f,0.0f,0.0f };
				this->projection.xyzw1 = { 0.0f,1.0f / y,0.0f,0.0f };
				this->projection.xyzw2 = { 0.0f,0.0f,1.0f,0.0f };
				this->projection.xyzw3 = { 0.0f,0.0f,0.0f,1.0f };
			}

			~Surface() {
				this->pc_cpuTextureRT->Release();
				this->pc_gpuTextureRT->Release();
				this->pc_rtv->Release();
			}

		};

		class DLLIO GraphicsObject : public Animation::Interpolator {
		private:
			friend class Renderer;
			ID3D11Buffer* pc_gpuVertexBuffer;
			ID3D11Buffer* pc_gpuMatrixBuffer;
			
		public:
			INT32 vertexCount;
			INT32 bufferSize;
			Renderer::Vertex* pc_cpuVertexBuffer;
			Math::Float4x4 matrix;

			GraphicsObject();

			virtual void Move(Math::Float4 from, Math::Float4 to, float t);

			~GraphicsObject();
		};

		class DLLIO TextObject : Animation::Interpolator {
		private:
			friend class Renderer;
			WCHAR* text;
			INT32 textLength;
			IDWriteTextFormat* pc_textFormat;
			D2D1_RECT_F rect;
			float x;
			float y;
			float xLength;
			float yLength;
			//alpha
			
		public:
			float t;
			TextObject();
			void SetText(const WCHAR* text);
			void SetRect(float x, float y, float xLength, float yLength);
			//
			virtual void Move(Math::Float4 from, Math::Float4 to, float t);
		};

		//VideoEncoder only support 1920x1080 BGRA mp4 output with 30 fps
		class DLLIO VideoEncoder {
		private:
			IMFSinkWriter* pc_sinkWriter;
			DWORD streamIndex;//warning:DWORD is unsigned long type
			UINT64 timeStamp;//in 100-nanoseconds unit
			UINT32 frameDuration;
			IMFSample* pc_sample;//a media file is consists of many samples
			IMFMediaBuffer* pc_sampleBuffer;
			UINT32 bufferLenthInBytes;
			UINT32 x;
			UINT32 y;
			UINT32 stride;
		public:
			VideoEncoder();
			RV CreateVideoOutput(const WCHAR* outFilePath,INT32 x,INT32 y);
			RV WriteFrame(void* pv_data);
			RV CloseVideoOutput();
		};

	}

}
