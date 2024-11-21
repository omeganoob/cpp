#include <iostream>
#include "proxy.h"

void ThirdPartyYoutubeClass::listVideos()
{
    std::cout << "sending GET API...\n";
    std::cout << "video 1 \nvideo 2 \nvideo 3 \n";
}

void ThirdPartyYoutubeClass::getVideoInfo(int id)
{
    std::cout << "sending GET API with id: " + id << ".\n";
    std::cout << "name: video " << id << " \nlength: 10:20 \nchannel: somechanel \nupload date: 9/1/2024 \n";
}

void ThirdPartyYoutubeClass::downloadVideo(int id)
{
    std::cout << "sending download request with id: " + id << ".\n";
    std::cout << "video downloading.... \nvideo downloaded.\n";
}

CachedYoutubeClass::CachedYoutubeClass(ThirdPartyYoutubeLib* service) : _service(service)
{
}

void CachedYoutubeClass::listVideos()
{
    if (_service == nullptr || _needReset)
    {
        delete _service;
        _service = new ThirdPartyYoutubeClass();
    }
    _service->listVideos();
}

void CachedYoutubeClass::getVideoInfo(int id)
{
    if (_service == nullptr || _needReset)
    {
        delete _service;
        _service = new ThirdPartyYoutubeClass();
    }
    _service->getVideoInfo(id);
}

void CachedYoutubeClass::downloadVideo(int id)
{
    if (_service == nullptr || _needReset)
    {
        delete _service;
        _service = new ThirdPartyYoutubeClass();
    }
    _service->downloadVideo(id);
}

YoutubeManager::YoutubeManager(ThirdPartyYoutubeLib* service) : _service(service)
{
}

void YoutubeManager::renderVideoList()
{
    std::cout << "list of available videos: \n";
    _service->listVideos();
    std::cout << "--------------------------------\n";
}

void YoutubeManager::renderVideoPage(int id)
{
    std::cout << "render video info: \n";
    _service->getVideoInfo(id);
    std::cout << "--------------------------------\n";
}

void YoutubeManager::reactOnUserInput(int id)
{
    renderVideoPage(id);
    renderVideoList();
}

void YoutubeManager::reactOnUserInput()
{
    renderVideoPage(1);
    renderVideoList();
}

void YoutubeManager::onDownloadBtnClicked(int id)
{
    std::cout << "Saving video....\n";
    _service->downloadVideo(id);
    std::cout << "Video saved.\n";
}

