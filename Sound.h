#ifndef __SOUND__
#define __SOUND__

#include "PreHeader.h"

struct WaveHeaderType
{
    char chunkId[4];
    unsigned long chunkSize;
    char format[4];
    char subChunkId[4];
    unsigned long subChunkSize;
    unsigned short audioFormat;
    unsigned short numChannels;
    unsigned long sampleRate;
    unsigned long bytesPerSecond;
    unsigned short blockAlign;
    unsigned short bitsPerSample;
    char dataChunkId[4];
    unsigned long dataSize;
};

class SoundMgr
{
public:
    //单例
    static SoundMgr* GetInstance();

    ~SoundMgr();

    //初始化DirectSound设备和primaryBuffer
    BOOL Init(HWND);

    //销毁设备
    void Shutdown();

    //获得声卡设备，释放DirectSound设备和primaryBuffer
    IDirectSound8 * GetDevice();

private:

    SoundMgr();

    SoundMgr(const SoundMgr&){};

private:
    IDirectSound8* device;
    IDirectSoundBuffer* primaryBuffer;
};

class Sound
{
private:
    string fileName;
    DWORD playPos;
    IDirectSoundBuffer8* soundData;

public:
    Sound();
    ~Sound();
    
    //载入声音文件
    BOOL LoadFile(const string);

    //播放音频文件
    void Play();

    //暂停
    void Pause();

    //停止
    void Stop();

    //是否正在播放
    BOOL IsPlaying();

    //释放soundData
    void Release();
};

class SoundBatch
{
private:
    map<string, IDirectSoundBuffer8*>   name2SoundTable;
public:
    ~SoundBatch();

    //单例
    static SoundBatch* GetInstance();

    //加载wav格式音频文件
    IDirectSoundBuffer8* LoadSound(const string &);

    //按名取得wav格式音频文件
    IDirectSoundBuffer8* GetSoundByName(const string &);

    //清空
    void Clear();

private:
    SoundBatch(){};

    SoundBatch(const SoundBatch &){};

    SoundBatch & operator = (const SoundBatch &){};

    //判断载入的文件格式是否合法
    BOOL IsLegal(const WaveHeaderType &);
};

#endif