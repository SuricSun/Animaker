#include"Animaker.h"

using namespace Animaker::Core;

Animaker::Core::Renderer::Renderer(){

	this->isInitialized = false;
	return;

}

RV Animaker::Core::Renderer::Init(){

	//IDXGIAdapter* pAdapter;

	//IDXGIFactory* pFactory = NULL;
	//DXGI_ADAPTER_DESC desc = { 0 };
	//CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&pFactory);

	//pFactory->EnumAdapters(0, &pAdapter);
    //pAdapter->GetDesc(&desc);

	if (this->isInitialized == false) {

		UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

		D3D_FEATURE_LEVEL featureLevels[] = {
			D3D_FEATURE_LEVEL_11_1,
		};

		HRESULT result = D3D11CreateDevice(
			nullptr
			,D3D_DRIVER_TYPE_HARDWARE
			,0
			,creationFlags
			,featureLevels
			,ARRAYSIZE(featureLevels)
			,D3D11_SDK_VERSION
			,&this->pc_d3dDevice
			,nullptr
			,&this->pc_d3dDeviceCtx
		);
		if (!SUCCEEDED(result)) {
			return RV_ERR_CreateD3DDevice;
		}
		
		//create gpuRenderTarget
		D3D11_TEXTURE2D_DESC desc;
		desc.Width = 1920;
		desc.Height = 1080;
		desc.MipLevels = desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_RENDER_TARGET;
		desc.CPUAccessFlags = 0;//CPU access is not needed
		desc.MiscFlags = 0;
		result = this->pc_d3dDevice->CreateTexture2D(&desc, nullptr, &this->pc_gpuTextureRT);
		if (!SUCCEEDED(result)) {
			return RV_ERR_CreateGPUTextureRT;
		}

		//create cpuRenderTarget
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		desc.Usage = D3D11_USAGE_STAGING;
		desc.BindFlags = 0;
		result = this->pc_d3dDevice->CreateTexture2D(&desc, nullptr, &this->pc_cpuTextureRT);
		if (!SUCCEEDED(result)) {
			return RV_ERR_CreateCPUTextureRT;
		}

		//create render target view
		result = this->pc_d3dDevice->CreateRenderTargetView(this->pc_gpuTextureRT, nullptr, &this->pc_rtv);
		if (!SUCCEEDED(result)) {
			return RV_ERR_CreateRTV;
		}

		//create depth stencil
		CD3D11_TEXTURE2D_DESC depthStencilDesc(
			DXGI_FORMAT_D24_UNORM_S8_UINT,
			1920,
			1080,
			1, // This depth stencil view has only one texture.
			1, // Use a single mipmap level.
			D3D11_BIND_DEPTH_STENCIL
		);
		result = this->pc_d3dDevice->CreateTexture2D(&depthStencilDesc, nullptr, &this->pc_depthStencilTexture);
		if (!SUCCEEDED(result)) {
			return RV_ERR_CreateDepthStencil;
		}

		//create dsv
		CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc(D3D11_DSV_DIMENSION_TEXTURE2D);
		result = this->pc_d3dDevice->CreateDepthStencilView(this->pc_depthStencilTexture, &depthStencilViewDesc, &this->pc_dsv);
		if (!SUCCEEDED(result)) {
			return RV_ERR_CreateDSV;
		}

		//create viewport for screen mapping in rs stage
		ZeroMemory(&this->viewPort, sizeof(D3D11_VIEWPORT));
		this->viewPort.Height = 1080.0f;
		this->viewPort.Width = 1920.0f;
		this->viewPort.MinDepth = 0;
		this->viewPort.MaxDepth = 1;

		//create shaders
		FILE* pc_vShaderFile, * pc_pShaderFile;
		pc_vShaderFile = nullptr;
		pc_pShaderFile = nullptr;

		BYTE* p_buffer;
		size_t bufferSize = 1024 * 1024;
		DWORD bytesRead = 0;
		p_buffer = new BYTE[bufferSize];

		//fopen_s(&pc_vShaderFile, "C:\\Work\\VS Projects\\AniMaker\\x64\\Debug\\VertexShader.cso", "rb");
		//if (pc_vShaderFile == nullptr) {
		//	this->stateCode = RV_ERR_OpenVertexShader;
		//	return;
		//}
		//bytesRead = fread_s(p_buffer, bufferSize, bufferSize, 1, pc_vShaderFile);
		HANDLE fileHandle = CreateFile(L"C:\\Work\\VS Solutions\\Animaker\\x64\\Debug\\VertexShader.cso", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (fileHandle == INVALID_HANDLE_VALUE) {
			return RV_ERR_OpenVertexShader;
		}
		ReadFile(fileHandle, p_buffer, GetFileSize(fileHandle, NULL), &bytesRead, NULL);

		result = this->pc_d3dDevice->CreateVertexShader(
			p_buffer,
			bytesRead,
			nullptr,
			&this->pc_vertexShader
		);
		if (!SUCCEEDED(result)) {
			return RV_ERR_CreateVertexShader;
		}

		D3D11_INPUT_ELEMENT_DESC iaDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT,
			0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },

			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT,
			0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};

		result = this->pc_d3dDevice->CreateInputLayout(
			iaDesc,
			ARRAYSIZE(iaDesc),
			p_buffer,
			bytesRead,
			&this->pc_inputLayout
		);
		if (!SUCCEEDED(result)) {
			return RV_ERR_CreateInputLayout;
		}

		delete[bufferSize] p_buffer;

		p_buffer = new BYTE[bufferSize];
		bytesRead = 0;

		fileHandle = CreateFile(L"C:\\Work\\VS Solutions\\Animaker\\x64\\Debug\\PixelShader.cso", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (fileHandle == INVALID_HANDLE_VALUE) {
			return RV_ERR_OpenVertexShader;
		}
		ReadFile(fileHandle, p_buffer, GetFileSize(fileHandle, NULL), &bytesRead, NULL);
		result = pc_d3dDevice->CreatePixelShader(
			p_buffer,
			bytesRead,
			nullptr,
			&this->pc_pixelShader
		);
		if (!SUCCEEDED(result)) {
			return RV_ERR_CreatePixelShader;
		}

		delete[bufferSize] p_buffer;

		//create d2d stuff
		result = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &this->pc_d2dFactory);
		if (!SUCCEEDED(result)) {
			return RV_ERR_CreateD2DFactory;
		}

		D2D1_RENDER_TARGET_PROPERTIES props =
			D2D1::RenderTargetProperties(
				D2D1_RENDER_TARGET_TYPE_DEFAULT,
				D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED),
				96,
				96
			);
		IDXGISurface* pc_surface = nullptr;
		result = pc_gpuTextureRT->QueryInterface(IID_PPV_ARGS(&pc_surface));
		if (!SUCCEEDED(result)) {
			return RV_ERR_QueryIDXGISurface;
		}

		result = this->pc_d2dFactory->CreateDxgiSurfaceRenderTarget(pc_surface, &props, &this->pc_d2dRenderTarget);
		if (!SUCCEEDED(result)) {
			return RV_ERR_CreateD2DRenderTarget;
		}

		this->isInitialized = true;

	}
	return RV_OK;
}

