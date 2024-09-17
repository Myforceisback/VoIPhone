#include "MyCall.h"
#include "Magma.h"
#include <fstream>
void MyCall::onCallState(pj::OnCallStateParam& prm)
{
    PJ_UNUSED_ARG(prm);

    pj::CallInfo ci = getInfo();
    std::cout << "*** Call: " << ci.remoteUri << " [" << ci.stateText
        << "]" << std::endl;

    if (ci.state == PJSIP_INV_STATE_CONFIRMED)
    {
        // do stuff
    }
    else if (ci.state == PJSIP_INV_STATE_DISCONNECTED)
    {
        // do stuff related to call disconnection
        // 
        myAcc->removeCall(this);
        delete this;
        return;
    }
}

void MyCall::onCallMediaState(pj::OnCallMediaStateParam& media_param)
{
    if (this->encr == 0) {
        pj::CallInfo ci = getInfo();
        pj::AudioMedia* aud_med = NULL;
        Magma magma("keyboard", "mono");
        for (unsigned i = 0; i < ci.media.size(); ++i) {
            if (ci.media[i].type == PJMEDIA_TYPE_AUDIO) {
                aud_med = (pj::AudioMedia*)getMedia(i);
                break;
            }
        }
        pj::AudioMediaRecorder wav_writer;
        pj::AudioMedia& mic_media = pj::Endpoint::instance().audDevManager().getCaptureDevMedia();
        try {
            wav_writer.createRecorder("file.wav");
            mic_media.startTransmit(wav_writer);

        }
        catch (pj::Error& err) {
        }

        pj::AudioMediaPlayer player;
        pj::AudioMedia& speaker_media = pj::Endpoint::instance().audDevManager().getPlaybackDevMedia();
        try {
            player.createPlayer("Encrfile.wav");
            player.startTransmit(speaker_media);
        }
        catch (pj::Error& err) {
        }

        if (aud_med) {
            mic_media.startTransmit(*aud_med);
            aud_med->startTransmit(speaker_media);
        }
    }
    else {
        pj::CallInfo ci = getInfo();
        pj::AudioMedia* aud_med = NULL;
        Magma magma("keyboard", "mono");
        for (unsigned i = 0; i < ci.media.size(); ++i) {
            if (ci.media[i].type == PJMEDIA_TYPE_UNKNOWN) {
                aud_med = (pj::AudioMedia*)getMedia(i);
                break;
            }
        }
        pj::AudioMediaRecorder wav_writer;
        pj::AudioMedia& mic_media = pj::Endpoint::instance().audDevManager().getCaptureDevMedia();
        std::string file = "file.wav";
        try {
            wav_writer.createRecorder(file);
            mic_media.startTransmit(wav_writer);

        }
        catch (pj::Error& err) {
        }

        pj::AudioMediaPlayer player;
        pj::AudioMedia& speaker_media = pj::Endpoint::instance().audDevManager().getPlaybackDevMedia();
        try {
            player.createPlayer("Encrfile.wav");
            player.startTransmit(speaker_media);
        }
        catch (pj::Error& err) {
        }

        if (aud_med) {
            mic_media.startTransmit(*aud_med);
            aud_med->startTransmit(speaker_media);
        }
    }
}