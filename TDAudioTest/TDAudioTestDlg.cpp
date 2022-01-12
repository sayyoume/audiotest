
// TDAudioTestDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "TDAudioTest.h"
#include "TDAudioTestDlg.h"
#include "afxdialogex.h"
#include "interf_dec.h"
#include "wavwriter.h"
#include <portaudio.h>
#include <iostream>
#include <Mmsystem.h>
#include "Sine.h"

#include "SDL.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace std;
// CTDAudioTestDlg 对话框

#pragma comment(lib,"D:\\code\\TDAudioTest\\Debug\\opencoreD.lib")
#pragma comment(lib,"D:\\code\\TDAudioTest\\Debug\\portaudioD.lib")
#pragma comment(lib,"Winmm.lib")
#pragma comment(lib,"SDL2.lib")
//#pragma comment(lib,"Winmm.lib")
//#pragma comment(lib,"Winmm.lib")

const int sizes[] = { 12, 13, 15, 17, 19, 20, 26, 31, 5, 6, 5, 5, 0, 0, 0, 0 };

CTDAudioTestDlg::CTDAudioTestDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_TDAUDIOTEST_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTDAudioTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CTDAudioTestDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CTDAudioTestDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CTDAudioTestDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CTDAudioTestDlg::OnBnClickedButton3)
END_MESSAGE_MAP()


// CTDAudioTestDlg 消息处理程序

