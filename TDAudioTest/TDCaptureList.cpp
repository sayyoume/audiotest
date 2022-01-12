#include "TDCaptureList.h"
#include <Functiondiscoverykeys_devpkey.h>

//#define  CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
//#define  IID IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);

#define SAFE_RELEASE(punk)  \
              if ((punk) != NULL)  \
                { (punk)->Release(); (punk) = NULL; }

#define EXIT_ON_ERROR(hres)  \
              if (FAILED(hres)) { goto Exit; }

TDCaptureList::TDCaptureList(): m_cRef(1){
	CoInitializeEx(NULL, COINIT_MULTITHREADED);
	m_nIn = 0;
	m_nOut = 0;
}

TDCaptureList::~TDCaptureList()
{
	SAFE_RELEASE(m_pEnumerator);
	m_pEnumerator->UnregisterEndpointNotificationCallback(this);
}

void TDCaptureList::Init(){
	::CoInitialize(NULL);
	HRESULT hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**)&m_pEnumerator);
	if (FAILED(hr)) {
		return;
	}
	
	m_pEnumerator->RegisterEndpointNotificationCallback(this);
}

void TDCaptureList::GetCaputureList(std::map<std::wstring,std::wstring>& map)
{
	HRESULT hr = S_OK;
	IMMDeviceCollection* pCollection = NULL;
	IMMDevice* pEndpoint = NULL;
	IPropertyStore* pProps = NULL;
	LPWSTR pwszID = NULL;
	
	//枚举系统所有设备
	//eAll : 会列举出系统中所有的音频设备包括Render --- 扬声器， Capture --- Microphone, Stereo Mixer;
	//eRender: 会列举出系统中所有的音频播放设备；
	//eCapture: 会列举出系统中所有音频采集设备
	hr = m_pEnumerator->EnumAudioEndpoints(eAll, DEVICE_STATE_ACTIVE, &pCollection);
	EXIT_ON_ERROR(hr);

	UINT  count;
	hr = pCollection->GetCount(&count);
	EXIT_ON_ERROR(hr);

	// Each iteration prints the name of an endpoint device.
	PROPVARIANT varName;
	for (ULONG i = 0; i < count; i++)
	{
		// Get pointer to endpoint number i.
		hr = pCollection->Item(i, &pEndpoint);
		EXIT_ON_ERROR(hr);

		// Get the endpoint ID string.
		hr = pEndpoint->GetId(&pwszID);
		EXIT_ON_ERROR(hr);

		hr = pEndpoint->OpenPropertyStore(
			STGM_READ, &pProps);
		EXIT_ON_ERROR(hr);

		// Initialize container for property value.
		PropVariantInit(&varName);

		// Get the endpoint's friendly-name property.
		hr = pProps->GetValue(PKEY_Device_FriendlyName, &varName);
		EXIT_ON_ERROR(hr);

		// Print endpoint friendly name and endpoint ID.
		//printf("Endpoint %d: \"%S\" (%S)\n", i, varName.pwszVal, pwszID);

		std::wstring sCaptureName = varName.pwszVal;
		map[pwszID] = sCaptureName;

		CoTaskMemFree(pwszID);
		pwszID = NULL;
		PropVariantClear(&varName);
	}

Exit:
	CoTaskMemFree(pwszID);
	pwszID = NULL;
	PropVariantClear(&varName);
	SAFE_RELEASE(pCollection);
	SAFE_RELEASE(pEndpoint);
	SAFE_RELEASE(pProps);
}

HRESULT STDMETHODCALLTYPE TDCaptureList::OnDefaultDeviceChanged(EDataFlow flow, ERole role,LPCWSTR pwstrDeviceId)
{
	std::string pszFlow;
	std::string pszRole;
	std::wstring sName;
	//_PrintDeviceName(pwstrDeviceId,sName);

	switch (flow)
	{
	case eRender:
		pszFlow = "eRender";
		break;
	case eCapture:
	{
		m_vec.push_back(sName);
		pszFlow = "eCapture";
		break;
	}
	}

	switch (role)
	{
	case eConsole:
		pszRole = "eConsole";
		break;
	case eMultimedia:
		pszRole = "eMultimedia";
		break;
	case eCommunications:
		pszRole = "eCommunications";
		break;
	}

	if (pszFlow == "eCapture" && pszRole == "eMultimedia")
	{
		printf("  -->OnDefaultDeviceChanged\n");
		int ii = 3;
	}

	return S_OK;
}

