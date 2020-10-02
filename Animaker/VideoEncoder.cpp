#include"Animaker.h"

using namespace Animaker::Core;
using namespace Animaker::Math;

Animaker::Core::VideoEncoder::VideoEncoder(){
	this->pc_sinkWriter = nullptr;
	this->streamIndex = -1;
	this->timeStamp = 0;
	this->frameDuration = 333333.3333333333333333333;//in 100-nanoseconds unit
	this->x = -1;
	this->y = -1;
	this->stride = -1;
	this->bufferLenthInBytes = -1;
}

RV Animaker::Core::VideoEncoder::CreateVideoOutput(const WCHAR* outFilePath, INT32 x, INT32 y){
	this->x = x;
	this->y = y;
	this->stride = this->x * 4;
	this->bufferLenthInBytes = this->x * this->y * 4;

	IMFMediaType* pc_mediaTypeOut = nullptr;
	IMFMediaType* pc_mediaTypeIn = nullptr;

	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
	if (SUCCEEDED(hr) == false) {
		return RV_ERR_CreateVideoOutput;
	}
	hr = MFStartup(MF_VERSION);
	if(SUCCEEDED(hr) == false) {
		return RV_ERR_CreateVideoOutput;
	}
	//create sink writer
	hr = MFCreateSinkWriterFromURL(outFilePath, NULL, NULL, &this->pc_sinkWriter);
	if (SUCCEEDED(hr) == false) {
		return RV_ERR_CreateVideoOutput;
	}
	//create and init out_media type
	hr = MFCreateMediaType(&pc_mediaTypeOut);
	if (SUCCEEDED(hr) == false) {
		return RV_ERR_CreateVideoOutput;
	}
	hr = pc_mediaTypeOut->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);
	if (SUCCEEDED(hr) == false) {
		return RV_ERR_CreateVideoOutput;
	}
	hr = pc_mediaTypeOut->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_WMV3);
	if (SUCCEEDED(hr) == false) {
		return RV_ERR_CreateVideoOutput;
	}
	hr = pc_mediaTypeOut->SetUINT32(MF_MT_AVG_BITRATE, 999999999);
	if (SUCCEEDED(hr) == false) {
		return RV_ERR_CreateVideoOutput;
	}
	hr = pc_mediaTypeOut->SetUINT32(MF_MT_INTERLACE_MODE, MFVideoInterlace_Progressive);
	if (SUCCEEDED(hr) == false) {
		return RV_ERR_CreateVideoOutput;
	}
	hr = MFSetAttributeSize(pc_mediaTypeOut, MF_MT_FRAME_SIZE, this->x, this->y);
	if (SUCCEEDED(hr) == false) {
		return RV_ERR_CreateVideoOutput;
	}
	hr = MFSetAttributeRatio(pc_mediaTypeOut, MF_MT_FRAME_RATE, 30, 1);
	if (SUCCEEDED(hr) == false) {
		return RV_ERR_CreateVideoOutput;
	}
	hr = MFSetAttributeRatio(pc_mediaTypeOut, MF_MT_PIXEL_ASPECT_RATIO, 1, 1);
	if (SUCCEEDED(hr) == false) {
		return RV_ERR_CreateVideoOutput;
	}
	hr = this->pc_sinkWriter->AddStream(pc_mediaTypeOut, &this->streamIndex);
	if (SUCCEEDED(hr) == false) {
		return RV_ERR_CreateVideoOutput;
	}
	//create and init in_media type
	hr = MFCreateMediaType(&pc_mediaTypeIn);
	if (SUCCEEDED(hr) == false) {
		return RV_ERR_CreateVideoOutput;
	}
	hr = pc_mediaTypeIn->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);
	if (SUCCEEDED(hr) == false) {
		return RV_ERR_CreateVideoOutput;
	}
	hr = pc_mediaTypeIn->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_RGB32);
	if (SUCCEEDED(hr) == false) {
		return RV_ERR_CreateVideoOutput;
	}
	hr = pc_mediaTypeIn->SetUINT32(MF_MT_INTERLACE_MODE, MFVideoInterlace_Progressive);
	if (SUCCEEDED(hr) == false) {
		return RV_ERR_CreateVideoOutput;
	}
	hr = MFSetAttributeSize(pc_mediaTypeIn, MF_MT_FRAME_SIZE, this->x, this->y);
	if (SUCCEEDED(hr) == false) {
		return RV_ERR_CreateVideoOutput;
	}
	hr = MFSetAttributeRatio(pc_mediaTypeIn, MF_MT_FRAME_RATE, 30, 1);
	if (SUCCEEDED(hr) == false) {
		return RV_ERR_CreateVideoOutput;
	}
	hr = MFSetAttributeRatio(pc_mediaTypeIn, MF_MT_PIXEL_ASPECT_RATIO, 1, 1);
	if (SUCCEEDED(hr) == false) {
		return RV_ERR_CreateVideoOutput;
	}
	hr = this->pc_sinkWriter->SetInputMediaType(streamIndex, pc_mediaTypeIn, NULL);
	if (SUCCEEDED(hr) == false) {
		return RV_ERR_CreateVideoOutput;
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////////
	//now we are ready to fight
	hr = this->pc_sinkWriter->BeginWriting();
	if (SUCCEEDED(hr) == false) {
		return RV_ERR_CreateVideoOutput;
	}

	pc_mediaTypeIn->Release();
	pc_mediaTypeOut->Release();

	return RV_OK;
}