BOOL CTDAudioTestDlg::OnInitDialog()
{
	//AllocConsole();
	//freopen("conout$", "w", stdout);
	
	
	//printf("hello hplonline!-_-\n");
	//std::cout << "i'm cout" << std::endl;
	//freopen("conout$", "w", stderr);
	//std::cerr << "i'm cerr" << std::endl;


	CDialogEx::OnInitDialog();
	tdlist.Init();
	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CTDAudioTestDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CTDAudioTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


int amr2wav(std::string sAmrFile,std::string sWavFile) {
	FILE* in;
	char header[6];
	int n;
	void* wav, * amr;
	

	fopen_s(&in,sAmrFile.c_str(), "rb");
	if (!in) {
		
		return 1;
	}
	n = fread(header, 1, 6, in);
	if (n != 6 || memcmp(header, "#!AMR\n", 6)) {
		fprintf(stderr, "Bad header\n");
		//return 1;
	}
	wav = wav_write_open(sWavFile.c_str(), 8000, 16, 1);
	if (!wav) {
		
		return 1;
	}

	amr = Decoder_Interface_init();
	while (1) {
		uint8_t buffer[500], littleendian[320], * ptr;
		int size, i;
		int16_t outbuffer[160];
		/* Read the mode byte */
		n = fread(buffer, 1, 1, in);
		if (n <= 0)
			break;
		/* Find the packet size */
		size = sizes[(buffer[0] >> 3) & 0x0f];
		n = fread(buffer + 1, 1, size, in);
		if (n != size)
			break;

		/* Decode the packet */
		Decoder_Interface_Decode(amr, buffer, outbuffer, 0);

		/* Convert to little endian and write to wav */
		ptr = littleendian;
		for (i = 0; i < 160; i++) {
			*ptr++ = (outbuffer[i] >> 0) & 0xff;
			*ptr++ = (outbuffer[i] >> 8) & 0xff;
		}
		wav_write_data(wav, littleendian, 320);
	}
	fclose(in);
	Decoder_Interface_exit(amr);
	wav_write_close(wav);
	return 0;
}


void play(std::string filename)
{
	char* buff = nullptr;
	FILE* file;
	fopen_s(&file,filename.c_str(), "rb");
	if (file != nullptr)
	{
		fseek(file, 0, SEEK_END);
		long fileSize = ftell(file);
		rewind(file);

		int num = fileSize / sizeof(char);

		buff = (char*)malloc(sizeof(char) * num);

		if (buff == NULL)
		{
			return ;
		}
		fread(buff, sizeof(char), num, file);
	}
	DeleteFileA(filename.c_str());
	PlaySoundA((LPCSTR)buff, NULL, SND_MEMORY | SND_LOOP | SND_ASYNC);
	free(buff);
}


//获取设备列表
void CTDAudioTestDlg::OnBnClickedButton1()
{
	//std::string fileName = "C:\\Users\\qinxw\\Downloads\\60.amr";
	//std::string fileNamewav = "C:\\Users\\qinxw\\Downloads\\60.wav";
	//amr2wav(fileName, fileNamewav);
	
	//play(fileNamewav);
	
	
	PlaySoundA("d:\\test_1.pcm", NULL, SND_FILENAME);
	//PlaySoundA((LPCSTR)buff, NULL, SND_MEMORY | SND_LOOP | SND_ASYNC);

	//std::map<wstring, wstring> tmap;
	//tdlist.GetCaputureList(tmap);

	int ii = 3;
}



static  Uint8* audio_chunk;
static  Uint32  audio_len;
static  Uint8* audio_pos;

/* Audio Callback
 * The audio function callback takes the following parameters:
 * stream: A pointer to the audio buffer to be filled
 * len: The length (in bytes) of the audio buffer
 *
*/
void  fill_audio(void* udata, Uint8* stream, int len) {
	//SDL 2.0
	SDL_memset(stream, 0, len);
	if (audio_len == 0)
		return;
	len = (len > audio_len ? audio_len : len);

	SDL_MixAudio(stream, audio_pos, len, SDL_MIX_MAXVOLUME);
	audio_pos += len;
	audio_len -= len;
}

string& replace_str(string& str, const string& to_replaced, const string& newchars)
{
	for (string::size_type pos(0); pos != string::npos; pos += newchars.length())
	{
		pos = str.find(to_replaced, pos);
		if (pos != string::npos)
			str.replace(pos, to_replaced.length(), newchars);
		else
			break;
	}
	return   str;
}

//播放
void CTDAudioTestDlg::OnBnClickedButton2()
{
	if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_TIMER)) {
		printf("Could not initialize SDL - %s\n", SDL_GetError());
		return ;
	}
	//SDL_AudioSpec
	SDL_AudioSpec wanted_spec;
	wanted_spec.freq = 8000;
	wanted_spec.format = AUDIO_S16SYS;
	wanted_spec.channels = 2;
	wanted_spec.silence = 0;
	wanted_spec.samples = 1024;
	wanted_spec.callback = fill_audio;

	if (SDL_OpenAudio(&wanted_spec, NULL) < 0) {
		printf("can't open audio.\n");
		return ;
	}

	FILE* fp;
	fopen_s(&fp, "d:\\audio_short16.pcm", "rb+");
	if (fp == NULL) {
		printf("cannot open this file\n");
		return ;
	}



	int pcm_buffer_size = 4096;
	char* pcm_buffer = (char*)malloc(pcm_buffer_size);
	int data_count = 0;

	//Play
	SDL_PauseAudio(0);

	while (1) {
		if (fread(pcm_buffer, 1, pcm_buffer_size, fp) != pcm_buffer_size) {
			// Loop
			fseek(fp, 0, SEEK_SET);
			fread(pcm_buffer, 1, pcm_buffer_size, fp);
			data_count = 0;
		}
		//printf("Now Playing %10d Bytes data.\n", data_count);
		data_count += pcm_buffer_size;
		//Set audio buffer (PCM data)
		audio_chunk = (Uint8*)pcm_buffer;
		//Audio buffer length
		audio_len = pcm_buffer_size;
		audio_pos = audio_chunk;

		while (audio_len > 0)//Wait until finish
			SDL_Delay(1);
	}
	free(pcm_buffer);
	SDL_Quit();
	

}



//waveout 播放
void CTDAudioTestDlg::OnBnClickedButton3()
{
	
}

