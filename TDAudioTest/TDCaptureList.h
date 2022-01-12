#pragma once

/*
 * 可以用来检测设备插拔，获取输入设备列表  
 * 2021/9/31 qinxiaowei
*/

#include <string>
#include <mmdeviceapi.h>
#include <map>
#include <vector>


class CMMNotificationClient;

class TDCaptureList : public IMMNotificationClient {

public:
	TDCaptureList();
	~TDCaptureList();
public:
	void Init();
	void GetCaputureList(std::map<std::wstring, std::wstring>& map);

	// IUnknown methods -- AddRef, Release, and QueryInterface
	ULONG STDMETHODCALLTYPE AddRef()
	{
		return InterlockedIncrement(&m_cRef);
	}

	ULONG STDMETHODCALLTYPE Release()
	{
		ULONG ulRef = InterlockedDecrement(&m_cRef);
		if (0 == ulRef)
		{
			delete this;
		}
		return ulRef;
	}

	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, VOID** ppvInterface)
	{
		if (IID_IUnknown == riid)
		{
			AddRef();
			*ppvInterface = (IUnknown*)this;
		}
		else if (__uuidof(IMMNotificationClient) == riid)
		{
			AddRef();
			*ppvInterface = (IMMNotificationClient*)this;
		}
		else
		{
			*ppvInterface = NULL;
			return E_NOINTERFACE;
		}
		return S_OK;
	}

	//当用户更改音频终结点设备的设备角色时调用
	HRESULT STDMETHODCALLTYPE OnDefaultDeviceChanged(EDataFlow flow, ERole role, LPCWSTR pwstrDeviceId);
	//当用户将音频终结点设备添加到系统时调用。
	HRESULT STDMETHODCALLTYPE OnDeviceAdded(LPCWSTR pwstrDeviceId);
	//当用户从系统中删除音频终结点设备时调用。
	HRESULT STDMETHODCALLTYPE OnDeviceRemoved(LPCWSTR pwstrDeviceId);
	//当音频终结点设备的设备状态更改时调用
	HRESULT STDMETHODCALLTYPE OnDeviceStateChanged(LPCWSTR pwstrDeviceId, DWORD dwNewState);
	//当音频终结点设备属性值更改时调用
	HRESULT STDMETHODCALLTYPE OnPropertyValueChanged(LPCWSTR pwstrDeviceId, const PROPERTYKEY key);

	HRESULT _PrintDeviceName(LPCWSTR  pwstrId,std::wstring& sName);
private:
	IMMDeviceEnumerator* m_pEnumerator = NULL;
	std::vector<std::wstring> m_vec;
private:
	LONG m_cRef;
	int m_nIn;
	int m_nOut;
};