RV Animaker::Core::Renderer::Render(GraphicsObject* pc_graObj,Surface* pc_surface){
	const float teal[] = { 0.2f, 0.2f, 0.2f, 1.000f };
	this->pc_d3dDeviceCtx->ClearRenderTargetView(
		pc_surface->pc_rtv,
		teal
	);
	this->pc_d3dDeviceCtx->ClearDepthStencilView(
		this->pc_dsv,
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f,
		0);

	this->pc_d3dDeviceCtx->OMSetRenderTargets(
		1,
		&pc_surface->pc_rtv,
		this->pc_dsv
	);

	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	this->pc_d3dDeviceCtx->IASetVertexBuffers(
		0,
		1,
		&pc_graObj->pc_gpuVertexBuffer,
		&stride,
		&offset
	);

	this->pc_d3dDeviceCtx->IASetPrimitiveTopology(
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST
	);

	this->pc_d3dDeviceCtx->IASetInputLayout(this->pc_inputLayout);

	this->pc_d3dDeviceCtx->VSSetConstantBuffers(0, 1, &pc_graObj->pc_gpuMatrixBuffer);

	this->pc_d3dDeviceCtx->VSSetShader(
		this->pc_vertexShader,
		nullptr,
		0
	);

	this->pc_d3dDeviceCtx->RSSetViewports(1, &pc_surface->viewPort);

	// Set up the pixel shader stage.
	this->pc_d3dDeviceCtx->PSSetShader(
		this->pc_pixelShader,
		nullptr,
		0
	);

	// Calling Draw tells Direct3D to start sending commands to the graphics device.
	this->pc_d3dDeviceCtx->Draw(
		pc_graObj->vertexCount,
		0
	);

	//d2d draw
	//ID2D1SolidColorBrush* pc_brush = nullptr;
	//this->pc_d2dRenderTarget->CreateSolidColorBrush(D2D1::ColorF(1, 0, 0), &pc_brush);
	//this->pc_d2dRenderTarget->BeginDraw();
	//this->pc_d2dRenderTarget->DrawRectangle(D2D1::RectF(0, 0, 100, 100), pc_brush);
	//this->pc_d2dRenderTarget->EndDraw();

	//HANDLE fileHandle = CreateFile(L"C:\\game\\10.bmp", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	//if (fileHandle == INVALID_HANDLE_VALUE) {
	//	return (RV)-1;
	//}
	//DWORD bytesWritten = 0;
	//WriteFile(fileHandle, p_data, 1920 * 1080 * 4, &bytesWritten, NULL);

	return RV_OK;
}

