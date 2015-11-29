#include "Sound.h"
#include <mmsystem.h>
#include "Utility.h"

SoundMgr* SoundMgr::GetInstance()
{
    static SoundMgr instance;
    return &instance;
}

SoundMgr::SoundMgr()
{
    device = NULL;
    primaryBuffer = NULL;
}

SoundMgr::~SoundMgr()
{
    Shutdown();
}

BOOL SoundMgr::Init(HWND hwnd)
{
    HRESULT         result;
    DSBUFFERDESC    bufferDesc;
    WAVEFORMATEX    waveFormat;

    if(FAILED(DirectSoundCreate8(NULL, &device, NULL)))
    {
        MessageBox(hwnd, L"初始化声卡失败", L"Error", MB_OK);
        return FALSE;
    }

    if(FAILED(device->SetCooperativeLevel(hwnd, DSSCL_PRIORITY)))
    {
        MessageBox(hwnd, L"设置调和等级失败", L"Error", MB_OK);
        return FALSE;
    }

    bufferDesc.dwSize = sizeof(DSBUFFERDESC);
    bufferDesc.dwFlags = DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRLVOLUME;
    bufferDesc.dwBufferBytes = 0;
    bufferDesc.dwReserved = 0;
    bufferDesc.lpwfxFormat = NULL;
    bufferDesc.guid3DAlgorithm = GUID_NULL;

    if(FAILED(device->CreateSoundBuffer(&bufferDesc, &primaryBuffer, NULL)))
    {
        MessageBox(hwnd, L"创建声卡主缓冲失败", L"Error", MB_OK);
        return FALSE;
    }

    waveFormat.wFormatTag = WAVE_FORMAT_PCM;
    waveFormat.nSamplesPerSec = 44100;
    waveFormat.wBitsPerSample = 16;
    waveFormat.nChannels = 2;
    waveFormat.nBlockAlign = (waveFormat.wBitsPerSample / 8) * waveFormat.nChannels;
    waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
    waveFormat.cbSize = 0;

    result = primaryBuffer->SetFormat(&waveFormat);
    if(FAILED(result))
    {
        MessageBox(hwnd, L"设置声卡主缓冲格式失败", L"Error", MB_OK);
        return FALSE;
    }

    return TRUE;
}

void SoundMgr::Shutdown()
{
    //析构顺序不能颠倒，回收设别会自动回收由设备创建出来的缓冲
    SafeRelease(primaryBuffer);
    SafeRelease(device);
}

IDirectSound8 * SoundMgr::GetDevice()
{
    return device;
}

Sound::Sound():playPos(0), soundData(NULL)
{

}

Sound::~Sound()
{
    SafeRelease(soundData);
}

BOOL Sound::LoadFile(const string filename)
{
    fileName = filename;
    if((soundData = SoundBatch::GetInstance()->GetSoundByName(fileName)) != NULL)
    {
        return TRUE;
    }
    if((soundData = SoundBatch::GetInstance()->LoadSound(fileName)) != NULL)
    {
        return TRUE;
    }
    return FALSE;
}

void Sound::Play()
{
    if(FAILED(soundData->SetCurrentPosition(playPos)))
    {
        return ;
    }

    if(FAILED(soundData->SetVolume(DSBVOLUME_MAX)))
    {
        return ;
    }

    if(FAILED(soundData->Play(0, 0, 0)))
    {
        return ;
    }
}

void Sound::Pause()
{
    if(soundData != NULL)
    {
        soundData->GetCurrentPosition(&playPos, NULL);
        soundData->Stop();
    }
}

void Sound::Stop()
{
    if(soundData != NULL)
    {
        soundData->Stop();
        playPos = 0;
    }
}

BOOL Sound::IsPlaying()
{
    if(soundData != NULL)
    {
        DWORD status = 0;
        HRESULT hr = soundData->GetStatus(&status);
        if(hr != DS_OK)
        {
            return FALSE;
        }
        return ((status & DSBSTATUS_PLAYING) ? TRUE : FALSE);
    }
    return FALSE;
}

void Sound::Release()
{
    SafeRelease(soundData);
}

SoundBatch::~SoundBatch()
{
    Clear();
}

void SoundBatch::Clear()
{
    name2SoundTable.clear();
}

SoundBatch* SoundBatch::GetInstance()
{
    static SoundBatch instance;
    return &instance;
}

IDirectSoundBuffer8* SoundBatch::GetSoundByName(const string &fileName)
{
    if(name2SoundTable.find(fileName) != name2SoundTable.end())
    {
        IDirectSoundBuffer8 *retBuffer;
        SoundMgr::GetInstance()->GetDevice()->DuplicateSoundBuffer(
            name2SoundTable[fileName],
            (LPDIRECTSOUNDBUFFER*)&retBuffer);
        return retBuffer;
    }
    return NULL;
}

