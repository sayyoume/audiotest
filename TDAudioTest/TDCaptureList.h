#pragma once

/*
 * ������������豸��Σ���ȡ�����豸�б�  
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

	//���û�������Ƶ�ս���豸���豸��ɫʱ����
	HRESULT STDMETHODCALLTYPE OnDefaultDeviceChanged(EDataFlow flow, ERole role, LPCWSTR pwstrDeviceId);
	//���û�����Ƶ�ս���豸��ӵ�ϵͳʱ���á�
	HRESULT STDMETHODCALLTYPE OnDeviceAdded(LPCWSTR pwstrDeviceId);
	//���û���ϵͳ��ɾ����Ƶ�ս���豸ʱ���á�
	HRESULT STDMETHODCALLTYPE OnDeviceRemoved(LPCWSTR pwstrDeviceId);
	//����Ƶ�ս���豸���豸״̬����ʱ����
	HRESULT STDMETHODCALLTYPE OnDeviceStateChanged(LPCWSTR pwstrDeviceId, DWORD dwNewState);
	//����Ƶ�ս���豸����ֵ����ʱ����
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