RV Animaker::Core::Renderer::GOUploadVertexBuffer(GraphicsObject* pc_graObj){
	HRESULT hr = S_OK;

	CD3D11_BUFFER_DESC vDesc(
		pc_graObj->bufferSize,
		D3D11_BIND_VERTEX_BUFFER
	);

	D3D11_SUBRESOURCE_DATA vData;
	ZeroMemory(&vData, sizeof(D3D11_SUBRESOURCE_DATA));
	vData.pSysMem = pc_graObj->pc_cpuVertexBuffer;
	vData.SysMemPitch = 0;
	vData.SysMemSlicePitch = 0;

	hr = this->pc_d3dDevice->CreateBuffer(
		&vDesc,
		&vData,
		&pc_graObj->pc_gpuVertexBuffer
	);
	if (!SUCCEEDED(hr)) {
		return RV_ERR_CreateVertexBuffer;
	}
	return RV_OK;
}

RV Animaker::Core::Renderer::GOInitMatrixBuffer(GraphicsObject* pc_go){
	CD3D11_BUFFER_DESC cbDesc(
		sizeof(Math::Float4x4),
		D3D11_BIND_CONSTANT_BUFFER
	);
	

	HRESULT hr = this->pc_d3dDevice->CreateBuffer(&cbDesc, nullptr, &pc_go->pc_gpuMatrixBuffer);
	if (SUCCEEDED(hr)==false) {
		return RV_ERR_CreateMatrixBuffer;
	}

	return RV_OK;
}

void Animaker::Core::Renderer::GOUpdateMatrixBuffer(GraphicsObject* pc_go)
{
	this->pc_d3dDeviceCtx->UpdateSubresource(pc_go->pc_gpuMatrixBuffer, 0, nullptr, &pc_go->matrix, 0, 0);

	return;
}

RV Animaker::Core::Renderer::SurfaceInit(Surface* pc_surface){

	pc_surface->x = 1920;
	pc_surface->y = 1080;

	D3D11_TEXTURE2D_DESC desc;
	desc.Width = 1920;
	desc.Height = 1080;
	desc.MipLevels = desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_RENDER_TARGET;
	desc.CPUAccessFlags = 0;//CPU access is not needed
	desc.MiscFlags = 0;
	HRESULT result = this->pc_d3dDevice->CreateTexture2D(&desc, nullptr, &pc_surface->pc_gpuTextureRT);
	if (!SUCCEEDED(result)) {
		return RV_ERR_CreateGPUTextureRT;
	}
	//create cpuRenderTarget
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	desc.Usage = D3D11_USAGE_STAGING;
	desc.BindFlags = 0;
	result = this->pc_d3dDevice->CreateTexture2D(&desc, nullptr, &pc_surface->pc_cpuTextureRT);
	if (!SUCCEEDED(result)) {
		return RV_ERR_CreateCPUTextureRT;
	}
	//create render target view
	result = this->pc_d3dDevice->CreateRenderTargetView(pc_surface->pc_gpuTextureRT, nullptr, &pc_surface->pc_rtv);
	if (!SUCCEEDED(result)) {
		return RV_ERR_CreateRTV;
	}
	//set viewport
	pc_surface->viewPort.Width = pc_surface->x;
	pc_surface->viewPort.Height = pc_surface->y;
	pc_surface->viewPort.TopLeftX = 0;
	pc_surface->viewPort.TopLeftY = 0;
	pc_surface->viewPort.MaxDepth = 1;
	pc_surface->viewPort.MinDepth = 0;

	return RV_OK;
}

void* Animaker::Core::Renderer::SurfaceGetData(Surface* pc_surface){
	this->pc_d3dDeviceCtx->CopyResource(pc_surface->pc_cpuTextureRT, pc_surface->pc_gpuTextureRT);
	D3D11_MAPPED_SUBRESOURCE mappedData;
	HRESULT hr = this->pc_d3dDeviceCtx->Map(pc_surface->pc_cpuTextureRT, 0, D3D11_MAP_READ, 0, &mappedData);
	if (SUCCEEDED(hr) == false) {
		return nullptr;
	}
	return mappedData.pData;
}