IDirectSoundBuffer8* SoundBatch::LoadSound(const string &fileName)
{
    IDirectSoundBuffer8 *retBuffer;
    if((retBuffer = GetSoundByName(fileName)) != NULL)
    {
        return retBuffer;
    }
    FILE* filePtr;
    WaveHeaderType waveFileHeader;    

    if(fopen_s(&filePtr, fileName.c_str(), "rb") != 0)
    {
        wstring msg = L"打开" + StrToWstr(fileName) + L"声音文件失败";
        MessageBox(NULL, msg.c_str(), L"Error", MB_OK);
        return NULL;
    }

    if(fread(&waveFileHeader, sizeof(waveFileHeader), 1, filePtr) != 1)
    {
        wstring msg = L"读取" + StrToWstr(fileName) + L"文件格式头失败";
        MessageBox(NULL, msg.c_str(), L"Error", MB_OK);
        return NULL;
    }

    if(IsLegal(waveFileHeader) == FALSE)
    {
        wstring msg = StrToWstr(fileName) + L"文件格式非法";
        MessageBox(NULL, msg.c_str(), L"Error", MB_OK);
        return NULL;
    }

    WAVEFORMATEX waveFormat;
    waveFormat.wFormatTag = WAVE_FORMAT_PCM;
    waveFormat.nSamplesPerSec = 44100;
    waveFormat.wBitsPerSample = 16;
    waveFormat.nChannels = 2;
    waveFormat.nBlockAlign = (waveFormat.wBitsPerSample / 8) * waveFormat.nChannels;
    waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
    waveFormat.cbSize = 0;

    DSBUFFERDESC bufferDesc;
    bufferDesc.dwSize = sizeof(DSBUFFERDESC);
    bufferDesc.dwFlags = DSBCAPS_CTRLVOLUME;
    bufferDesc.dwBufferBytes = waveFileHeader.dataSize;
    bufferDesc.dwReserved = 0;
    bufferDesc.lpwfxFormat = &waveFormat;
    bufferDesc.guid3DAlgorithm = GUID_NULL;

    IDirectSoundBuffer* tempBuffer;
    if(FAILED(SoundMgr::GetInstance()->GetDevice()->CreateSoundBuffer(&bufferDesc, &tempBuffer, NULL)))
    {
        return NULL;
    }

    IDirectSoundBuffer8* soundData;
    if(FAILED(tempBuffer->QueryInterface(IID_IDirectSoundBuffer8, (void**)&soundData)))
    {
        return NULL;
    }

    SafeRelease(tempBuffer);

    fseek(filePtr, sizeof(WaveHeaderType), SEEK_SET);
    unsigned char* waveData = new unsigned char[waveFileHeader.dataSize];
    if(!waveData)
    {
        return NULL;
    }

    if(fread(waveData, 1, waveFileHeader.dataSize, filePtr) != waveFileHeader.dataSize)
    {
        return NULL;
    }

    if(fclose(filePtr) != 0)
    {
        return NULL;
    }

    unsigned char *bufferPtr;
    unsigned long bufferSize;
    if(FAILED(soundData->Lock(0, waveFileHeader.dataSize, (void**)&bufferPtr, (DWORD*)&bufferSize, NULL, 0, 0)))
    {
        return NULL;
    }
    memcpy(bufferPtr, waveData, waveFileHeader.dataSize); 
    if(FAILED(soundData->Unlock((void*)bufferPtr, bufferSize, NULL, 0)))
    {
        return NULL;
    }

    ReleasNewArray(waveData);
    name2SoundTable[fileName] = soundData;

    return soundData;
}

BOOL SoundBatch::IsLegal(const WaveHeaderType &waveFileHeader)
{
    if((waveFileHeader.chunkId[0] != 'R') || (waveFileHeader.chunkId[1] != 'I') || 
        (waveFileHeader.chunkId[2] != 'F') || (waveFileHeader.chunkId[3] != 'F'))
    {
        return FALSE;
    }

    if((waveFileHeader.format[0] != 'W') || (waveFileHeader.format[1] != 'A') ||
        (waveFileHeader.format[2] != 'V') || (waveFileHeader.format[3] != 'E'))
    {
        return FALSE;
    }

    if((waveFileHeader.subChunkId[0] != 'f') || (waveFileHeader.subChunkId[1] != 'm') ||
        (waveFileHeader.subChunkId[2] != 't') || (waveFileHeader.subChunkId[3] != ' '))
    {
        return FALSE;
    }

    if(waveFileHeader.audioFormat != WAVE_FORMAT_PCM)
    {
        return FALSE;
    }

    if(waveFileHeader.numChannels != 2)
    {
        return FALSE;
    }

    if(waveFileHeader.sampleRate != 44100)
    {
        return FALSE;
    }

    if(waveFileHeader.bitsPerSample != 16)
    {
        return FALSE;
    }

    if((waveFileHeader.dataChunkId[0] != 'd') || (waveFileHeader.dataChunkId[1] != 'a') ||
        (waveFileHeader.dataChunkId[2] != 't') || (waveFileHeader.dataChunkId[3] != 'a'))
    {
        return FALSE;
    }

    return TRUE;
}