HRESULT STDMETHODCALLTYPE TDCaptureList::OnDeviceAdded(LPCWSTR pwstrDeviceId)
{
	printf("  -->OnDeviceAdded\n");
	return S_OK;
};

HRESULT STDMETHODCALLTYPE TDCaptureList::OnDeviceRemoved(LPCWSTR pwstrDeviceId)
{
	printf("  -->OnDeviceRemoved\n");
	return S_OK;
}

HRESULT STDMETHODCALLTYPE TDCaptureList::OnDeviceStateChanged(LPCWSTR pwstrDeviceId,DWORD dwNewState)
{
	std::string pszState;
	
	switch (dwNewState)
	{
	case DEVICE_STATE_ACTIVE:
		{
		printf("OnDeviceStateChanged::DEVICE_STATE_ACTIVE\n");
			pszState = "ACTIVE";
			m_nIn++;
			break;
		}
	case DEVICE_STATE_DISABLED:
		pszState = "DISABLED";
		break;
	case DEVICE_STATE_NOTPRESENT:
		pszState = "NOTPRESENT";
		break;
	case DEVICE_STATE_UNPLUGGED:{
		printf("OnDeviceStateChanged::UNPLUGGED\n");
			pszState = "UNPLUGGED";
			m_nOut++;
		}
		break;
	}
	if (m_nIn == 2) {
		m_nIn = 0;
	}

	if (m_nOut == 2) {
		m_nOut = 0;
	}

	return S_OK;
}

HRESULT STDMETHODCALLTYPE TDCaptureList::OnPropertyValueChanged(LPCWSTR pwstrDeviceId,const PROPERTYKEY key)
{
	/*printf("  -->Changed device property "
		"{%8.8x-%4.4x-%4.4x-%2.2x%2.2x-%2.2x%2.2x%2.2x%2.2x%2.2x%2.2x}#%d\n",
		key.fmtid.Data1, key.fmtid.Data2, key.fmtid.Data3,
		key.fmtid.Data4[0], key.fmtid.Data4[1],
		key.fmtid.Data4[2], key.fmtid.Data4[3],
		key.fmtid.Data4[4], key.fmtid.Data4[5],
		key.fmtid.Data4[6], key.fmtid.Data4[7],
		key.pid);*/
	std::wstring sname;
	_PrintDeviceName(pwstrDeviceId, sname);
	
	//sname.find("")
	printf("ValueChanged:: %s\n",sname.c_str());

	return S_OK;
}

// Given an endpoint ID string, print the friendly device name.
HRESULT TDCaptureList::_PrintDeviceName(LPCWSTR pwstrId,std::wstring& sName)
{
	HRESULT hr = S_OK;
	IMMDevice* pDevice = NULL;
	IPropertyStore* pProps = NULL;
	PROPVARIANT varString;

	CoInitialize(NULL);
	PropVariantInit(&varString);

	if (m_pEnumerator == NULL)
	{
		// Get enumerator for audio endpoint devices.
		hr = CoCreateInstance(__uuidof(MMDeviceEnumerator),
			NULL, CLSCTX_INPROC_SERVER,
			__uuidof(IMMDeviceEnumerator),
			(void**)&m_pEnumerator);
	}
	if (hr == S_OK)
	{
		hr = m_pEnumerator->GetDevice(pwstrId, &pDevice);
	}
	if (hr == S_OK)
	{
		hr = pDevice->OpenPropertyStore(STGM_READ, &pProps);
	}
	if (hr == S_OK)
	{
		// Get the endpoint device's friendly-name property.
		hr = pProps->GetValue(PKEY_Device_FriendlyName, &varString);
	}
	printf("----------------------\nDevice name: \"%S\"\n"
		"  Endpoint ID string: \"%S\"\n",
		(hr == S_OK) ? varString.pwszVal : L"null device",
		(pwstrId != NULL) ? pwstrId : L"null ID");
	sName = varString.pwszVal;
	PropVariantClear(&varString);

	SAFE_RELEASE(pProps)
		SAFE_RELEASE(pDevice)
		CoUninitialize();
	return hr;
}