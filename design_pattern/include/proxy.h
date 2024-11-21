class ThirdPartyYoutubeLib
{
public:
    virtual ~ThirdPartyYoutubeLib() {};
    virtual void listVideos() = 0;
    virtual void getVideoInfo(int id) = 0;
    virtual void downloadVideo(int id) = 0;
};

class ThirdPartyYoutubeClass : public ThirdPartyYoutubeLib
{
public:
    virtual ~ThirdPartyYoutubeClass() {};
    void listVideos() override;
    void getVideoInfo(int id) override;
    void downloadVideo(int id) override;
};

class CachedYoutubeClass : public ThirdPartyYoutubeLib
{
private:
    ThirdPartyYoutubeLib* _service;
    bool _needReset;
public:
    CachedYoutubeClass() = default;
    CachedYoutubeClass(ThirdPartyYoutubeLib*);
    virtual ~CachedYoutubeClass()
    {
        delete _service;
    };

    void listVideos() override;
    void getVideoInfo(int id) override;
    void downloadVideo(int id) override;
};

class YoutubeManager
{
protected:
    ThirdPartyYoutubeLib* _service;
public:
    YoutubeManager(ThirdPartyYoutubeLib*);
    void renderVideoList();
    void renderVideoPage(int);
    void reactOnUserInput(int);
    void reactOnUserInput();
    void onDownloadBtnClicked(int);
};