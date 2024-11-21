#include "proxy.h"

int main(int argc, char** argv)
{
    CachedYoutubeClass yt_proxy{};
    YoutubeManager manager(&yt_proxy);

    manager.reactOnUserInput();
    manager.onDownloadBtnClicked(1);
    return 0;
}