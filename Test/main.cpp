#include<iostream>
#include<Animaker.h>

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
	cout << r.GOUploadWorldMatrixBuffer(&go);
	cout << endl;

	

	Surface surface;
	cout << r.SurfaceInit(&surface);
	cout << endl;

	TextObject txtObj;
	txtObj.SetText(L"But what is Socket actually???");
	txtObj.SetRect(-10.0, 10.0, 10.0, 10.0);
	//TextObject txtObj1;
	//txtObj1.SetText(L"你觉得这个字体怎么样呢？");
	//txtObj1.SetRect(0, 1080 / 2, 1920, 1080 / 2);

	cout << r.TOInitText(&txtObj);
	//cout << r.TOInitText(&txtObj1);

	//DWORD* p = (DWORD*)malloc(1920 * 1080 * 4);

	Math::Float4 from, to;
	from = { 0,0,0.0f,0 };
	to = { 1,1,0.0f,0 };

	surface.SetProjection(10.0f, 10.0f);
	cout << r.SurfaceUploadProjectionMatrixBuffer(&surface);
	r.SurfaceUpdateProjectionMatrixBuffer(&surface);

	Renderer::Vertex* pc_vertBuffer;
	pc_vertBuffer = (Renderer::Vertex*)r.GOGetGPUVertexData(&go);
	if (pc_vertBuffer == nullptr) {
		exit(-1);
	}
	pc_vertBuffer[0].pos.x = -10;
	r.GOReleaseGPUVertexData(&go);

	VideoEncoder v;
	v.CreateVideoOutput(L"c:\\game\\wow.wmv",1920,1080);
	for (float i = 0.0f; i <= 1.5f; i+=0.01) {
		r.SurfaceClear(&surface);
		
		//go.Move(from, to, i);
		//Renderer::Vertex* pc_vertBuffer;
		//pc_vertBuffer = (Renderer::Vertex*)r.GOGetGPUVertexData(&go);
		//if (pc_vertBuffer == nullptr) {
		//	exit(-1);
		//}
		//pc_vertBuffer[0].pos.x += i/2.0f;
		//r.GOReleaseGPUVertexData(&go);
		//r.GOUpdateWorldMatrixBuffer(&go);

		//r.GORender(&go, &surface);
		//txtObj.t = i;
		//txtObj.Move(from, to, i);
		txtObj.t = i;
		r.TORender(&txtObj, &surface);
		void* p = r.SurfaceGetData(&surface);
		cout << i << ",";
		//ZeroMemory(p + i * 1920 * 4, 1920 * 4);
		v.WriteFrame(p);
	}
	//for (float i = 0.0f; i <= 1.5f; i += 0.01) {
		//r.SurfaceClear(&surface);
		//go.Move(&from, &to, i);
		//r.GOUpdateMatrixBuffer(&go);
		//r.Render(&go, &surface);
		//r.TORender(&txtObj, &surface);
		//txtObj1.t = i;
		//r.TORender(&txtObj1, &surface);
		//void* p = r.SurfaceGetData(&surface);
		//cout << i;
		//ZeroMemory(p + i * 1920 * 4, 1920 * 4);
		//v.WriteFrame(p);
	//}
	v.CloseVideoOutput();
	return 0;
}

int mainaa() {
	Renderer r;
	r.Init();
	//r.TestFunction();
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

int maadin() {

	Renderer r;
	cout << r.Init();

	Surface surface;
	cout << r.SurfaceInit(&surface);

	TextObject txtObj;
	txtObj.SetText(L"Wow,你好呀世界！");
	txtObj.SetRect(0, 0, 500, 500);

	cout << r.TOInitText(&txtObj);

	r.TORender(&txtObj, &surface);

	VideoEncoder v;
	cout << v.CreateVideoOutput(L"c:\\game\\1000.wmv", 1920, 1080);
	for (int i = 0; i < 100; i++) {
		cout << v.WriteFrame((void*)r.SurfaceGetData(&surface));
	}
	v.CloseVideoOutput();
	return 0;

	return 0;
}