RV Animaker::Core::Renderer::TestFunction(){
	HRESULT hr = S_OK;

	Vertex rectVertices[3] =
	{
		{Math::Float4(0.0f,0.5f,0.5f,1.0f), Math::Float4(1.0f,0.0f,0.0f,1.0f)},
		{Math::Float4(0.5f,-0.5f, 0.5f,1.0f), Math::Float4(0.0f,1.0f,0.0f,1.0f)},
		{Math::Float4(-0.5f, -0.5f,0.5f,1.0f), Math::Float4(0.0f,0.0f,1.0f,1.0f)},
	};

	CD3D11_BUFFER_DESC vDesc(
		sizeof(rectVertices),
		D3D11_BIND_VERTEX_BUFFER
	);

	D3D11_SUBRESOURCE_DATA vData;
	ZeroMemory(&vData, sizeof(D3D11_SUBRESOURCE_DATA));
	vData.pSysMem = rectVertices;
	vData.SysMemPitch = 0;
	vData.SysMemSlicePitch = 0;

	hr = this->pc_d3dDevice->CreateBuffer(
		&vDesc,
		&vData,
		&this->pc_vertexBuffer
	);
	if (!SUCCEEDED(hr)) {
		return (RV)-1;
	}

	//draw
	const float teal[] = { 0.2f, 0.2f, 0.2f, 1.000f };
	this->pc_d3dDeviceCtx->ClearRenderTargetView(
		this->pc_rtv,
		teal
	);
	this->pc_d3dDeviceCtx->ClearDepthStencilView(
		this->pc_dsv,
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f,
		0);

	this->pc_d3dDeviceCtx->OMSetRenderTargets(
		1,
		&this->pc_rtv,
		this->pc_dsv
	);

	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	this->pc_d3dDeviceCtx->IASetVertexBuffers(
		0,
		1,
		&this->pc_vertexBuffer,
		&stride,
		&offset
	);

	this->pc_d3dDeviceCtx->IASetPrimitiveTopology(
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST
	);

	this->pc_d3dDeviceCtx->IASetInputLayout(this->pc_inputLayout);

	this->pc_d3dDeviceCtx->VSSetShader(
		this->pc_vertexShader,
		nullptr,
		0
	);

	this->pc_d3dDeviceCtx->RSSetViewports(1, &this->viewPort);

	// Set up the pixel shader stage.
	this->pc_d3dDeviceCtx->PSSetShader(
		this->pc_pixelShader,
		nullptr,
		0
	);

	// Calling Draw tells Direct3D to start sending commands to the graphics device.
	this->pc_d3dDeviceCtx->Draw(
		3,
		0
	);

	//d2d draw
	ID2D1SolidColorBrush* pc_brush = nullptr;
	this->pc_d2dRenderTarget->CreateSolidColorBrush(D2D1::ColorF(1, 0, 0), &pc_brush);
	this->pc_d2dRenderTarget->BeginDraw();
	this->pc_d2dRenderTarget->DrawRectangle(D2D1::RectF(0, 0, 100, 100), pc_brush);
	this->pc_d2dRenderTarget->EndDraw();

	this->pc_d3dDeviceCtx->CopyResource(this->pc_cpuTextureRT, this->pc_gpuTextureRT);
	D3D11_MAPPED_SUBRESOURCE mappedData;
	this->pc_d3dDeviceCtx->Map(this->pc_cpuTextureRT, 0, D3D11_MAP_READ, 0, &mappedData);

	BYTE* p_data = (BYTE*)mappedData.pData;

	HANDLE fileHandle = CreateFile(L"C:\\game\\10.bmp", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (fileHandle == INVALID_HANDLE_VALUE) {
		return (RV)-1;
	}
	DWORD bytesWritten = 0;
	WriteFile(fileHandle, p_data, 1920 * 1080 * 4, &bytesWritten, NULL);

	return RV_OK;
}

Animaker::Core::Renderer::~Renderer(){
	this->pc_cpuTextureRT->Release();
	this->pc_d2dRenderTarget->Release();
	this->pc_d3dDeviceCtx->Release();
	this->pc_depthStencilTexture->Release();
	this->pc_dsv->Release();
	this->pc_gpuTextureRT->Release();
	this->pc_pixelShader->Release();
	this->pc_rtv->Release();
	this->pc_vertexShader->Release();
	this->pc_d3dDevice->Release();
	this->pc_d2dFactory->Release();
	this->pc_inputLayout->Release();
}