RV Animaker::Core::VideoEncoder::WriteFrame(void* pv_data){
	BYTE* p_sampleBuffer;

	HRESULT hr = MFCreateMemoryBuffer(this->bufferLenthInBytes, &this->pc_sampleBuffer);
	if (SUCCEEDED(hr) == false) {
		return RV_ERR_CreateVideoOutput;
	}

	hr = this->pc_sampleBuffer->Lock(&p_sampleBuffer, NULL, NULL);
	//ZeroMemory(p_sampleBuffer, 1920 * 1080 * 4);
	MFCopyImage(p_sampleBuffer, this->stride, (BYTE*)pv_data, this->stride, this->stride, this->y);
	this->pc_sampleBuffer->Unlock();

	// Set the data length of the buffer.
	hr = this->pc_sampleBuffer->SetCurrentLength(this->bufferLenthInBytes);
	if (SUCCEEDED(hr) == false) {
		return RV_ERR_CreateVideoOutput;
	}

	// Create a media sample and add the buffer to the sample.
	hr = MFCreateSample(&this->pc_sample);
	if (SUCCEEDED(hr) == false) {
		return RV_ERR_CreateVideoOutput;
	}

	hr = this->pc_sample->AddBuffer(this->pc_sampleBuffer);
	if (SUCCEEDED(hr) == false) {
		return RV_ERR_CreateVideoOutput;
	}

	hr = this->pc_sample->SetSampleDuration(this->frameDuration);//in 100-nanoseconds unit
	if (SUCCEEDED(hr) == false) {
		return RV_ERR_CreateVideoOutput;
	}

	hr = this->pc_sample->SetSampleTime(this->timeStamp);
	if (SUCCEEDED(hr) == false) {
		return RV_ERR_WriteVideo;
	}

	hr = this->pc_sinkWriter->WriteSample(this->streamIndex, this->pc_sample);
	if (SUCCEEDED(hr) == false) {
		return RV_ERR_WriteVideo;
	}

	this->timeStamp += this->frameDuration;

	return RV_OK;
}

RV Animaker::Core::VideoEncoder::CloseVideoOutput(){
	HRESULT hr = this->pc_sinkWriter->Finalize();
	if (SUCCEEDED(hr) == false) {
		return RV_ERR_CloseVideo;
	}

	MFShutdown();
	CoUninitialize();

	return RV_OK;
}
