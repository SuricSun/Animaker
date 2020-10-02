#include<iostream>
#include"Animaker.h"

using namespace std;
using namespace Animaker::Core;
using namespace Animaker;

int ma234in() {
	IDXGIAdapter* pAdapter;

	IDXGIFactory* pFactory = NULL;
	DXGI_ADAPTER_DESC desc;
	CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&pFactory);
	for (UINT i = 0; pFactory->EnumAdapters(i, &pAdapter) != DXGI_ERROR_NOT_FOUND; ++i)
	{
		cout << pAdapter->GetDesc(&desc);
		cout << desc.Description;
		return 0;
	}
}

int main() {
	Renderer r;
	cout << r.Init();
	cout << endl;
	GraphicsObject go;
	Renderer::Vertex rectVertices[3] =
	{
		{Math::Float4(-1.0,1.0,0.5f,1.0f), Math::Float4(1.0f,0.0f,0.0f,1.0f)},
		{Math::Float4(0.0f,1.0f,0.5f,1.0), Math::Float4(0.0f,1.0f,0.0f,1.0f)},
		{Math::Float4(-1.0,0.0,0.5f,1.0f), Math::Float4(0.0f,0.0f,1.0f,1.0f)},
	};
	go.pc_cpuVertexBuffer = rectVertices;
	go.vertexCount = 3;
	go.bufferSize = sizeof(Renderer::Vertex) * go.vertexCount;
	go.matrix.xyzw0 = { 1,0,0,0 };
	go.matrix.xyzw1 = { 0,1,0,0 };
	go.matrix.xyzw2 = { 0,0,1,0 };
	go.matrix.xyzw3 = { 0,0,0,1 };
	cout<<r.GOUploadVertexBuffer(&go);
	cout << endl;
	cout << r.GOInitMatrixBuffer(&go);
	cout << endl;

	Surface surface;
	cout << r.SurfaceInit(&surface);
	cout << endl;

	//DWORD* p = (DWORD*)malloc(1920 * 1080 * 4);

	Math::Float4 from, to;
	from = { 0,0,0.0f,0 };
	to = { 1,-1,0.0f,0 };

	VideoEncoder v;
	v.CreateVideoOutput(L"c:\\game\\wow.wmv",1920,1080);
	for (float i = 0.0f; i < 1.0f; i+=0.01) {
		
		go.Interpolate(&from, &to, i);
		r.GOUpdateMatrixBuffer(&go);
		r.Render(&go, &surface);
		void* p = r.SurfaceGetData(&surface);
		cout << i;
		//ZeroMemory(p + i * 1920 * 4, 1920 * 4);
		v.WriteFrame(p);
	}
	v.CloseVideoOutput();
	return 0;
}

int mainaa() {
	Renderer r;
	r.Init();
	r.TestFunction();
	return 0;
}

int maina() {
	VideoEncoder v;
	cout<<v.CreateVideoOutput(L"c:\\game\\1000.wmv",512,512);
	BYTE* p = (BYTE*)malloc(512 * 512 * 4);
	for (int i = 0; i < 100; i++) {
		cout<<v.WriteFrame((void*)p);
	}
	v.CloseVideoOutput();
	return 0;
}
